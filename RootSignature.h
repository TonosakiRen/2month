#pragma once

#include <d3d12.h>
#include <wrl/client.h>


class RootSignature {
public:
    void Create(const D3D12_ROOT_SIGNATURE_DESC& desc);

    operator ID3D12RootSignature* () const { return rootSignature_.Get(); }
    operator bool() const { return rootSignature_; }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};