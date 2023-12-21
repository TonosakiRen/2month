#include "UploadBuffer.h"

#include <cassert>
#include "DirectXCommon.h"

UploadBuffer::~UploadBuffer() {
    Destroy();
}

void UploadBuffer::Create(size_t bufferSize) {
    HRESULT result = S_FALSE;
    auto device = DirectXCommon::GetInstance()->GetDevice();

    Destroy();

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(bufferSize));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    result = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf()));

    assert(SUCCEEDED(result));

    state_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = bufferSize;

    result =  resource_->Map(0, nullptr, &cpuData_);
    assert(SUCCEEDED(result));
}

void UploadBuffer::Create(size_t numElements, size_t elementSize) {
    Create(numElements * elementSize);
}

void UploadBuffer::Copy(const void* srcData, size_t copySize) const {
    assert(copySize <= bufferSize_);
    memcpy(cpuData_, srcData, copySize);
}

void UploadBuffer::Destroy() {
    if (cpuData_ && resource_) {
        resource_->Unmap(0, nullptr);
        resource_.Reset();
        cpuData_ = nullptr;
    }
}
