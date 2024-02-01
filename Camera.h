#pragma once
#include "ViewProjection.h"
#include <memory>
class Camera :
	public ViewProjection
{
public:
	void Initialize();
	void Initialize(Vector3 position, Quaternion rotation);
	void Update(float playerX);
	void Update(const Vector3& position, const Quaternion& rotation);
private:
	float savePlayerX = 0.0f;
};