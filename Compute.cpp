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

void Compute::Dispatch(CommandContext& commandContext)
{
	commandContext.TransitionResource(rwStructureBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetComputeRootSignature(rootSignature_);
	commandContext.SetComputeUAVBuffer(0, rwStructureBuffer_->GetGPUVirtualAddress());
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

	CD3DX12_ROOT_PARAMETER rootparams[1]{};
	rootparams[0].InitAsUnorderedAccessView(0);

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.pParameters = rootparams;
	rootSignatureDesc.NumParameters = _countof(rootparams);

	rootSignature_.Create(rootSignatureDesc);

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSignature_;
	desc.CS = CD3DX12_SHADER_BYTECODE(uavBlob->GetBufferPointer(), uavBlob->GetBufferSize());

	pipelineState_.Create(desc);

}
