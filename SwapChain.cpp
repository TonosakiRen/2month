#include "SwapChain.h"

#include "Helper.h"
#include <assert.h>

#include "DirectXCommon.h"

using namespace Microsoft::WRL;

void SwapChain::Create(HWND hWnd) {

    ComPtr<IDXGIFactory7> factory;
    Helper::AssertIfFailed(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

    RECT clientRect{};
    if (!GetClientRect(hWnd, &clientRect)) {
        assert(false);
    }

    HDC hdc = GetDC(hWnd);
    refreshRate_ = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(hWnd, hdc);

    LONG clientWidth = clientRect.right - clientRect.left;
    LONG clientHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = UINT(clientWidth); // 画面幅
    desc.Height = UINT(clientHeight); // 画面高
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
    desc.SampleDesc.Count = 1; // マルチサンプルしない
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画ターゲットとして利用する
    desc.BufferCount = kNumBuffers;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに移したら、中身を破棄

    Helper::AssertIfFailed((factory->CreateSwapChainForHwnd(
        DirectXCommon::GetInstance()->GetCommandQueue(),
        hWnd,
        &desc,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.ReleaseAndGetAddressOf()))));

    for (uint32_t i = 0; i < kNumBuffers; ++i) {
        ComPtr<ID3D12Resource> resource;
        Helper::AssertIfFailed(swapChain_->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf())));
        buffers_[i] = std::make_unique<ColorBuffer>();
        buffers_[i]->CreateFromSwapChain(resource.Detach());
    }
}

void SwapChain::Present() {
    static constexpr int32_t kThreasholdRefreshRate = 58;
    swapChain_->Present(refreshRate_ < kThreasholdRefreshRate ? 0 : 1, 0);
    currentBufferIndex_ = (currentBufferIndex_ + 1) % kNumBuffers;
}
