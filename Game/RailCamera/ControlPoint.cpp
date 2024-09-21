#include "ControlPoint.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Game/Character/BaseCharacter.h"
#undef min

void ControlPoint::Initialize(Quaternion quaternion, Vector3 translate) {
	std::string name = "box1x1";
	GameObject::Initialize(name);

	worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;
}

void ControlPoint::Update() {
	UpdateMatrix();
}

void ControlPoint::Draw() {
#ifdef _DEBUG
	GameObject::Draw();
#endif // !_DEBUG
}

void ControlPoint::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}
