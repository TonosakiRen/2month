#include "Model.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>
#pragma comment(lib, "d3dcompiler.lib")
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Renderer.h"

using namespace DirectX;
using namespace Microsoft::WRL;

ID3D12GraphicsCommandList* Model::commandList_ = nullptr;
std::unique_ptr<RootSignature> Model::rootSignature_;
std::unique_ptr<PipelineState> Model::pipelineState_;
void Model::StaticInitialize() {
    CreatePipeline();
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList, const ViewProjection& viewProjection, const DirectionalLights& directionalLight) {
    assert(Model::commandList_ == nullptr);

    commandList_ = commandList;

    commandList->SetPipelineState(*pipelineState_);
    commandList->SetGraphicsRootSignature(*rootSignature_);

    // CBVをセット（ビュープロジェクション行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
    // CBVをセット（ライト）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kDirectionalLights), directionalLight.GetGPUVirtualAddress());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
    commandList_ = nullptr;
}

void Model::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;    
    ComPtr<IDxcBlob> psBlob;    
    ComPtr<ID3DBlob> errorBlob; 

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"BasicVS.hlsl",ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"BasicPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);

    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
        descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[5] = {};
        rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[2].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[3].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);

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
          {
           "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
          {
           "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
           D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        };

        // グラフィックスパイプラインの流れを設定
        D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
        gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = true;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.RenderTarget[0] = blenddesc;

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = Renderer::kRenderTargetNum;
        gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
        gpipeline.RTVFormats[int(Renderer::kNormal)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kNormal);
        gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void Model::Draw(uint32_t modelHandle, const WorldTransform& worldTransform,const Material& material) {

    // CBVをセット（ワールド行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform),worldTransform.GetGPUVirtualAddress());

    // CBVをセット（マテリアル）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    ModelManager::GetInstance()->DrawInstanced(commandList_, modelHandle, static_cast<UINT>(RootParameter::kTexture));
}

void Model::Draw(uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material, uint32_t textureHandle) {

    // CBVをセット（ワールド行列）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    // CBVをセット（マテリアル）
    commandList_->SetGraphicsRootConstantBufferView(static_cast<UINT>(RootParameter::kMaterial), material.GetGPUVirtualAddress());

    ModelManager::GetInstance()->DrawInstanced(commandList_, modelHandle, static_cast<UINT>(RootParameter::kTexture));
}
