#include "ShadowMap.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "LightNumBuffer.h"
#include "DirectionalLights.h"

using namespace Microsoft::WRL;

bool ShadowMap::isDrawShadowMap = false;
CommandContext* ShadowMap::commandContext_ = nullptr;
std::unique_ptr<RootSignature> ShadowMap::rootSignature_;
std::unique_ptr<PipelineState> ShadowMap::pipelineState_;
DirectionalLights* ShadowMap::directionalLights_;
void ShadowMap::StaticInitialize() {
    CreatePipeline();
}

void ShadowMap::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
}

void ShadowMap::PreDraw(CommandContext* commandContext,DirectionalLights& directionalLight) {
    assert(ShadowMap::commandContext_ == nullptr);

    commandContext_ = commandContext;
    directionalLights_ = &directionalLight;
    isDrawShadowMap = true;

    commandContext_->SetPipelineState(*pipelineState_);
    commandContext_->SetGraphicsRootSignature(*rootSignature_);

    for (int i = 0; i < DirectionalLights::lightNum; i++) {
        commandContext_->TransitionResource(directionalLights_->lights_[i].shadowMap_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandContext_->ClearDepth(directionalLights_->lights_[i].shadowMap_);
    }

    commandContext_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShadowMap::PostDraw() {
    commandContext_ = nullptr;
    isDrawShadowMap = false;
}

void ShadowMap::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"ShadowMapVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

         // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)RootParameter::kDirectionalLight].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

        // スタティックサンプラー
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
            CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        // ルートシグネチャの設定
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.pParameters = rootparams;
        rootSignatureDesc.NumParameters = _countof(rootparams);
        rootSignatureDesc.pStaticSamplers = &samplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_->Create(rootSignatureDesc);

    }

    {

        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
          {
           "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.SampleDesc.Count = 1;

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void ShadowMap::Draw(uint32_t modelHandle, const WorldTransform& worldTransform) {

    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    for (int i = 0; i < DirectionalLights::lightNum; i++) {
        commandContext_->SetDepthStencil(directionalLights_->lights_[i].shadowMap_.GetDSV());
        commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kDirectionalLight), directionalLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
        ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
    }
}

