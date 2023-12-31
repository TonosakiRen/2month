#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"
#include "DepthBuffer.h"
#include <vector>

class DirectionalLights
{
public:

	static const uint32_t  lightNum = 1;

	struct DirectionalLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		Vector3 position = { 0.0f,200.0f,0.0f };
		float intensity = 1.0f;
		Matrix4x4 viewProjection;
		Vector3 lockUp = { 1.0f,0.0f,0.0f };
		DepthBuffer shadowMap_;
		UploadBuffer constBuffer_;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Matrix4x4 viewProjection;
	};

	void Initialize();
	void Update();

	//影用viewProjection
	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)1 / (float)1;
	float nearZ_ = 0.1f;
	float farZ_ = 500.0f;

public:
	std::vector<DirectionalLight> lights_;
private:
	
};

