#pragma once
#include "PixelBuffer.h"

#include "DescriptorHandle.h"

class ColorBuffer : public PixelBuffer {
public:
    void CreateFromSwapChain(ID3D12Resource* resource);
    void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);
    void CreateArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format);

    void SetClearColor(const float* clearColor);
    const float* GetClearColor() const { return clearColor_; }

    const DescriptorHandle& GetSRV() const { return srvHandle_; }
    const DescriptorHandle& GetRTV() const { return rtvHandle_; }

private:
    void CreateViews();

    float clearColor_[4]{ 0.0f,0.0f,0.0f,0.0f };
    DescriptorHandle srvHandle_;
    DescriptorHandle rtvHandle_;
};
