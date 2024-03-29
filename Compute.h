#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"

class ShadowSpotLights;
class ViewProjection;
class Compute
{
public:

	static void* data_;
	static void* hitPosData_;

	enum class RootParameter {
		kRwStructure,
		kHitPos,
		kColorTexture,
		kDepthTexture,
		kEnemyNum,
		kViewProjection,
		
		ParameterNum
	};
	void Initialize(ColorBuffer* indexBuffer);
	void Dispatch(CommandContext& commandContext, DepthBuffer& depthBuffer, const ViewProjection& viewProjection);
	static void* GetData();
	static void* GetPosData();
	void UnMap();
private:
	void CreatePipeline();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	ColorBuffer* indexBuffer_;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;

	GPUResource hitPosBuffer_;
	GPUResource hitPosCopyBuffer_;

	const uint32_t kNum = 1024;
	DescriptorHandle uavHandle_;
	DescriptorHandle uavHandle2_;
};

