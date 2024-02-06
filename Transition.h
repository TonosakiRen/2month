#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include "Mymath.h"

#include "CommandContext.h"

class Transition
{
public:
	enum class RootParameter {
		kT,
		kResult,
		kTexture,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	static bool isNextScene_;
	static bool isTransition_;

	void Initialize(ColorBuffer& resultBuffer);

	void Draw(ColorBuffer& resultBuffer, const DescriptorHandle& textureBuffer, CommandContext& commandContext);

	static void StartTransition() {
		isTransition_ = true;
	};


private:
	void CreateMesh();
	void CreatePipeline();
private:
	RootSignature rootSignature_;
	PipelineState pipelineState_;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;
	ColorBuffer saveResultBuffer_;

	float t_ = 0.0f;
	float speed_ = 0.01f;
};