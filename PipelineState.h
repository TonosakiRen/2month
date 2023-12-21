#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <string>
class PipelineState
{
public:
	void Create(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
	void Create(const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc);

	operator ID3D12PipelineState* () const { return pipelineState_.Get(); }
	operator bool() const { return pipelineState_; }
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};

