#include "SpotLightShadowMap.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "LightNumBuffer.h"
#include "ShadowSpotLights.h"
#include "Player.h"

using namespace Microsoft::WRL;

bool SpotLightShadowMap::isDrawSpotLightShadowMap = false;
CommandContext* SpotLightShadowMap::commandContext_ = nullptr;
std::unique_ptr<RootSignature> SpotLightShadowMap::rootSignature_;
std::unique_ptr<PipelineState> SpotLightShadowMap::pipelineState_;
D3D12_VERTEX_BUFFER_VIEW SpotLightShadowMap::vbView_;
D3D12_INDEX_BUFFER_VIEW SpotLightShadowMap::ibView_;
ShadowSpotLights* SpotLightShadowMap::shadowSpotLights_;
std::unique_ptr<UploadBuffer> SpotLightShadowMap::unCollisionData_;
std::unique_ptr<UploadBuffer> SpotLightShadowMap::playerCollisionData_;

std::vector<SpotLightShadowMap::VertexData> SpotLightShadowMap::vertices_;
std::vector<uint16_t> SpotLightShadowMap::indices_;
std::unique_ptr<UploadBuffer> SpotLightShadowMap::vertexBuffer_;
std::unique_ptr<UploadBuffer> SpotLightShadowMap::indexBuffer_;

void SpotLightShadowMap::StaticInitialize() {
    CreatePipeline();
    CreateMesh();
    Vector2 playerData = { 1.0f,0.0f };
    Vector2 nonData = { 0.0f,0.0f };
    playerCollisionData_ = std::make_unique<UploadBuffer>();
    playerCollisionData_->Create(sizeof(Vector2));
    playerCollisionData_->Copy(playerData);
    unCollisionData_ = std::make_unique<UploadBuffer>();
    unCollisionData_->Create(sizeof(Vector2));
    unCollisionData_->Copy(nonData);
}

void SpotLightShadowMap::Finalize()
{
    rootSignature_.reset();
    pipelineState_.reset();
    playerCollisionData_.reset();
    unCollisionData_.reset();
    vertexBuffer_.reset();
    indexBuffer_.reset();
}

void SpotLightShadowMap::PreDraw(CommandContext* commandContext, ShadowSpotLights& shadowSpotLights) {
    assert(SpotLightShadowMap::commandContext_ == nullptr);

    commandContext_ = commandContext;
    shadowSpotLights_ = &shadowSpotLights;
    isDrawSpotLightShadowMap = true;

    commandContext_->SetPipelineState(*pipelineState_);
    commandContext_->SetGraphicsRootSignature(*rootSignature_);

    commandContext->SetVertexBuffer(0, vbView_);
    commandContext->SetIndexBuffer(ibView_);

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        commandContext_->TransitionResource(shadowSpotLights_->lights_[i].shadowMap_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandContext_->TransitionResource(shadowSpotLights_->lights_[i].collisionData, D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandContext_->ClearDepth(shadowSpotLights_->lights_[i].shadowMap_);
        commandContext_->ClearColor(shadowSpotLights_->lights_[i].collisionData);
    }

    commandContext_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpotLightShadowMap::PostDraw() {
    commandContext_ = nullptr;
    isDrawSpotLightShadowMap = false;
}

void SpotLightShadowMap::CreatePipeline() {
    HRESULT result = S_FALSE;
    ComPtr<IDxcBlob> vsBlob;
    ComPtr<IDxcBlob> psBlob;

    auto shaderManager = ShaderManager::GetInstance();

    vsBlob = shaderManager->Compile(L"SpotLightShadowMapVS.hlsl", ShaderManager::kVertex);
    assert(vsBlob != nullptr);

    psBlob = shaderManager->Compile(L"SpotLightShadowMapPS.hlsl", ShaderManager::kPixel);
    assert(psBlob != nullptr);


    rootSignature_ = std::make_unique<RootSignature>();
    pipelineState_ = std::make_unique<PipelineState>();

    {

        // ルートパラメータ
        CD3DX12_ROOT_PARAMETER rootparams[int(RootParameter::parameterNum)] = {};
        rootparams[int(RootParameter::kWorldTransform)].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)RootParameter::kShadowSpotLight].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootparams[(int)RootParameter::kCollisionData].InitAsConstants(2, 2);

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
        gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

        // サンプルマスク
        gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
        // ラスタライザステート
        gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

        // レンダーターゲットのブレンド設定
        D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
        blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        blenddesc.BlendEnable = false;
        blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

        blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
        blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

        // ブレンドステートの設定
        gpipeline.BlendState.RenderTarget[0] = blenddesc;

        //  デプスステンシルステート
        gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        // 深度バッファのフォーマット
        gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();

        // 頂点レイアウトの設定
        gpipeline.InputLayout.pInputElementDescs = inputLayout;
        gpipeline.InputLayout.NumElements = _countof(inputLayout);

        // 図形の形状設定（三角形）
        gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
        gpipeline.RTVFormats[0] = DXGI_FORMAT_R32G32_FLOAT;
        gpipeline.SampleDesc.Count = 1;

        gpipeline.pRootSignature = *rootSignature_;

        // グラフィックスパイプラインの生成
        pipelineState_->Create(gpipeline);
    }
}

void SpotLightShadowMap::CreateMesh()
{
    vertices_.resize(4);

    //左下
    vertices_[0].pos = { -1.0f,-1.0f,0.0f, 1.0f };
    vertices_[0].uv = { 0.0f,1.0f };
    //左上
    vertices_[1].pos = { -1.0f,1.0f,0.0f, 1.0f };
    vertices_[1].uv = { 0.0f,0.0f };
    //右上
    vertices_[2].pos = { 1.0f,1.0f,0.0f, 1.0f };
    vertices_[2].uv = { 1.0f,0.0f };
    //右下
    vertices_[3].pos = { 1.0f,-1.0f,0.0f, 1.0f };
    vertices_[3].uv = { 1.0f,1.0f };

    // 頂点インデックスの設定
    indices_ = { 0,  1,  2, 0, 2, 3 };

    // 頂点データのサイズ
    UINT sizeVB = static_cast<UINT>(sizeof(VertexData) * vertices_.size());

    vertexBuffer_ = std::make_unique<UploadBuffer>();

    vertexBuffer_->Create(sizeVB);

    vertexBuffer_->Copy(vertices_.data(), sizeVB);

    // 頂点バッファビューの作成
    vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeVB;
    vbView_.StrideInBytes = sizeof(vertices_[0]);


    // インデックスデータのサイズ
    UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

    indexBuffer_ = std::make_unique<UploadBuffer>();

    indexBuffer_->Create(sizeIB);

    indexBuffer_->Copy(indices_.data(), sizeIB);

    // インデックスバッファビューの作成
    ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
    ibView_.Format = DXGI_FORMAT_R16_UINT;
    ibView_.SizeInBytes = sizeIB;

}


void SpotLightShadowMap::Draw(uint32_t modelHandle, const WorldTransform& worldTransform) {

    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kCollisionData), 0.0f, 0.0f);

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (shadowSpotLights_->lights_[i].isActive == true) {
            float distance = Length(MakeTranslation(shadowSpotLights_->lights_[i].worldTransform.matWorld_) - MakeTranslation(worldTransform.matWorld_));
            if (distance <= shadowSpotLights_->lights_[i].distance + 15.0f) {
                commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
                commandContext_->SetRenderTarget(shadowSpotLights_->lights_[i].collisionData.GetRTV(), shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
                ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
            }
        }
    }
}

void SpotLightShadowMap::Draw(int nonShadeLightIndex, uint32_t modelHandle, const WorldTransform& worldTransform) {

    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kCollisionData), 0.0f, 0.0f);

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (i != nonShadeLightIndex) {
            if (shadowSpotLights_->lights_[i].isActive == true) {
                float distance = Length(MakeTranslation(shadowSpotLights_->lights_[i].worldTransform.matWorld_) - MakeTranslation(worldTransform.matWorld_));
                if (distance <= shadowSpotLights_->lights_[i].distance + 15.0f) {
                    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
                    commandContext_->SetRenderTarget(shadowSpotLights_->lights_[i].collisionData.GetRTV(), shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
                    ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
                }
            }
        }
    }
}

void SpotLightShadowMap::PlayerDraw(uint32_t modelHandle, const WorldTransform& worldTransform)
{
    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kCollisionData), 1.0f, 0.0f);


    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (shadowSpotLights_->lights_[i].isActive == true) {
            float distance = Length(MakeTranslation(shadowSpotLights_->lights_[i].worldTransform.matWorld_) - MakeTranslation(worldTransform.matWorld_));
            if (distance <= shadowSpotLights_->lights_[i].distance + 15.0f) {
                commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
                commandContext_->SetRenderTarget(shadowSpotLights_->lights_[i].collisionData.GetRTV(), shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
                ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
            }
        }
    }
}

void SpotLightShadowMap::EnemyDraw(const Vector2& enemyIndex, uint32_t modelHandle, const WorldTransform& worldTransform)
{
    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kCollisionData), enemyIndex.x, enemyIndex.y);


    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (shadowSpotLights_->lights_[i].isActive == true) {
            float distance = Length(MakeTranslation(shadowSpotLights_->lights_[i].worldTransform.matWorld_) - MakeTranslation(worldTransform.matWorld_));
            if (distance <= shadowSpotLights_->lights_[i].distance + 15.0f) {
                commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
                commandContext_->SetRenderTarget(shadowSpotLights_->lights_[i].collisionData.GetRTV(), shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
                ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
            }
        }
    }
}

void SpotLightShadowMap::EnemyDraw(const Vector2& enemyIndex, uint32_t modelHandle, const WorldTransform& worldTransform, uint32_t unShadingLightIndex)
{
    // CBVをセット（ワールド行列）
    commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kWorldTransform), worldTransform.GetGPUVirtualAddress());

    commandContext_->SetConstants(static_cast<UINT>(RootParameter::kCollisionData), enemyIndex.x, enemyIndex.y);

    for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
        if (i != unShadingLightIndex) {
            for (int i = 0; i < ShadowSpotLights::lightNum; i++) {
                if (shadowSpotLights_->lights_[i].isActive == true) {
                    float distance = Length(MakeTranslation(shadowSpotLights_->lights_[i].worldTransform.matWorld_) - MakeTranslation(worldTransform.matWorld_));
                    if (distance <= shadowSpotLights_->lights_[i].distance + 10.0f) {
                        commandContext_->SetConstantBuffer(static_cast<UINT>(RootParameter::kShadowSpotLight), shadowSpotLights_->lights_[i].constBuffer_.GetGPUVirtualAddress());
                        commandContext_->SetRenderTarget(shadowSpotLights_->lights_[i].collisionData.GetRTV(), shadowSpotLights_->lights_[i].shadowMap_.GetDSV());
                        ModelManager::GetInstance()->DrawInstanced(commandContext_, modelHandle);
                    }
                }
            }
        }
    }
}

