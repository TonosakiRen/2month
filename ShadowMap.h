#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"

#include "WorldTransform.h"

class DirectXCommon;
class LightNumBuffer;
class DirectionalLights;

class ShadowMap
{
public:
	enum class RootParameter {
		kWorldTransform,
		kDirectionalLight,

		parameterNum
	};

	static bool isDrawShadowMap;

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext, DirectionalLights& directionalLights);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static DirectionalLights* directionalLights_;
};

