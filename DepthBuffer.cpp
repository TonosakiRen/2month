#include "DepthBuffer.h"

#include "DirectXCommon.h"

DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;

    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;

    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        return DXGI_FORMAT_D16_UNORM;

    default:
        break;
    }
    return format;
};
DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;

    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        return DXGI_FORMAT_R16_UNORM;

    default:
        break;
    }
    return DXGI_FORMAT_UNKNOWN;
}

void DepthBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    auto desc = DescribeTex2D(width, height, 1, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = clearValue_;

    CreateTextureResource(desc, clearValue);
    CreateViews();
}

void DepthBuffer::CreateArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    auto desc = DescribeTex2D(width, height, arraySize, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = clearValue_;

    CreateTextureResource(desc, clearValue);
    CreateViews();
}

void DepthBuffer::CreateViews() {
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    dsvDesc.Format = GetDSVFormat(format_);
    srvDesc.Format = GetDepthFormat(format_);
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (arraySize_ > 1) {
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.MipSlice = 0;
        dsvDesc.Texture2DArray.FirstArraySlice = 0;
        dsvDesc.Texture2DArray.ArraySize = UINT(arraySize_);

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = UINT(arraySize_);
    }
    else {
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }

    auto graphics = DirectXCommon::GetInstance();
    if (dsvHandle_.IsNull()) {
        dsvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    auto device = graphics->GetDevice();
    device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
