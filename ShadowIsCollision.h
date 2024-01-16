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
	static void StaticInitialize();
	void Initialize(ColorBuffer* resultBuffer);
	void Dispatch(CommandContext& commandContext);
	void* GetData();
	void UnMap();
private:
	static void CreatePipeline();
private:
	static PipelineState pipelineState_;
	static RootSignature rootSignature_;
	ShadowSpotLights* shadowSpotLights_;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;
	void* data_;
	const uint32_t kNum = 1024;
	DescriptorHandle uavHandle_;
};

