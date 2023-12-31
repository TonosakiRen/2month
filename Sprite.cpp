#include "Sprite.h"
#include "TextureManager.h"
#include <cassert>
#include "WinApp.h"
#include "ShaderManager.h"
#include "Renderer.h"

using namespace Microsoft::WRL;
CommandContext* Sprite::commandContext_ = nullptr;
std::unique_ptr<RootSignature> Sprite::rootSignature_;
std::unique_ptr<PipelineState> Sprite::pipelineState_;
Matrix4x4 Sprite::matProjection_;

void Sprite::StaticInitialize() {
	ComPtr<IDxcBlob> vsBlob;    
	ComPtr<IDxcBlob> psBlob;    
	ComPtr<ID3DBlob> errorBlob; 

	auto shaderManager = ShaderManager::GetInstance();

	vsBlob = shaderManager->Compile(L"SpriteVS.hlsl", ShaderManager::kVertex);
	assert(vsBlob != nullptr);

	psBlob = shaderManager->Compile(L"SpritePS.hlsl", ShaderManager::kPixel);
	assert(psBlob != nullptr);

	rootSignature_ = std::make_unique<RootSignature>();
	pipelineState_ = std::make_unique<PipelineState>();

	{

		CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
		descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER rootparams[2] = {};
		rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_STATIC_SAMPLER_DESC samplerDesc =
			CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0 レジスタ
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

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
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
		gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob->GetBufferPointer(), psBlob->GetBufferSize());

		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;


		D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
		blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blenddesc.BlendEnable = true;
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


		gpipeline.BlendState.RenderTarget[0] = blenddesc;


		gpipeline.DSVFormat = Renderer::GetInstance()->GetDSVFormat();


		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);


		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		gpipeline.NumRenderTargets = 1;
		gpipeline.RTVFormats[int(Renderer::kColor)] = Renderer::GetInstance()->GetRTVFormat(Renderer::kColor);
		gpipeline.SampleDesc.Count = 1;

		gpipeline.pRootSignature = *rootSignature_;

		// グラフィックスパイプラインの生成
		pipelineState_->Create(gpipeline);

	}

	matProjection_ = MakeOrthograohicmatrix(0.0f, 0.0f, (float)WinApp::kWindowWidth, (float)WinApp::kWindowHeight,  0.0f, 1.0f);
}

void Sprite::PreDraw(CommandContext* commandContext) {
	assert(Sprite::commandContext_ == nullptr);

	commandContext_ = commandContext;

	commandContext_->SetPipelineState(*pipelineState_);
	commandContext_->SetGraphicsRootSignature(*rootSignature_);
	commandContext_->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw() {
	Sprite::commandContext_ = nullptr;
}

void Sprite::Initialize(uint32_t textureHandle, Vector2 position, Vector4 color, Vector2 anchorpoint, bool isFlipX, bool isFlipY) {

	Vector2 size = { 100.0f, 100.0f };

	const D3D12_RESOURCE_DESC& resDesc = TextureManager::GetInstance()->GetResoureDesc(textureHandle);
	size = { (float)resDesc.Width, (float)resDesc.Height };

	position_ = position;
	size_ = size;
	anchorPoint_ = anchorpoint;
	matWorld_ = MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
	texSize_ = size;

	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	vertexBuffer_.Create(sizeof(VertexData) * 4);

	// 頂点バッファへのデータ転送
	TransferVertices();

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(VertexData) * 4;
	vbView_.StrideInBytes = sizeof(VertexData);

	constBuffer_.Create((sizeof(ConstBufferData) + 0xff) & ~0xff);
}

void Sprite::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);
}

void Sprite::Draw() {

	TransferVertices();

	matWorld_ = MakeIdentity4x4();
	matWorld_ *= MakeRotateZMatrix(rotation_);
	matWorld_ *= MakeTranslateMatrix({ position_.x, position_.y, 0.0f });

	ConstBufferData mapData;

	mapData.color = color_;
	mapData.mat = matWorld_ * matProjection_;

	constBuffer_.Copy(mapData);

	commandContext_->SetVertexBuffer(0, 1, &vbView_);

	commandContext_->SetConstantBuffer(0, constBuffer_.GetGPUVirtualAddress());

	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandContext_, 1, textureHandle_);
	
	commandContext_->DrawInstanced(4, 1, 0, 0);
}

void Sprite::TransferVertices() {

	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	if (isFlipX_) { 
		left = -left;
		right = -right;
	}

	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// 頂点データ
	VertexData vertices[4];

	vertices[LB].pos = { left, bottom, 0.0f };  
	vertices[LT].pos = { left, top, 0.0f };     
	vertices[RB].pos = { right, bottom, 0.0f }; 
	vertices[RT].pos = { right, top, 0.0f };    

	{
		float tex_left = texBase_.x / resourceDesc_.Width;
		float tex_right = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float tex_top = texBase_.y / resourceDesc_.Height;
		float tex_bottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices[LB].uv = { tex_left, tex_bottom };  
		vertices[LT].uv = { tex_left, tex_top };     
		vertices[RB].uv = { tex_right, tex_bottom }; 
		vertices[RT].uv = { tex_right, tex_top };    
	}

	// 頂点バッファへのデータ転送
	vertexBuffer_.Copy(vertices, sizeof(vertices));
}
