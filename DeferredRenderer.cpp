#include "DeferredRenderer.h"
#include <Windows.h>
#include "DirectXCommon.h"
#include "ShaderManager.h"
#include "Helper.h"
#include <assert.h>
#include "Renderer.h"
#include "ViewProjection.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"

using namespace Microsoft::WRL;

RootSignature DeferredRenderer::rootSignature_;
PipelineState DeferredRenderer::pipelineState_;

void DeferredRenderer::Initialize(ColorBuffer* colorTexture, ColorBuffer* normalTexture, DepthBuffer* depthTexture)
{
	normalTexture_ = normalTexture;
	depthTexture_ = depthTexture;
	colorTexture_ = colorTexture;
	CreatePipeline();
	CreateMesh();
	lightNumBuffer_.Create(sizeof(LightNum));
	LightNum lightNum{DirectionalLights::lightNum,PointLights::lightNum,SpotLights::lightNum};
	lightNumBuffer_.Copy(lightNum);
}

void DeferredRenderer::Render(CommandContext& commandContext,ColorBuffer* originalBuffer, const ViewProjection& viewProjection, const DirectionalLights& directionalLight, const PointLights& pointLights, const SpotLights& spotLights)
{
	commandContext.TransitionResource(*originalBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext.SetRenderTarget(originalBuffer->GetRTV());
	commandContext.SetViewportAndScissorRect(0, 0, originalBuffer->GetWidth(), originalBuffer->GetHeight());

	commandContext.TransitionResource(*colorTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*normalTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext.TransitionResource(*depthTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetGraphicsRootSignature(rootSignature_);
	commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kColorTexture), colorTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kNormalTexture), normalTexture_->GetSRV());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kDepthTexture), depthTexture_->GetSRV());
	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kViewProjection), viewProjection.GetGPUVirtualAddress());
	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kDirectionalLights), directionalLight.lights_[0].constBuffer_.GetGPUVirtualAddress());
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kPointLights), pointLights.srvHandle_);
	commandContext.SetDescriptorTable(static_cast<UINT>(RootParameter::kSpotLights), spotLights.srvHandle_);

	commandContext.SetConstantBuffer(static_cast<UINT>(RootParameter::kLightNum),lightNumBuffer_.GetGPUVirtualAddress());
	

	commandContext.SetVertexBuffer(0, vbView_);
	commandContext.SetIndexBuffer(ibView_);

	commandContext.DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);


}

void DeferredRenderer::CreatePipeline()
{
	ComPtr<IDxcBlob> vsBlob;
	ComPtr<IDxcBlob> psBlob;

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"PostEffectVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"DeferredRenderingPS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	{

		CD3DX12_DESCRIPTOR_RANGE ranges[5]{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
		ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

		ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
		ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);

		CD3DX12_ROOT_PARAMETER rootParameters[(int)RootParameter::ParameterNum]{};
		rootParameters[(int)RootParameter::kColorTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kColorTexture]);
		rootParameters[(int)RootParameter::kNormalTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kNormalTexture]);
		rootParameters[(int)RootParameter::kDepthTexture].InitAsDescriptorTable(1, &ranges[(int)RootParameter::kDepthTexture]);
		rootParameters[(int)RootParameter::kViewProjection].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

		rootParameters[(int)RootParameter::kDirectionalLights].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[(int)RootParameter::kPointLights].InitAsDescriptorTable(1, &ranges[3]);
		rootParameters[(int)RootParameter::kSpotLights].InitAsDescriptorTable(1, &ranges[4]);

		rootParameters[(int)RootParameter::kLightNum].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
		
		
		// スタティックサンプラー
		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pStaticSamplers = &samplerDesc;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		rootSignature_.Create(rootSignatureDesc);

	}

	{

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			  {
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
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

		// 頂点レイアウトの設定
		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);

		// 図形の形状設定（三角形）
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;                            // 描画対象は1つ
		gpipeline.RTVFormats[0] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor); // 0～255指定のRGBA
		gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

		gpipeline.pRootSignature = rootSignature_;

		pipelineState_.Create(gpipeline);

	}

	
}

void DeferredRenderer::CreateMesh()
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

	vertexBuffer_.Create(sizeVB);

	vertexBuffer_.Copy(vertices_.data(), sizeVB);

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);


	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());

	indexBuffer_.Create(sizeIB);

	indexBuffer_.Copy(indices_.data(), sizeIB);

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuffer_.GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

}

