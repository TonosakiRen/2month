#pragma once
#include "PipelineState.h"
#include "RootSignature.h"
#include "GPUResource.h"
#include "CommandContext.h"
#include "DescriptorHandle.h"
class Compute
{
public:
	static void StaticInitialize();
	void Initialize();
	void Dispatch(CommandContext& commandContext);
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
	const uint32_t kNum = 156;
	DescriptorHandle uavHandle_;
};

