#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <chrono>

#include "DescriptorHandle.h"
#include "ColorBuffer.h"

class SwapChain {
public:
    static const uint32_t kNumBuffers = 3;

    void Create(HWND hWnd);
    void Present();

    ColorBuffer& GetColorBuffer() { return *buffers_[currentBufferIndex_]; }
    const ColorBuffer& GetColorBuffer() const { return *buffers_[currentBufferIndex_]; }
    uint32_t GetBufferIndex() const { return currentBufferIndex_; }
    const DescriptorHandle& GetRTV() const { return buffers_[currentBufferIndex_]->GetRTV(); }

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    std::unique_ptr<ColorBuffer> buffers_[kNumBuffers];
    std::chrono::steady_clock::time_point reference_;
    uint32_t currentBufferIndex_ = 0;
    int32_t refreshRate_ = 0;
};