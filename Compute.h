#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
#include "ColorBuffer.h"
class Compute
{
public:
	enum class RootParameter {
		kRwStructure,
		kColorTexture1,
		kColorTexture2,
		
		ParameterNum
	};
	static void StaticInitialize();
	void Initialize();
	void Dispatch(CommandContext& commandContext,ColorBuffer* colorBuffer1,ColorBuffer* colorBuffer2);
	void* GetData();
	void UnMap();
private:
	static void CreatePipeline();
private:
	static PipelineState pipelineState_;
	static RootSignature rootSignature_;
	GPUResource rwStructureBuffer_;
	GPUResource copyBuffer_;
	void* data_;
	const uint32_t kNum = 1024 * 1024;
	DescriptorHandle uavHandle_;
};

