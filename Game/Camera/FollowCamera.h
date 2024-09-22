#pragma once
#include "WorldTransform.h"

class FollowCamera {
public:
	FollowCamera();
	~FollowCamera() = default;

	void Inisialize(const WorldTransform& transform);
	void Update(const Vector3& playerPosition);
	const WorldTransform& GetTransform() const { return transform_; }

private:
	void DrawImGui();
public:
	static inline Vector3 Shake(Vector3 shakeValue);

	WorldTransform transform_;
	float savePlayerX_ = 0.0f;
	float savePlayerY_ = 0.0f;

	struct RT {
		Quaternion rotate;
		Vector3 translate;
	};
	RT start_;
	RT end_;
	float count_ = 0.0f;
	bool isEase_ = false;

	Vector3 rotate;
};
