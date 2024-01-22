#include "Renderer.h"

#include "ShaderManager.h"
#include "ImGuiManager.h"
#include "Helper.h"
#include "WinApp.h"

#include "ViewProjection.h"
#include "DirectionalLights.h"
#include "ShadowSpotLights.h"

#include "GlobalVariables.h"

Renderer* Renderer::GetInstance() {
    static Renderer instance;
    return &instance;
}

void Renderer::Initialize() {
    // 描画デバイスを初期化
    graphics_ = DirectXCommon::GetInstance();
    graphics_->Initialize();

    // コマンドリストを初期化   
    commandContext_.Create();
    commandContext_.SetDescriptorHeap();

    // シェーダー関連を初期化
    ShaderManager::GetInstance()->Initialize();

    // スワップチェーンを初期化
    auto window = WinApp::GetInstance();
    swapChain_ = std::make_unique<SwapChain>();
    swapChain_->Create(window->GetHwnd());

    // メインとなるバッファを初期化
    auto& swapChainBuffer = swapChain_->GetColorBuffer();
    float clearColor[4] = { 0.0f,0.0f,0.3f,0.0f };
    colorBuffers_[kColor] = std::make_unique<ColorBuffer>();
    colorBuffers_[kColor]->SetClearColor(clearColor);
    colorBuffers_[kColor]->Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    resultBuffer_ = std::make_unique<ColorBuffer>();
    resultBuffer_->Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
   
    mainDepthBuffer_ = std::make_unique<DepthBuffer>();
    mainDepthBuffer_->Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    float clearNormal[4] = { 0.0f,0.0f,0.0f,1.0f };
    colorBuffers_[kNormal] = std::make_unique<ColorBuffer>();
    colorBuffers_[kNormal]->SetClearColor(clearNormal);
    colorBuffers_[kNormal]->Create(swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);


    // ブルームを初期化
    bloom_ = std::make_unique<Bloom>();
    bloom_->Initialize(colorBuffers_[kColor].get());

    postEffect_ = std::make_unique<PostEffect>();
    postEffect_->Initialize();

    shadowTexture_ = std::make_unique<ColorBuffer>();
    shadowTexture_->Create(colorBuffers_[kColor]->GetWidth(), colorBuffers_[kColor]->GetHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT);

    deferredRenderer_ = std::make_unique<DeferredRenderer>();
    deferredRenderer_->Initialize(colorBuffers_[kColor].get(), colorBuffers_[kNormal].get(), shadowTexture_.get(), mainDepthBuffer_.get());

    edgeRenderer_ = std::make_unique<EdgeRenderer>();
    edgeRenderer_->Initialize(colorBuffers_[kColor].get(), colorBuffers_[kNormal].get(), mainDepthBuffer_.get());

    shadowEdgeRenderer_ = std::make_unique<ShadowEdgeRenderer>();
    shadowEdgeRenderer_->Initialize(colorBuffers_[kColor].get(), shadowTexture_.get());

    // ImGuiを初期化
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window);

    lightNumBuffer_ = std::make_unique<LightNumBuffer>();
    lightNumBuffer_->Create();

    shadowIsCollision_ = std::make_unique<ShadowIsCollision>();
    shadowIsCollision_->Initialize(resultBuffer_.get(), shadowTexture_.get());

    globalVariables_ = GlobalVariables::GetInstance();
    SetGlobalVariable();

}

void Renderer::SetGlobalVariable()
{

    std::string name = "ShadingCollisionAdjustment";
    globalVariables_->CreateGroup(name);
    globalVariables_->AddItem(name,"shadingValue", shading_);
    globalVariables_->AddItem(name, "collisionLuminance", collisionLuminance_);
    globalVariables_->LoadFile(name);
    ApplyGlobalVariable();
}

void Renderer::ApplyGlobalVariable()
{
    std::string name = "ShadingCollisionAdjustment";
    shading_ = globalVariables_->GetFloatValue(name, "shadingValue");
    collisionLuminance_ = globalVariables_->GetFloatValue(name, "collisionLuminance");
}

void Renderer::BeginFrame()
{
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Begin();
    ApplyGlobalVariable();
}

void Renderer::BeginMainRender() {

    // メインカラーバッファをレンダ―ターゲットに
    commandContext_.TransitionResource(*colorBuffers_[kColor], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*colorBuffers_[kNormal], D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(*mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[] = { colorBuffers_[kColor]->GetRTV(),colorBuffers_[kNormal]->GetRTV() };
    commandContext_.SetRenderTargets(kRenderTargetNum, rtvHandle, mainDepthBuffer_->GetDSV());
   
    for (int i = 0; i < kRenderTargetNum;i++) {
        commandContext_.ClearColor(*colorBuffers_[i]);
    }
    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.ClearColor(*resultBuffer_);

    commandContext_.ClearDepth(*mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, colorBuffers_[kColor]->GetWidth(), colorBuffers_[kColor]->GetHeight());
}

void Renderer::DeferredRender(const ViewProjection& viewProjection, DirectionalLights& directionalLight, const PointLights& pointLights, const SpotLights& spotLights, ShadowSpotLights& shadowSpotLights)
{
    deferredRenderer_->Render(commandContext_, resultBuffer_.get(), viewProjection, directionalLight, pointLights, spotLights, shadowSpotLights, *lightNumBuffer_,shading_);
}

void Renderer::BeginShadowMapRender(DirectionalLights& directionalLights)
{
    commandContext_.SetViewportAndScissorRect(0, 0, DirectionalLights::shadowWidth, DirectionalLights::shadowHeight);
}

void Renderer::EndShadowMapRender(DirectionalLights& directionalLights)
{
    for (int i = 0; i < DirectionalLights::lightNum;i++) {
        commandContext_.TransitionResource(directionalLights.lights_[i].shadowMap_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void Renderer::BeginSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights)
{
    commandContext_.SetViewportAndScissorRect(0, 0, ShadowSpotLights::shadowWidth, ShadowSpotLights::shadowHeight);
}

void Renderer::EndSpotLightShadowMapRender(ShadowSpotLights& shadowSpotLights)
{
    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        commandContext_.TransitionResource(shadowSpotLights.lights_[i].shadowMap_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

void Renderer::EndMainRender() {
 
    bool isCollision = ShadowIsCollision::isShadowCollision;
    ImGui::Text("%d", int(isCollision));

    shadowIsCollision_->Dispatch(commandContext_,{0.0f,0.0f}, collisionLuminance_);

    edgeRenderer_->Render(commandContext_, resultBuffer_.get());
    shadowEdgeRenderer_->Render(commandContext_, resultBuffer_.get());
    //bloom_.Render(commandContext_, &resultBuffer_);
}

void Renderer::BeginUIRender()
{

    //コピーできないよん
    //commandContext_.CopyBuffer(swapChain_.GetColorBuffer(), *resultBuffer_);

    //コピーしようとするとき消して
    commandContext_.TransitionResource(*resultBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandContext_.SetViewportAndScissorRect(0, 0, resultBuffer_->GetWidth(), resultBuffer_->GetHeight());
    commandContext_.TransitionResource(swapChain_->GetColorBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(swapChain_->GetRTV());
    commandContext_.ClearColor(swapChain_->GetColorBuffer());

    postEffect_->Draw(resultBuffer_->GetSRV(),commandContext_);
}

void Renderer::EndUIRender()
{

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Draw(commandContext_);

    commandContext_.TransitionResource(swapChain_->GetColorBuffer(), D3D12_RESOURCE_STATE_PRESENT);
    commandContext_.Close();
    CommandQueue& commandQueue = graphics_->GetCommandQueue();
    
    commandQueue.Excute(commandContext_);
    swapChain_->Present();

    commandQueue.Signal();
    commandQueue.WaitForGPU();
    commandQueue.UpdateFixFPS();

    commandContext_.Reset();
}

void Renderer::Shutdown() {
   
    for (int i = 0; i < kRenderTargetNum; i++) {
        colorBuffers_[i].reset();
    }

    shadowIsCollision_.reset();

    mainDepthBuffer_.reset();

    resultBuffer_.reset();

    deferredRenderer_.reset();
    edgeRenderer_.reset();
    shadowEdgeRenderer_.reset();
    shadowTexture_.reset();


    bloom_.reset();
    postEffect_.reset();

    lightNumBuffer_.reset();

    swapChain_.reset();
    commandContext_.ShutDown();

    graphics_->Shutdown();
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Finalize();
}


