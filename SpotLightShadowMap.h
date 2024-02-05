#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"
#include "UploadBuffer.h"

#include "WorldTransform.h"

class DirectXCommon;
class LightNumBuffer;
class ShadowSpotLights;

class SpotLightShadowMap
{
public:
	enum class RootParameter {
		kWorldTransform,
		kShadowSpotLight,
		kCollisionData,

		parameterNum
	};

	struct VertexData {
		Vector4 pos;
		Vector2 uv;
	};

	static bool isDrawSpotLightShadowMap;

	static void StaticInitialize();
	static void Finalize();
	static void PreDraw(CommandContext* commandContext, ShadowSpotLights& shadowSpotLights);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

	static void Draw(int nonShadeLightIndex,uint32_t modelHandle, const WorldTransform& worldTransform);

	static void PlayerDraw(uint32_t modelHandle, const WorldTransform& worldTransform);
	static void EnemyDraw(const Vector2& enemyIndex, uint32_t modelHandle, const WorldTransform& worldTransform);
	static void EnemyDraw(const Vector2& enemyIndex, uint32_t modelHandle, const WorldTransform& worldTransform,uint32_t unShadingLightIndex);
private:
	static void CreatePipeline();
	static void CreateMesh();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static D3D12_VERTEX_BUFFER_VIEW vbView_;
	static D3D12_INDEX_BUFFER_VIEW ibView_;
	static ShadowSpotLights* shadowSpotLights_;
	static std::unique_ptr<UploadBuffer> playerCollisionData_;
	static std::unique_ptr<UploadBuffer> unCollisionData_;

	static std::vector<VertexData> vertices_;
	static std::vector<uint16_t> indices_;
	static std::unique_ptr<UploadBuffer> vertexBuffer_;
	static std::unique_ptr<UploadBuffer> indexBuffer_;
};