#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"
#include "Mymath.h"

class ShadowSpotLights;
class ShadowIsCollision
{
public:

	static bool isShadowCollision;

	enum class RootParameter {
		kRwStructure,
		kIndex,
		kLuminance,
		kColorTexture,
		kIndexTexture,

		ParameterNum
	};
	void Initialize(ColorBuffer* resultBuffer , ColorBuffer* indexBuffer);
	void Dispatch(CommandContext& commandContext, const Vector2& index, const float luminance);
	void* GetData();
	void UnMap();
private:
	void CreatePipeline();
private:
	PipelineState pipelineState_;
	RootSignature rootSignature_;
	ShadowSpotLights* shadowSpotLights_;
	ColorBuffer* resultBuffer_ = nullptr;
	ColorBuffer* indexBuffer_ = nullptr;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;
	void* data_;
	DescriptorHandle uavHandle_;
};

