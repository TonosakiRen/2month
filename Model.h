#pragma once
#include <Windows.h>
#include <memory>
#include <d3d12.h>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DirectionalLights.h"
#include "Material.h"
#include "Mesh.h"

class DirectXCommon;

class Model
{
public:
	enum class RootParameter {
		kWorldTransform, 
		kViewProjection, 
		kDirectionalLights,
		kTexture,
		kShadowMap,
		kMaterial,

		parameterNum
	};

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection, const DirectionalLights& directionalLights);
	static void PostDraw();

	static void Draw(uint32_t modelHandle,const WorldTransform& worldTransform, const Material& material);
	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform, const Material& material, uint32_t textureHadle);

private: 
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static std::unique_ptr<PipelineState> wireFramePipelineState_;
};

