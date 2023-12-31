#pragma once

#include "DirectXCommon.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Bloom.h"
#include "PostEffect.h"

#include "DeferredRenderer.h"
#include "EdgeRenderer.h"

class viewProjection;
class DirectionalLights;

class Renderer
{
public:

    enum RenderTargetType {
        kColor,
        kNormal,

        kRenderTargetNum
    };

	static Renderer* GetInstance();

    void Initialize();
    void BeginFrame();
    void BeginMainRender();
    void EndMainRender();
    void DeferredRender(const ViewProjection& viewProjection, const DirectionalLights& directionalLight, const PointLights& pointLights, const SpotLights& spotLights);
    void BeginShadowMapRender(DirectionalLights& directionalLights);
    void EndShadowMapRender(DirectionalLights& directionalLights);
    void BeginUIRender();
    void EndUIRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }
    Bloom& GetBloom() { return bloom_; }

    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[rtvType].GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_.GetFormat(); }

    void ClearMainDepthBuffer() { commandContext_.ClearDepth(mainDepthBuffer_); }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;


    DirectXCommon* graphics_ = nullptr;
    SwapChain swapChain_;
    CommandContext commandContext_;

    ColorBuffer colorBuffers_[kRenderTargetNum];
    DepthBuffer mainDepthBuffer_;

    ColorBuffer resultBuffer_;

    DeferredRenderer deferredRenderer_;
    EdgeRenderer edgeRenderer_;

    Bloom bloom_;
    PostEffect postEffect_;
};

