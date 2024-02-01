#pragma once
#include "GameObject.h"
#include "Collider.h"

// モンスターハウスを呼び出すためのオブジェクト
class TrapButton : public GameObject {
public:
	void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
	void Update(const Vector3& playerWorldPosition);
	void Draw();
	void DrawImGui();
public:
	Collider collider_;
	Vector3 rotate;
private:
	bool ActiveChack(const Vector3& playerWorldPosition) const;
	bool isActive_ = true;

};

