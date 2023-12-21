#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include <numbers>
#include "UploadBuffer.h"

class ViewProjection {
	friend class DebugCamera;
	friend class Camera;
public:

	static bool isUseDebugCamera;

	struct ConstBufferData {
		Matrix4x4 viewProjection;
		Matrix4x4 inverseViewProjection;
		Vector3 viewPosition;
	};

	static void SwitchIsUseDebugCamera();

	void Initialize();
	void Update();

	bool Shake(Vector3 shakeValue, int& frame);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuffer_.GetGPUVirtualAddress();
	}

	void SetFarZ(float farZ) {
		farZ_ = farZ;
	}

	const Matrix4x4 GetMatView() const {
		return matView;
	}
private:
	Vector3 translation_ = { 0.0f, 7.0f, -27.0f };
	Quaternion quaternion_ = { 0.0f, 0.0f, 0.0f,1.0f };
	Vector3 shakeValue_ = { 0.0f,0.0f,0.0f };

	float orthographicValue_ = 1.0f;

	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)16 / (float)9;
	float nearZ_ = 0.1f;
	float farZ_ = 100.0f;

	Matrix4x4 matView;
	Matrix4x4 matProjection;

	UploadBuffer constBuffer_;
};