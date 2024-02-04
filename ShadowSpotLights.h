#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include <vector>
#include "WorldTransform.h"
#include "GlobalVariables.h"

class ShadowSpotLights
{
public:

	static const uint32_t  lightNum = 5;
	static const uint32_t shadowWidth = 1024;
	static const uint32_t shadowHeight = 1024;

	struct SpotLight {
		Vector4 color = { 1.0f,1.0f,0.58f };
		WorldTransform worldTransform;
		float intensity = 5.5f;
		Vector3 direction = { 0.0f,0.0f,1.0f };
		float distance = 20.0f;
		float shadeDistance = 5.0f;
		float decay = 1.0f;
		float cosAngle = 0.8f;
		bool isActive = false;
		float playerDistance = 0.0f;
		Vector3 lockUp = { 1.0f,0.0f,0.0f };
		DepthBuffer shadowMap_;
		UploadBuffer constBuffer_;
		ColorBuffer collisionData;
		uint32_t shadowDescriptorHeapIndex;
		uint32_t collisionDescriptorHeapIndex;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		float isActive = false;
		uint32_t shadowDescriptorHeapIndex;
		uint32_t collisionDescriptorHeapIndex;
		uint32_t padding;
		uint32_t padding2;
		uint32_t padding3;
		Matrix4x4 viewProjection;
	};

	void Initialize();
	void SetGlobalVariable();
	void ApplyGlobalVariable();
	void Update();

public:
	std::vector<SpotLight> lights_;
	UploadBuffer structureBuffer_;
	DescriptorHandle srvHandle_;
	GlobalVariables* globalVariables_ = nullptr;
private:
	//影用viewProjection
	float fovAngleY_ = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio_ = (float)1 / (float)1;
	float nearZ_ = 0.001f;
};

