#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"

class ShadowSpotLights;
class ShadowIsCollision
{
public:
	enum class RootParameter {
		kRwStructure,
		kColorTexture,

		ParameterNum
	};
	void Initialize(ColorBuffer* resultBuffer);
	void Dispatch(CommandContext& commandContext);
	void* GetData();
	void UnMap();
private:
	void CreatePipeline();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	ShadowSpotLights* shadowSpotLights_;
	ColorBuffer* resultBuffer_ = nullptr;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;
	void* data_;
	DescriptorHandle uavHandle_;
};

