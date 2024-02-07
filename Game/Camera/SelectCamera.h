#pragma once
#include "WorldTransform.h"

class SelectCamera {
public:
	SelectCamera();
	~SelectCamera() = default;

	void Update();
	const WorldTransform& GetTransform() const { return transform_; }
private:

	void DrawImGui();

	WorldTransform transform_;

	struct RT {
		Quaternion rotate;
		Vector3 translate;
	};
	Vector3 rotate;

};

