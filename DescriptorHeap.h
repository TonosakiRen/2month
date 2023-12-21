#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "DescriptorHandle.h"
#include "externals/DirectXTex/d3dx12.h"

class DescriptorHeap {
public:
    DescriptorHeap() = default;

    void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
    // Reset関数で解放できる
    // 任意に解放はできない
    DescriptorHandle Allocate();
    // フリーディスクリプタへの位置を変更
    // デフォルトでは先頭まで戻す
    void Reset(uint32_t reallocationOffset = 0);

    operator ID3D12DescriptorHeap* () const { return descriptorHeap_.Get(); }

    DescriptorHandle GetDiscriptorHandle(uint32_t index) {
        DescriptorHandle handle;
        handle.cpu_ = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        descriptorStart_, index,
        descriptorSize_);

        return handle;
    }

private:
    // コピー禁止
    DescriptorHeap(const DescriptorHeap&) = delete;
    DescriptorHeap& operator=(const DescriptorHeap&) = delete;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    D3D12_DESCRIPTOR_HEAP_TYPE type_ = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    DescriptorHandle descriptorStart_;
    uint32_t numDescriptors_ = 0;
    uint32_t descriptorSize_ = 0;
    uint32_t freeDescriptorOffset_ = 0;
};