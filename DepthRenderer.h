#pragma once
#include <Windows.h>
#include <memory>

#include "PipelineState.h"
#include "RootSignature.h"
#include "CommandContext.h"

#include "WorldTransform.h"

class DirectXCommon;
class DepthBuffer;
class ViewProjection;

class DepthRenderer
{
public:
	enum class RootParameter {
		kWorldTransform,
		kViewProjection,

		parameterNum
	};

	static bool isDrawDepth;

	static void StaticInitialize();
	static void SetDepthBuffer(DepthBuffer* depthBuffer);
	static void Finalize();

	static void PreDraw(CommandContext* commandContext, const ViewProjection& viewProjection);
	static void PostDraw();

	static void Draw(uint32_t modelHandle, const WorldTransform& worldTransform);

private:
	static void CreatePipeline();
private:
	static CommandContext* commandContext_;
	static std::unique_ptr<RootSignature> rootSignature_;
	static std::unique_ptr<PipelineState> pipelineState_;
	static DepthBuffer* depthBuffer_;
};

