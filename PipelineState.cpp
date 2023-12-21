#include "PipelineState.h"

#include "DirectXCommon.h"
#include <assert.h>
void PipelineState::Create( const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
    HRESULT hr = S_FALSE;
    hr = (DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    assert(SUCCEEDED(hr));
}

void PipelineState::Create(const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc)
{
    HRESULT hr = S_FALSE;
    hr = (DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
    assert(SUCCEEDED(hr));
}
