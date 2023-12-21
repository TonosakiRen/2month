#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "Mymath.h"
#include "UploadBuffer.h"

class ViewProjection;
class DirectionalLights;


class EdgeRenderer
{
public:
	enum class RootParameter {
		kColorTexture,
		kNormalTexture,
		kDepthTexture,
		kEdgeColor,

		ParameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};
	void Initialize(ColorBuffer* originalTexture,ColorBuffer* normalTexture, DepthBuffer* depthTexture);
	void Render(CommandContext& commandContext, ColorBuffer* originalTexture);

private:
	void CreatePipeline();
	void CreateMesh();
private:
	PipelineState edgePipelineState_;
	PipelineState multiplyPipeline_;
	RootSignature edgeRootSignature_;
	RootSignature multiplyRootSignature_;

	ColorBuffer* normalTexture_;
	DepthBuffer* depthTexture_;

	ColorBuffer edgeTexture_;

	DescriptorHandle uavHandle_;


	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;

	Vector3 edgeColor_ = { 0.0f,0.0f,0.0f };


};

