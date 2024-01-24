#pragma once

#include "DirectXCommon.h"
#include "SwapChain.h"
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Bloom.h"
#include "PostEffect.h"
#include "Compute.h"

#include "DeferredRenderer.h"
#include "EdgeRenderer.h"
#include "LightNumBuffer.h"
#include "ShadowEdgeRenderer.h"
#include "ShadowIsCollision.h"
#include "DepthRenderer.h"

class viewProjection;
class DirectionalLights;
class ShadowSpotLights;
class GlobalVariables;

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
    void SetGlobalVariable();
    void ApplyGlobalVariable();
    void BeginFrame();
    void BeginMainRender();
    void EndMainRender();
    void DeferredRender(const ViewProjection& viewProjection, DirectionalLights& directionalLight, const PointLights& pointLights, const SpotLights& spotLights, ShadowSpotLights& shadowSpotLights);
    void BeginShadowMapRender(DirectionalLights& directionalLights);
    void EndShadowMapRender(DirectionalLights& directionalLights);
    void BeginSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights);
    void EndSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights);
    void BeginUIRender();
    void EndUIRender();
    void Shutdown();

    SwapChain& GetSwapChain() { return *swapChain_; }
    CommandContext& GetCommandContext() { return commandContext_; }
    Bloom& GetBloom() { return *bloom_; }

    DXGI_FORMAT GetRTVFormat(RenderTargetType rtvType) { return colorBuffers_[rtvType]->GetFormat(); }
    DXGI_FORMAT GetDSVFormat() { return mainDepthBuffer_->GetFormat(); }

    void ClearMainDepthBuffer() { commandContext_.ClearDepth(*mainDepthBuffer_); }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;


    DirectXCommon* graphics_ = nullptr;
    GlobalVariables* globalVariables_ = nullptr;
    std::unique_ptr<SwapChain> swapChain_;
    CommandContext commandContext_;

    std::unique_ptr<ColorBuffer> colorBuffers_[kRenderTargetNum];
    std::unique_ptr<DepthBuffer> mainDepthBuffer_;
    std::unique_ptr<DepthBuffer> nonCharacterDepthBuffer_;

    std::unique_ptr<ColorBuffer> resultBuffer_;

    std::unique_ptr<DeferredRenderer> deferredRenderer_;
    std::unique_ptr<EdgeRenderer> edgeRenderer_;
    std::unique_ptr<ShadowEdgeRenderer> shadowEdgeRenderer_;
    std::unique_ptr<ColorBuffer> shadowTexture_;
    
    std::unique_ptr<ShadowIsCollision> shadowIsCollision_;

    std::unique_ptr<Bloom> bloom_;
    std::unique_ptr<PostEffect> postEffect_;

    std::unique_ptr<LightNumBuffer> lightNumBuffer_;

    std::unique_ptr<Compute> compute_;

    float shading_ = 0.3f;
    float collisionLuminance_ = 0.8f;
};

