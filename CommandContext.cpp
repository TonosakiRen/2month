#include "CommandContext.h"

#include "DirectXCommon.h"
#include "DescriptorHeap.h"

#include "Helper.h"

void CommandContext::Create() {

    auto device = DirectXCommon::GetInstance()->GetDevice();
    Helper::AssertIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.ReleaseAndGetAddressOf())));

    Helper::AssertIfFailed(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
        nullptr, IID_PPV_ARGS(commandList_.ReleaseAndGetAddressOf())));
}

void CommandContext::SetDescriptorHeap()
{
    auto graphics = DirectXCommon::GetInstance();
    ID3D12DescriptorHeap* ppHeaps[] = {
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
     };
    commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CommandContext::Close() {
    FlushResourceBarriers();
    Helper::AssertIfFailed(commandList_->Close());
}

void CommandContext::Reset() {
    
    Helper::AssertIfFailed(commandAllocator_->Reset());

    Helper::AssertIfFailed(commandList_->Reset(commandAllocator_.Get(), nullptr));

    auto graphics = DirectXCommon::GetInstance();
    ID3D12DescriptorHeap* ppHeaps[] = {
        (ID3D12DescriptorHeap*)graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
     };
    commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    rootSignature_ = nullptr;
    pipelineState_ = nullptr;
}
