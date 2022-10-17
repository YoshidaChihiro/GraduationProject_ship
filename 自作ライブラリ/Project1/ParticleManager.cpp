#include "ParticleManager.h"

#include "ComputeWrapper.h"
#include"DirectXLib.h"
#include"PipelineState.h"
#include"Texture.h"
#include "ModelParticle.h"

ParticleManager* ParticleManager::GetInstance()
{
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Add(Particle* newParticle, const std::string& TexName)
{
    paramData[TexName].push_back(newParticle->parameter);
    vertData[TexName].push_back(newParticle->vsParam);
    activParticleCount++;
}

void ParticleManager::Initialize()
{
    CreateConstBuff();
    CreateModel();
    computeShade = std::make_unique<ComputeShade>(vertexCount);
    ComputeWrapper::GetInstance()->AddShade(computeShade.get());
}

void ParticleManager::Update()
{
    HRESULT result;

    //------------------------------コンピュートシェーダーにデータを転送------------------------
    params.clear();
    params.resize(activParticleCount);
    int paramOffset = 0;
    for (auto it : paramData)
    {
        const int size = it.second.size();
        if (size <= 0)
        {
            continue;
        }
        memcpy(params.data() + paramOffset, it.second.data(), sizeof(ParticleParameter) * size);
        paramOffset += size;
    }
    verts.clear();
    verts.resize(activParticleCount);
    int vertOffset = 0;
    for (auto it : vertData)
    {
        const int size = it.second.size();
        if (size <= 0)
        {
            continue;
        }

        memcpy(verts.data() + vertOffset, it.second.data(), sizeof(OutputData) * size);
        vertOffset += size;
    }
    computeShade->Update(params.data(), verts.data(), activParticleCount);
    //------------------------------------------------------------------------------------------------
        // 定数バッファへデータ転送
    ConstBufferData* constMap = nullptr;
    result = constBuff->Map(0, nullptr, (void**)&constMap);
    assert(SUCCEEDED(result));
    constMap->mat = camera->GetMatViewProjection();
    constMap->matBillboard = camera->GetMatBillboard();
    constBuff->Unmap(0, nullptr);
}

void ParticleManager::Draw()
{
    auto cmdList = DirectXLib::GetInstance()->GetCommandList();
    verts.clear();
    verts.resize(activParticleCount);

    memcpy(verts.data(), computeShade->GetOutputData(), sizeof(OutputData) * activParticleCount);

    int offset = 0;
    for (auto it = vertData.begin(); it != vertData.end(); ++it)
    {
        const int size = it->second.size();
        if (size <= 0)
        {
            continue;
        }
        memcpy(it->second.data(), verts.data() + offset, sizeof(OutputData) * size);
        offset += size;
    }

    for (auto itr = vertData.begin(); itr != vertData.end(); ++itr)
    {
        for (auto it = itr->second.begin(); it != itr->second.end();)
        {
            if ((*it).isDead == 0)
            {
                ++it;
                continue;
            }
            it = itr->second.erase(it);
            paramData[itr->first].erase(paramData[itr->first].begin() + (int)std::distance(itr->second.begin(), it));
            if (activParticleCount > 0)
                activParticleCount--;
            if (itr->second.size() == 0)
            {
                itr->second.clear();
                paramData[itr->first].clear();
                break;
            }
        }
        if (activParticleCount <= 0)
        {
            break;
        }
    }
    verts.clear();
    verts.resize(activParticleCount);
    int vertOffset = 0;
    for (auto it = vertData.begin(); it != vertData.end(); ++it)
    {
        const int size = it->second.size();
        if (size <= 0)
        {
            continue;
        }
        memcpy(verts.data() + vertOffset, it->second.data(), sizeof(OutputData) * size);
        vertOffset += size;
    }

    const auto outPutPtr = verts.data();
    OutputData* vertMap = nullptr;
    auto result = vertBuff->Map(0, nullptr, (void**)&vertMap);
    if (SUCCEEDED(result)) {
        memcpy(vertMap, outPutPtr, sizeof(OutputData) * activParticleCount);
        vertBuff->Unmap(0, nullptr);
    }
    PipelineState::SetPipeline("Particle", ADD);

    // 頂点バッファの設定
    cmdList->IASetVertexBuffers(0, 1, &vbView);

    // デスクリプタヒープの配列
    ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
    cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // 定数バッファビューをセット
    cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
    int count = 0;
    for (auto it : vertData)
    {
        // パーティクルが1つもない場合
        if (it.second.empty())
            continue;
        UINT drawNum = it.second.size();
        if (drawNum > vertexCount) {
            drawNum = vertexCount;
        }
        // シェーダリソースビューをセット
        cmdList->SetGraphicsRootDescriptorTable(1, Texture::GetGpuDescHandleSRV(it.first));
        // 描画コマンド
        cmdList->DrawInstanced(drawNum, 1, count, 0);
        count += drawNum;
    }
}

void ParticleManager::CreateConstBuff()
{
    HRESULT result;
    result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constBuff));
    if (FAILED(result)) {
        assert(0);
    }

}

void ParticleManager::CreateModel()
{
    HRESULT result = S_FALSE;
    auto dev = DirectXLib::GetInstance()->GetDevice();
    // 頂点バッファ生成
    result = dev->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData) * vertexCount),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));
    if (FAILED(result)) {
        assert(0);
        return;
    }

    // 頂点バッファビューの作成
    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeof(OutputData) * vertexCount;
    vbView.StrideInBytes = sizeof(OutputData);
}

void ParticleManager::End()
{
}

void ParticleManager::AddFeverCutEffect(FeverCutEffect* arg_effect)
{
    feverCutEffects.push_back(arg_effect);
}


void ParticleManager::AddDeadEffect(DeadEffectParticle* arg_effect)
{
    deadEffects.push_back(arg_effect);
}

void ParticleManager::DeleteDeadEffect()
{
    for (auto itr = deadEffects.begin(); itr != deadEffects.end();)
    {
        if ((*itr)->IsDead())
        {
            delete* itr;
            itr = deadEffects.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

void ParticleManager::UpdateDeadEffect()
{
    for (int i = 0; i < deadEffects.size(); i++)
    {
        deadEffects[i]->Update();
    }
}

void ParticleManager::DrawDeadEffect()
{
    for (int i = 0; i < deadEffects.size(); i++)
    {
        deadEffects[i]->Draw();
    }
}

void ParticleManager::ClearDeadEffect()
{
    for (auto itr = deadEffects.begin(); itr != deadEffects.end();)
    {
        delete* itr;
        itr = deadEffects.erase(itr);
    }
    deadEffects.clear();
}
