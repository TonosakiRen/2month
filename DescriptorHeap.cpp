#include "DescriptorHeap.h"

#include <cassert>

#include "DirectXCommon.h"

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
    assert(type != D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
    HRESULT hr = S_FALSE;

    auto device = DirectXCommon::GetInstance()->GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) {
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }
    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap_.ReleaseAndGetAddressOf()));
    assert(SUCCEEDED(hr));

    type_ = type;
    descriptorStart_.cpu_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    if (desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        descriptorStart_.gpu_ = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    }
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(type_);
    numDescriptors_ = numDescriptors;
    freeDescriptorOffset_ = 0;
}

DescriptorHandle DescriptorHeap::Allocate() {
    assert(freeDescriptorOffset_ <= numDescriptors_);

    DescriptorHandle allocationHandle;
    allocationHandle.cpu_ = descriptorStart_;
    allocationHandle.cpu_.ptr += uint64_t(freeDescriptorOffset_) * descriptorSize_;
    if (descriptorStart_.IsShaderVisible()) {
        allocationHandle.gpu_ = descriptorStart_;
        allocationHandle.gpu_.ptr += uint64_t(freeDescriptorOffset_) * descriptorSize_;
    }
    ++freeDescriptorOffset_;

    return allocationHandle;
}

void DescriptorHeap::Reset(uint32_t reallocationOffset) {
    assert(reallocationOffset < numDescriptors_);
    freeDescriptorOffset_ = reallocationOffset;
}
