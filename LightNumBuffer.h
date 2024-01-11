#pragma once
#include "UploadBuffer.h"
#include <stdint.h>
class LightNumBuffer
{
public:
	void Create();

	struct LightNum {
		int32_t directionalLightNum;
		int32_t pointLightNum;
		int32_t spotLightNum;
	};

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return lightNumBuffer_.GetGPUVirtualAddress();
	}

	UploadBuffer lightNumBuffer_;
};

