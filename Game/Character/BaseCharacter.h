#pragma once
#include "GameObject.h"
#include "Collider.h"
#include "ImGuiManager.h"

class BaseCharacter : public GameObject {
public:
	virtual void Initialize(Vector3 scale = Vector3(1.0f, 1.0f, 1.0f), Quaternion quaternion = Quaternion(0.0f, 0.0f, 0.0f, 1.0f), Vector3 translate = Vector3(0.0f, 0.0f, 0.0f)) {};
	virtual void Update() {};
	virtual void Draw() {};
	virtual void DrawImGui();

	Collider collider_;

protected:
	// 回転用
	Vector3 rotate;

};