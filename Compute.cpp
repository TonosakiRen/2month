#include "Compute.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>

using namespace Microsoft::WRL;

RootSignature Compute::rootSignature_;
PipelineState Compute::pipelineState_;

void Compute::StaticInitialize()
{
	CreatePipeline();
}

void Compute::Initialize()
{
	auto device = DirectXCommon::GetInstance()->GetDevice();
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(uint32_t) * kNum), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	D3D12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(rwStructureBuffer_.GetAddressOf())
	);

	uavHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc{};
	viewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	viewDesc.Buffer.NumElements = kNum;
	viewDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	device->CreateUnorderedAccessView(
		rwStructureBuffer_,
		nullptr,
		&viewDesc,
		uavHandle_
	);

	CD3DX12_RESOURCE_DESC copyDesc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(uint32_t) * kNum));
	D3D12_HEAP_PROPERTIES copyHeapProps(D3D12_HEAP_TYPE_READBACK);
	device->CreateCommittedResource(
		&copyHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&copyDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(copyBuffer_.GetAddressOf())
	);

	copyBuffer_->Map(0, nullptr, &data_);
}

void Compute::Dispatch(CommandContext& commandContext, ColorBuffer* colorBuffer1, ColorBuffer* colorBuffer2)
{

	commandContext.TransitionResource(rwStructureBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetComputeRootSignature(rootSignature_);
	commandContext.SetComputeUAVBuffer(0, rwStructureBuffer_->GetGPUVirtualAddress());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture1), colorBuffer1->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture2), colorBuffer2->GetSRV());
	commandContext.Dispatch(kNum,1,1);
	commandContext.CopyBuffer(copyBuffer_, rwStructureBuffer_);
	
}

void* Compute::GetData()
{
	return data_;
}

void Compute::UnMap()
{
	copyBuffer_->Unmap(0, nullptr);
}

void Compute::CreatePipeline()
{
	ComPtr<IDxcBlob> uavBlob;

	auto shaderManager = ShaderManager::GetInstance();
	uavBlob = shaderManager->Compile(L"CS.hlsl", ShaderManager::kCompute);
	assert(uavBlob != nullptr);

	CD3DX12_DESCRIPTOR_RANGE ranges[2]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

	CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::ParameterNum)]{};
	rootparams[(int)RootParameter::kRwStructure].InitAsUnorderedAccessView(0);
	rootparams[(int)RootParameter::kColorTexture1].InitAsDescriptorTable(1, &ranges[0]);
	rootparams[(int)RootParameter::kColorTexture2].InitAsDescriptorTable(1, &ranges[1]);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
		CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.pParameters = rootparams;
	rootSignatureDesc.NumParameters = _countof(rootparams);
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSignature_.Create(rootSignatureDesc);

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_;
	desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

	pipelineState_.Create(desc);

}
