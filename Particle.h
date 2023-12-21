#pragma once
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include <Windows.h>
#include <d3d12.h>
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <wrl.h>
#include "Mymath.h"
#include "Material.h"
#include "DescriptorHandle.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"

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
	static void PreDraw(ID3D12GraphicsCommandList* commandList, const ViewProjection& viewProjection);
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
	static ID3D12GraphicsCommandList* commandList_;
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