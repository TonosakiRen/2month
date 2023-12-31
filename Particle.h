#pragma once
#include <Windows.h>
#include <memory>
#include <vector>


#include "PipelineState.h"
#include "RootSignature.h"
#include "DescriptorHandle.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

class Particle
{
public:

	const uint32_t kParticleNum;

	enum class RootParameter {
		kWorldTransform,
		kViewProjection,
		kTexture,
		kMaterial,
		parameterNum
	};

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct InstancingBufferData {
		Matrix4x4 matWorld;
	};

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();
	static Particle* Create(uint32_t particleNum);

	Particle(uint32_t particleNum);
	void Initialize();
	void Draw(const std::vector<InstancingBufferData>& bufferData, const Vector4& color, const uint32_t textureHadle);
	void CreateMesh();

public:
	Material material_;

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static Matrix4x4 billBordMatrix;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	DescriptorHandle srvHandle_;
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	UploadBuffer vertexBuffer_;
	UploadBuffer indexBuffer_;
	UploadBuffer instancingBuffer_;
};