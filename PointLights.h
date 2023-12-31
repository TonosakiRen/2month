#pragma once
#include <d3d12.h>
#include "Mymath.h"
#include "UploadBuffer.h"
#include "DepthBuffer.h"
#include <vector>

class PointLights
{
public:

	static const uint32_t  lightNum = 2;

	struct PointLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f,0.0f,-1.0f };
		float intensity = 1.0f;
		float radius = 1.0f;
		float decay = 1.0f;
	};

	struct ConstBufferData {
		Vector4 color = { 1.0f, 1.0f, 1.0f,1.0f };
		Vector3 position = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
		float radius = 1.0f;
		float decay = 1.0f;
	};

	void Initialize();
	void Update();

public:
	std::vector<PointLight> lights_;
	UploadBuffer structureBuffer_;
	DescriptorHandle srvHandle_;
private:

};

