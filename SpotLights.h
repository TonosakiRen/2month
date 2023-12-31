#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"
#include "DepthBuffer.h"
#include <vector>

class SpotLights
{
public:

	static const uint32_t  lightNum = 1;

	struct SpotLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f,0.0f,-1.0f };
		float intensity = 1.0f;
		Vector3 direction = {1.0f,0.0f,0.0f};
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		UploadBuffer constBuffer_;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		Vector3 direction = { 1.0f,0.0f,0.0f };
		float distance = 1.0f;
		float decay = 1.0f;
		float cosAngle = Radian(45.0f);
		float padding[2];
	};

	void Initialize();
	void Update();

public:
	std::vector<SpotLight> lights_;
private:

};

