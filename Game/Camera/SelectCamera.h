#pragma once
#include "WorldTransform.h"

class SelectCamera {
public:
	SelectCamera();
	~SelectCamera() = default;

	void Update();
	const WorldTransform& GetTransform() const { return transform_; }
	inline void ChangeStage() { T = 0.0f; }
private:
	void DrawImGui();
	void Move();

	WorldTransform transform_;

	struct RT {
		Quaternion rotate;
		Vector3 translate;
	};
	Vector3 rotate;
	float T = 1.0f;

};

