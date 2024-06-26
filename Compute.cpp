#include "Compute.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>
#include "ShadowSpotLights.h"
#include "Game/Character/EnemyManager.h"
#include "ViewProjection.h"

using namespace Microsoft::WRL;

void* Compute::data_ = nullptr;
void* Compute::hitPosData_ = nullptr;

void Compute::Initialize(ColorBuffer* indexBuffer)
{

	CreatePipeline();

	auto device = DirectXCommon::GetInstance()->GetDevice();

	{

		uint32_t bufferSize;
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(bufferSize) * EnemyManager::kMaxEnemyCount), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
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
		viewDesc.Buffer.NumElements = 1;
		viewDesc.Buffer.StructureByteStride = sizeof(bufferSize) * EnemyManager::kMaxEnemyCount;
		device->CreateUnorderedAccessView(
			rwStructureBuffer_,
			nullptr,
			&viewDesc,
			uavHandle_
		);

		CD3DX12_RESOURCE_DESC copyDesc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(bufferSize) * EnemyManager::kMaxEnemyCount));
		D3D12_HEAP_PROPERTIES copyHeapProps(D3D12_HEAP_TYPE_READBACK);
		device->CreateCommittedResource(
			&copyHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&copyDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(copyBuffer_.GetAddressOf())
		);

		copyBuffer_->Map(0, nullptr, &data_);

	}

	{
		Vector3 bufferSize;
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(bufferSize)), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		D3D12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(hitPosBuffer_.GetAddressOf())
		);

		uavHandle2_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc{};
		viewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		viewDesc.Buffer.NumElements = 1;
		viewDesc.Buffer.StructureByteStride = sizeof(bufferSize);
		device->CreateUnorderedAccessView(
			hitPosBuffer_,
			nullptr,
			&viewDesc,
			uavHandle2_
		);

		CD3DX12_RESOURCE_DESC copyDesc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(sizeof(bufferSize)));
		D3D12_HEAP_PROPERTIES copyHeapProps(D3D12_HEAP_TYPE_READBACK);
		device->CreateCommittedResource(
			&copyHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&copyDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(hitPosCopyBuffer_.GetAddressOf())
		);

		hitPosCopyBuffer_->Map(0, nullptr, &hitPosData_);
	}

	indexBuffer_ = indexBuffer;
}

void Compute::Dispatch(CommandContext& commandContext,DepthBuffer& depthBuffer,const ViewProjection& viewProjection)
{
	D3D12_RESOURCE_DESC resourceDesc = copyBuffer_->GetDesc();
	UINT bufferSizeInBytes = static_cast<UINT>(resourceDesc.Width);
	memset(data_, 0, bufferSizeInBytes);

	commandContext.TransitionResource(rwStructureBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.TransitionResource(hitPosBuffer_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandContext.TransitionResource(depthBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	commandContext.TransitionResource(*indexBuffer_, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetComputeRootSignature(rootSignature_);
	commandContext.SetComputeUAVBuffer(0, rwStructureBuffer_->GetGPUVirtualAddress());
	commandContext.SetComputeUAVBuffer(1, hitPosBuffer_->GetGPUVirtualAddress());
	commandContext.SetComputeConstants(static_cast<UINT>(RootParameter::kEnemyNum), EnemyManager::kMaxEnemyCount);

	commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture), indexBuffer_->GetSRV());
	commandContext.SetComputeDescriptorTable(static_cast<UINT>(RootParameter::kDepthTexture), depthBuffer.GetSRV());
	commandContext.SetComputeConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection),viewProjection.GetGPUVirtualAddress());
	commandContext.Dispatch(1920 / 30, 1080 / 30, 1);

	commandContext.CopyBuffer(copyBuffer_, rwStructureBuffer_);
	commandContext.CopyBuffer(hitPosCopyBuffer_, hitPosBuffer_);

}

void* Compute::GetData()
{
	return data_;
}

void* Compute::GetPosData()
{
	return hitPosData_;
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
	rootparams[(int)RootParameter::kHitPos].InitAsUnorderedAccessView(1);
	rootparams[(int)RootParameter::kColorTexture].InitAsDescriptorTable(1, &ranges[0]);
	rootparams[(int)RootParameter::kDepthTexture].InitAsDescriptorTable(1, &ranges[1]);
	rootparams[(int)RootParameter::kEnemyNum].InitAsConstants(1, 0);
	rootparams[(int)RootParameter::kViewProjection].InitAsConstantBufferView(1, 0);

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
