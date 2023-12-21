#include "CommandQueue.h"

#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include "Helper.h"
#include <assert.h>
#include "DirectXCommon.h"

CommandQueue::~CommandQueue() {
    Destroy();
}

void CommandQueue::Create() {

    auto device = DirectXCommon::GetInstance()->GetDevice();

    D3D12_COMMAND_QUEUE_DESC desc{};
    Helper::AssertIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())));

    Helper::AssertIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())));

    if (fenceEvent_) {
        fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(fenceEvent_);
    }

    fenceValue_ = 0;
    //現在時間を記録する
    reference_ = std::chrono::steady_clock::now();
}

void CommandQueue::Excute(ID3D12GraphicsCommandList* commandList) {
    ID3D12CommandList* ppCmdList[] = {commandList};
    commandQueue_->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);
}

void CommandQueue::Signal() {
    Helper::AssertIfFailed(commandQueue_->Signal(fence_.Get(), ++fenceValue_));
}

void CommandQueue::WaitForGPU() {

    if (fence_->GetCompletedValue() < fenceValue_) {
        Helper::AssertIfFailed(fence_->SetEventOnCompletion(fenceValue_, fenceEvent_));
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}

void CommandQueue::UpdateFixFPS()
{
    timeBeginPeriod(1);
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::microseconds elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    static const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 62.0f));
    static const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    std::chrono::microseconds check = kMinCheckTime - elapsed;
    if (check > std::chrono::microseconds(0)) {
        std::chrono::microseconds waitTime = kMinTime - elapsed;

        std::chrono::steady_clock::time_point waitStart = std::chrono::steady_clock::now();
        do {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        } while (std::chrono::steady_clock::now() - waitStart < waitTime);
    }

    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - reference_);
    reference_ = std::chrono::steady_clock::now();
    timeEndPeriod(1);
}

void CommandQueue::Destroy() {
    WaitForGPU();
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
}
