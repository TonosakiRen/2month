#pragma once
#include "ViewProjection.h"
#include <memory>
class DebugCamera :
	public ViewProjection
{
public:
	void Initialize();
	void Initialize(Vector3 position,Vector3 rotation);
	void Update();
private:
	Vector3 debugRotation_ = { 0.0f,0.0f,0.0f };
	void DebugMode();
};

