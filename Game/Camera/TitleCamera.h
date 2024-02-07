#pragma once
#include "WorldTransform.h"

class TitleCamera {
public:
	TitleCamera() = default;
	~TitleCamera() = default;

	void Inisialize();
	void Update();
	const WorldTransform& GetTransform() const { return transform_; }

private:
	void DrawImGui();
	//Vector3 Shake(Vector3 shakeValue);

	WorldTransform transform_;
	Vector3 rotate;

};
