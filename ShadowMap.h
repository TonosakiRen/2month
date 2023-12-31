#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "UploadBuffer.h"
#include "CommandContext.h"

#include "WorldTransform.h"
#include "DirectionalLights.h"

class DirectXCommon;

class ShadowMap
{
public:
	enum class RootParameter {
		kWorldTransform,
		kDirectionalLights,

		parameterNum
	};

	static bool isDrawShadowMap;

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext, const DirectionalLights& directionalLight);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
};

