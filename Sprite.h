#pragma once

#include <Windows.h>
#include <string>
#include <wrl.h>
#include <memory>
#include "Mymath.h"

#include "CommandContext.h"
#include "PipelineState.h"
#include "GPUResource.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"

class Sprite {
public:
	struct VertexData {
		Vector3 pos;
		Vector2 uv;
	};

	struct ConstBufferData {
		Vector4 color; 
		Matrix4x4 mat; 
	};

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext);
	static void PostDraw();
	
	void Initialize(uint32_t textureHandle, Vector2 position, Vector4 color = { 1, 1, 1, 1 }, Vector2 anchorpoint = { 0.5f, 0.5f }, bool isFlipX = false, bool isFlipY = false);

	void SetTextureHandle(uint32_t textureHandle);
	uint32_t GetTextureHandle() { return textureHandle_; }

	void Draw();
public:

	float rotation_ = 0.0f;
	Vector2 position_{};
	Vector2 size_ = { 100.0f, 100.0f };
	Vector2 anchorPoint_ = { 0.0f, 0.0f };
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool isFlipX_ = false;
	bool isFlipY_ = false;
	Vector2 texBase_ = { 0, 0 };
	Vector2 texSize_ = { 100.0f, 100.0f };

private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static Matrix4x4 matProjection_;

private: 
	UploadBuffer vertexBuffer_;
	UploadBuffer constBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	UINT textureHandle_ = 0;
	Matrix4x4 matWorld_{};
	D3D12_RESOURCE_DESC resourceDesc_;

private:
	void TransferVertices();
};
