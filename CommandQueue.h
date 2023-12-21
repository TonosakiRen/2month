#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <chrono>

class CommandQueue {
public:
    ~CommandQueue();

    void Create();
    void Excute(ID3D12GraphicsCommandList* commandList);
    void Signal();
    void WaitForGPU();
    void UpdateFixFPS();

    operator ID3D12CommandQueue* () const { return commandQueue_.Get(); }

private:
    void Destroy();

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    std::chrono::steady_clock::time_point reference_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_;
};
