#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"
#include <vector>

class DirectionalLights
{
public:

	static const uint32_t  lightNum = 1;

	struct DirectionalLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 direction = { 1.0f, -1.0f, 1.0f };
		float intensity = 1.0f;
	};

	void Initialize();
	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}
public:
	std::vector<DirectionalLight> lights_;
private:
	UploadBuffer constBuffer_;
};

