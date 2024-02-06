#pragma once
#include "WorldTransform.h"

class FixedCamera {
public:
	FixedCamera();
	~FixedCamera() = default;

	void Initialize(const float& trapPosition, const WorldTransform& transform);

	void Update(); 
	const WorldTransform& GetTransform() const { return transform_; }
	const bool& GetMove() const { return isMove_; }

private:
	void DrawImGui();
	Vector3 Shake(Vector3 shakeValue);

private:
	WorldTransform transform_;
	struct RT {
		Quaternion rotate;
		Vector3 translate;
	};
	RT start_;
	RT end_;
	float count_ = 0.0f;
	bool isMove_ = false;

	Vector3 rotate;
};
