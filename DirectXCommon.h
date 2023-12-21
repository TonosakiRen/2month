#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHandle.h"
#include "DescriptorHeap.h"
#include "CommandQueue.h"

class DirectXCommon
{
public:

	//うんち設計
	uint32_t kSrvHeapDescritorNum = 1024;
	uint32_t kRtvHeapDescritorNum = 32;

	//mainColorBufferNum
	uint32_t kMainColorBufferNum = 1;

	static DirectXCommon* GetInstance();
	void Initialize();
	void Shutdown();

	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12Device* GetDevice() { return device_.Get(); }
	CommandQueue& GetCommandQueue() { return commandQueue_; }

	DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return descriptorHeaps_[type]; }

private:

	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	CommandQueue commandQueue_;

	DescriptorHeap descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

private:
	void CreateDevice();
};

