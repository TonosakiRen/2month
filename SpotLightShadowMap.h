#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"

#include "WorldTransform.h"
#include "ShadowSpotLights.h"

class DirectXCommon;

class SpotLightShadowMap
{
public:

	struct SpotLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		WorldTransform worldTransform;
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		bool isActive = false;
		DepthBuffer shadowMap_;
		UploadBuffer constBuffer_;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		float isActive = false;
	};

	enum class RootParameter {
		kWorldTransform,
		kSpotLights,

		parameterNum
	};

	static bool isDrawShadowMap;

	static void StaticInitialize();
	static void PreDraw(CommandContext* commandContext, const ShadowSpotLights& shadowSpotLights);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
};

