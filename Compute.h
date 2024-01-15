#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"

class ShadowSpotLights;
class Compute
{
public:
	enum class RootParameter {
		kRwStructure,
		kColorTexture1,
		
		ParameterNum
	};
	static void StaticInitialize();
	void Initialize(ShadowSpotLights& shadowSpotLights);
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
	const uint32_t kNum = 1024 * 1024;
	DescriptorHandle uavHandle_;
};

