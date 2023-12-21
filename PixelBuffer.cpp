#include "PixelBuffer.h"

#include "Externals/DirectXTex/d3dx12.h"


#include "Helper.h"
#include <assert.h>

#include "DirectXCommon.h"

void PixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& desc, D3D12_CLEAR_VALUE clearValue) {
    resource_.Reset();

    auto device = DirectXCommon::GetInstance()->GetDevice();

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    Helper::AssertIfFailed(device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE,
        &desc, D3D12_RESOURCE_STATE_COMMON,
        &clearValue, IID_PPV_ARGS(resource_.GetAddressOf())));

    state_ = D3D12_RESOURCE_STATE_COMMON;

}

void PixelBuffer::AssociateWithResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) {
    assert(resource);
    auto desc = resource->GetDesc();

    resource_.Attach(resource);
    state_ = state;

    width_ = uint32_t(desc.Width);
    height_ = desc.Height;
    arraySize_ = desc.DepthOrArraySize;
    format_ = desc.Format;
}

D3D12_RESOURCE_DESC PixelBuffer::DescribeTex2D(
    uint32_t width, uint32_t height, uint32_t arraySize,
    DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags) {
    width_ = width;
    height_ = height;
    arraySize_ = arraySize;
    format_ = format;

    return CD3DX12_RESOURCE_DESC::Tex2D(format, UINT64(width), height, UINT16(arraySize), 1, 1, 0, flags);
}

