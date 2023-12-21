#include "RootSignature.h"

#include "Helper.h"
#include <assert.h>

#include "DirectXCommon.h"

void RootSignature::Create(const D3D12_ROOT_SIGNATURE_DESC& desc) {

    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    if (FAILED(D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        blob.GetAddressOf(),
        errorBlob.GetAddressOf()))) {
        MessageBoxA(nullptr, static_cast<char*>(errorBlob->GetBufferPointer()), "Failed create RootSignature!!", S_OK);
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    Helper::AssertIfFailed(DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(
        0,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature_.ReleaseAndGetAddressOf())));
}
