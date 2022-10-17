#pragma once
#include<wrl.h>
#include<d3dx12.h>
#include"DirectXLib.h"
#include<unordered_map>
using namespace Microsoft::WRL;

class DirectXLib;
enum BLENDTYPE
{
	NOBLEND,
	ALPHA,
	ADD,
	SUB,
	COLORFLIP,
};
enum ShaderType
{
	PolygonBreak,
	GRASS,
	FBX,
	FBXPolygonBreak,
	NoShade,
	PARTICLE,
	SPRITE,
	NoAlphaToCoverageSprite,
	BasePostEffect,
	BasicObj,
	BloomObj,
	Bloom,
	NormalMap,
	NormalShadowMap,
	PBShadowMap,
	FBXShadowMap,
	GrassShadowMap,
	DrawShadowOBJ,
	GPUPARTICLE,
	ViewCollisionBox,
	ViewCollisionSphere,
	SkyDome,
	Blur,
	Ssao,
	SsaoCombine,
	AREAEFFECT,
	FisheyeLens,
	Migrate,
	DepthSPRITE,
	PIECHART,
	UVScrolling,
};

class PipelineState
{
public:
	inline static void SetPipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA)
	{
		if (rootsignature[keyName].Get() == nullptr || pipelinestate[keyName][blendType].Get() == nullptr)
			assert(0);
		auto cmdList = DirectXLib::GetInstance()->GetCommandList();
		cmdList->SetGraphicsRootSignature(rootsignature[keyName].Get());
		cmdList->SetPipelineState(pipelinestate[keyName][blendType].Get());
		cmdList->IASetPrimitiveTopology(primitiveTopologies[keyName]);

	}
	static void SetComputePipeline(const std::string& keyName, const BLENDTYPE blendType = ALPHA);

	static void CreatePipeline(const std::string& keyName, const ShaderType shader, const BLENDTYPE blendType = ALPHA);
private:
	DirectXLib* device;

	static std::unordered_map<std::string, D3D_PRIMITIVE_TOPOLOGY>primitiveTopologies;
	static std::unordered_map<std::string,ComPtr<ID3D12RootSignature>>rootsignature;
	static std::unordered_map<std::string, std::unordered_map<BLENDTYPE, ComPtr<ID3D12PipelineState>>> pipelinestate;
	
	enum SHADER
	{
		VS,
		GS,
		HS,
		DS,
		PS,
	};

	static void CompileShader(const std::string& shaderName, ComPtr < ID3DBlob>& shaderBlob, ComPtr < ID3DBlob>& errorBlob, const SHADER shaderType);
	static void SetVSLayout(const LPCSTR& semantics, std::vector<D3D12_INPUT_ELEMENT_DESC>& arg_layouts, const DXGI_FORMAT& format);
	static void SetDescriptorConstantBuffer(std::vector<CD3DX12_ROOT_PARAMETER>& arg_rootparams, const int arg_constBuffNum, const int arg_descriptorNum, std::vector<CD3DX12_DESCRIPTOR_RANGE*>& arg_descRangeSRVs);
};

