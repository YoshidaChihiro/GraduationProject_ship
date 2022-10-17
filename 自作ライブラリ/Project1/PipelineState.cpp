#include "PipelineState.h"
#include <d3dcompiler.h>
#include"PtrDelete.h"
std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> PipelineState::rootsignature = {};
std::unordered_map<std::string, D3D_PRIMITIVE_TOPOLOGY > PipelineState::primitiveTopologies = {};

std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> PipelineState::pipelinestate = {};


void PipelineState::SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType)
{
	if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
		assert(0);
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	cmdList->SetComputeRootSignature(rootsignature[keyName].Get());
	cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
}

void PipelineState::CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType)
{
	ID3D12Device* dev = DirectXLib::GetInstance()->GetDevice();
	HRESULT result;
	ComPtr < ID3DBlob> vsBlob = nullptr; //頂点シェーダオブジェクト
	ComPtr < ID3DBlob> psBlob = nullptr; //ピクセルシェーダオブジェクト
	ComPtr < ID3DBlob> gsBlob = nullptr; //ピクセルシェーダオブジェクト
	ComPtr < ID3DBlob> hsBlob = nullptr; //ハルシェーダオブジェクト
	ComPtr < ID3DBlob> dsBlob = nullptr; //ドメインシェーダオブジェクト

	ComPtr < ID3DBlob> csBlob = nullptr; //コンピュートシェーダオブジェクト

	ComPtr < ID3DBlob> errorBlob = nullptr; //エラーオブジェクト

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	//デスクリプタレンジの設定			
	std::vector<CD3DX12_DESCRIPTOR_RANGE*> descRangeSRVs;

		//デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//深度テストを行う
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); 
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//標準設定
	//ブレンドステートに設定する
	blenddesc.BlendEnable = true; // ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; // ソースの値を 100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; // デストの値を 0% 使う


	//ルートパラメータ―の配列
	std::vector<CD3DX12_ROOT_PARAMETER> rootparams = {};

	//頂点レイアウト
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};

	switch (shader)
	{
	case Bloom:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BlurPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 1,1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール
		break;
	}
	case BasePostEffect:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BasePostEffectPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 1, 4, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(),rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール

		break;
	}
	case NormalShadowMap:
	{
		CompileShader("ShadowMapVS", vsBlob, errorBlob, VS);
		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式

		SetDescriptorConstantBuffer(rootparams, 1, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書きルール

		break;
	}
	case FBXShadowMap:
	{
		CompileShader("ShadowMapFBXVS", vsBlob, errorBlob, VS);

		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	case GrassShadowMap:
	{
		CompileShader("NoConvertSMVS", vsBlob, errorBlob, VS);
		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);
		CompileShader("ShadowMapGrassGS", gsBlob, errorBlob, GS);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PBShadowMap:
	{
		CompileShader("NoConvertSMVS", vsBlob, errorBlob, VS);
		CompileShader("ShadowMapPS", psBlob, errorBlob, PS);
		CompileShader("ShadowMapPBGS", gsBlob, errorBlob, GS);
		CompileShader("ShadowMapPBHS", hsBlob, errorBlob, HS);
		CompileShader("ShadowMapPBDS", dsBlob, errorBlob, DS);

		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;//Rのみ32ビットのFLOAT形式
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			
		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		SetDescriptorConstantBuffer(rootparams, 2,0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		primitiveTopologies[keyName] = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

		break;
	}
	case DepthSPRITE:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SpritePixelShader", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;

		break;
	}

	case SPRITE:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SpritePixelShader", psBlob, errorBlob, PS);
		gpipeline.BlendState.AlphaToCoverageEnable = true;
		
		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		
		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case Migrate:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("MigratePS", psBlob, errorBlob, PS);
		gpipeline.BlendState.AlphaToCoverageEnable = true;

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 2, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case FisheyeLens:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("FisheyeLensPS", psBlob, errorBlob, PS);
		gpipeline.BlendState.AlphaToCoverageEnable = true;

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case NoAlphaToCoverageSprite:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SpritePixelShader", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case Ssao:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SSAOPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		gpipeline.BlendState.RenderTarget[0].BlendEnable = false;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		blenddesc.BlendEnable = false; // ブレンドを無効にする

		SetDescriptorConstantBuffer(rootparams, 2, 2, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SsaoCombine:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("SSAOCombinePS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
			
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 6, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case Blur:
	{
		CompileShader("SpriteVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BlurPS", psBlob, errorBlob, PS);

		//gpipeline.BlendState.AlphaToCoverageEnable = true;

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		primitiveTopologies[keyName] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case PARTICLE:
	{
		CompileShader("ParticleVS", vsBlob, errorBlob, VS);
		CompileShader("ParticlePS", psBlob, errorBlob, PS);
		CompileShader("ParticleGS", gsBlob, errorBlob, GS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD1a", inputLayout, DXGI_FORMAT_R32_UINT);
		SetVSLayout("ROTATION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("BILLBOARD", inputLayout, DXGI_FORMAT_R32_UINT);
		SetVSLayout("COLOR", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);
		SetVSLayout("SCALE", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("TEXCOORD4a", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("TEXCOORD0a", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD2a", inputLayout, DXGI_FORMAT_R32_UINT);

		// デプスの書き込みを禁止
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

		SetDescriptorConstantBuffer(rootparams, 1, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case GRASS:
	{
		CompileShader("GrassVS", vsBlob, errorBlob, VS);
		CompileShader("GrassPS", psBlob, errorBlob, PS);
		CompileShader("GrassGS", gsBlob, errorBlob, GS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 4, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


		break;
	}
	case PolygonBreak:
	{
		CompileShader("PolygonBreakVS", vsBlob, errorBlob, VS);
		CompileShader("PolygonBreakPS", psBlob, errorBlob, PS);
		CompileShader("PolygonBreakGS", gsBlob, errorBlob, GS);
		CompileShader("PolygonBreakHS", hsBlob, errorBlob, HS);
		CompileShader("PolygonBreakDS", dsBlob, errorBlob, DS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 5, 2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = CD3DX12_STATIC_SAMPLER_DESC(0);
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;
		//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		primitiveTopologies[keyName] = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;//0～255 指定の R

			
		break;
	}
	case BasicObj:
	{
		CompileShader("BasicVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BasicPixelShader", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case SkyDome:
	{
		CompileShader("SkyVS", vsBlob, errorBlob, VS);
		CompileShader("SkyPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		gpipeline.NumRenderTargets = 2;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA

		break;
	}
	case UVScrolling:
	{
		CompileShader("AreaEffectVS", vsBlob, errorBlob, VS);
		CompileShader("UVScrollingPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 1, descRangeSRVs);
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case AREAEFFECT:
	{
		CompileShader("AreaEffectVS", vsBlob, errorBlob, VS);
		CompileShader("AreaEffectPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 1, descRangeSRVs);
		gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case BloomObj:
	{
		CompileShader("BasicVertexShader", vsBlob, errorBlob, VS);
		CompileShader("BloomOBJPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA


		break;
	}

	case NormalMap:
	{
		CompileShader("NormalMapVS", vsBlob, errorBlob, VS);
		CompileShader("NormalMapPS", psBlob, errorBlob, PS);
		CompileShader("NormalMapGS", gsBlob, errorBlob, GS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 2, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case FBX:
	{
		//gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA

		CompileShader("FBXVS", vsBlob, errorBlob, VS);
		CompileShader("FBXPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);
		//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		SetDescriptorConstantBuffer(rootparams, 4 ,2, descRangeSRVs);
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;//0～255 指定の RGBA

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case FBXPolygonBreak:
	{
		CompileShader("FBXPolygonBreakVS", vsBlob, errorBlob, VS);
		CompileShader("FBXPolygonBreakPS", psBlob, errorBlob, PS);
		CompileShader("FBXPolygonBreakGS", gsBlob, errorBlob, GS);
		CompileShader("FBXPolygonBreakHS", hsBlob, errorBlob, HS);
		CompileShader("FBXPolygonBreakDS", dsBlob, errorBlob, DS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);
		SetVSLayout("BONEINDICES", inputLayout, DXGI_FORMAT_R32G32B32A32_UINT);
		SetVSLayout("BONEWEIGHTS", inputLayout, DXGI_FORMAT_R32G32B32A32_FLOAT);
			
		SetDescriptorConstantBuffer(rootparams, 5, 2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		primitiveTopologies[keyName] = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;//0～255 指定の R

		break;
	}

	case NoShade:
	{
		CompileShader("NoShadeVS", vsBlob, errorBlob, VS);
		CompileShader("NoShadePS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 2, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case PIECHART:
	{
		CompileShader("NoShadeVS", vsBlob, errorBlob, VS);
		CompileShader("PieChartPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 3, 1, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}

	case DrawShadowOBJ:
	{
		CompileShader("DrawShadowOBJVS", vsBlob, errorBlob, VS);
		CompileShader("DrawShadowOBJPS", psBlob, errorBlob, PS);

		SetVSLayout("POSITION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("NORMAL", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("TEXCOORD", inputLayout, DXGI_FORMAT_R32G32_FLOAT);

		SetDescriptorConstantBuffer(rootparams, 4,2, descRangeSRVs);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc2 = samplerDesc;
		samplerDesc2.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc2.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//D3D12_COMPARISON_FUNC_GREATER;
		samplerDesc2.MaxAnisotropy = 1;
		samplerDesc2.ShaderRegister = 1;

		CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = { samplerDesc,samplerDesc2 };
		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 2, samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		gpipeline.NumRenderTargets = 3;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255 指定の RGBA
		gpipeline.RTVFormats[2] = DXGI_FORMAT_R32_FLOAT;//0～255 指定の R


		break;
	}
	case ViewCollisionBox:
	{
		CompileShader("ViewCollisionBoxVS", vsBlob, errorBlob, VS);
		CompileShader("ViewCollisionBoxGS", gsBlob, errorBlob, GS);
		CompileShader("ViewCollisionBoxPS", psBlob, errorBlob, PS);

		//頂点レイアウト
		SetVSLayout("CENTER", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("SCALE", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("OBJCENTER", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("ROTATION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("COLROTATION", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
				
		// デプスの書き込みを禁止
		//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

		SetDescriptorConstantBuffer(rootparams, 1, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		break;
	}
	case ViewCollisionSphere:
	{
		CompileShader("ViewCollisionSphereVS", vsBlob, errorBlob, VS);
		CompileShader("ViewCollisionSphereGS", gsBlob, errorBlob, GS);
		CompileShader("ViewCollisionSpherePS", psBlob, errorBlob, PS);

		//頂点レイアウト
		SetVSLayout("CENTER", inputLayout, DXGI_FORMAT_R32G32B32_FLOAT);
		SetVSLayout("RADIUS", inputLayout, DXGI_FORMAT_R32_FLOAT);

		// デプスの書き込みを禁止
		//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		primitiveTopologies[keyName] = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;

		SetDescriptorConstantBuffer(rootparams, 1, 0, descRangeSRVs);

		rootSignatureDesc.Init_1_0(rootparams.size(), rootparams.data(), 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


		break;
	}
	default:
		assert(0);
		break;
	}

	gpipeline.InputLayout.pInputElementDescs = inputLayout.data();
	gpipeline.InputLayout.NumElements = inputLayout.size();

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	if (gsBlob != nullptr)
		gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	if (hsBlob != nullptr)
		gpipeline.HS = CD3DX12_SHADER_BYTECODE(hsBlob.Get());
	if (dsBlob != nullptr)
		gpipeline.DS = CD3DX12_SHADER_BYTECODE(dsBlob.Get());
	
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	

	gpipeline.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値のフォーマット

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//static HRESULT r;
	//r = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//printf("%d\n", r);
	assert(SUCCEEDED(result));
	// ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature[keyName]));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = rootsignature[keyName].Get();

	if (pipelinestate[keyName][blendType] != nullptr)
		pipelinestate[keyName][blendType]->Release();
	switch (blendType)
	{
	case NOBLEND:
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][NOBLEND]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case ALPHA:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;
		if (shader == BasicObj)
		{
			gpipeline.BlendState.RenderTarget[1] = blenddesc;
			gpipeline.BlendState.RenderTarget[2] = blenddesc;
		}

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ALPHA]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	case ADD:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][ADD]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case SUB:
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blenddesc.SrcBlend = D3D12_BLEND_ONE;
		blenddesc.DestBlend = D3D12_BLEND_ONE;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][SUB]));
		if (FAILED(result)) {
			assert(0);
		}

		break;
	case COLORFLIP:
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		gpipeline.BlendState.RenderTarget[0] = blenddesc;

		result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate[keyName][COLORFLIP]));
		if (FAILED(result)) {
			assert(0);
		}
		break;
	default:
		break;
	}
	
	//作ったレンジを削除
	for(auto it = descRangeSRVs.begin();it!=descRangeSRVs.end();++it)
	{
		PtrDelete(*it);
	}
}

void PipelineState::CompileShader(const std::string& shaderName, ComPtr < ID3DBlob>& shaderBlob, ComPtr < ID3DBlob>& errorBlob, const SHADER shaderType)
{
	// ファイルパスを結合
	std::string filepath = "Resources/Shader/" + shaderName+".hlsl";
	wchar_t wfilepath[128];

	// ユニコード文字列に変換
	MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	LPCSTR entryName = "", modelName = "";
	switch (shaderType)
	{
	case VS:
		entryName = "VSmain";
		modelName = "vs_5_0";
		break;
	case HS:
		entryName = "HSmain";
		modelName = "hs_5_0";
		break;
	case DS:
		entryName = "DSmain";
		modelName = "ds_5_0";
		break;
	case GS:
		entryName = "GSmain";
		modelName = "gs_5_0";
		break;
	case PS:
		entryName = "PSmain";
		modelName = "ps_5_0";
		break;

	default:
		break;
	}

	auto result = D3DCompileFromFile(
		wfilepath,  //シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		entryName, modelName,//エントリーポイント名、シェーダモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0, &shaderBlob, &errorBlob);

	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

}

void PipelineState::SetVSLayout(const LPCSTR& semantics, std::vector<D3D12_INPUT_ELEMENT_DESC>& arg_layouts, const DXGI_FORMAT& format)
{
	D3D12_INPUT_ELEMENT_DESC layout =
	{
		semantics, 0, format, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}; //uv座標
	arg_layouts.push_back(layout);

}

void PipelineState::SetDescriptorConstantBuffer(std::vector<CD3DX12_ROOT_PARAMETER>& arg_rootparams, const int arg_constBuffNum, const int arg_descriptorNum, std::vector<CD3DX12_DESCRIPTOR_RANGE*>& arg_descRangeSRVs)
{
	int constBuffNumber = 0;
	int descriptorNumber = 0;

	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER param;
	arg_descRangeSRVs.resize(arg_descriptorNum);

	//モデルのテクスチャの場所固定してしまっているため複数のコンストバッファとテクスチャ領域を作るときに無理やり合うようにしている
	
	if (arg_constBuffNum > 1)
	{
		for (; constBuffNumber < 2; constBuffNumber++)
		{
			param.InitAsConstantBufferView(constBuffNumber, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
			arg_rootparams.push_back(param);
		}
	}
	else if(arg_constBuffNum == 1)
	{
		param.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		arg_rootparams.push_back(param);
		constBuffNumber++;
	}

	if(arg_descriptorNum != 0)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL); //種類
		arg_rootparams.push_back(param);
		descriptorNumber++;
	}

	for (; constBuffNumber < arg_constBuffNum; constBuffNumber++)
	{
		param.InitAsConstantBufferView(constBuffNumber, 0, D3D12_SHADER_VISIBILITY_ALL); //種類
		arg_rootparams.push_back(param);
	}
	
	for (; descriptorNumber < arg_descriptorNum; descriptorNumber++)
	{
		arg_descRangeSRVs[descriptorNumber] = new CD3DX12_DESCRIPTOR_RANGE;
		arg_descRangeSRVs[descriptorNumber]->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, descriptorNumber);

		param.InitAsDescriptorTable(1, arg_descRangeSRVs[descriptorNumber], D3D12_SHADER_VISIBILITY_ALL); //種類
		arg_rootparams.push_back(param);
	}

}

