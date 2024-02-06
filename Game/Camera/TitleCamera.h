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
