#include "Floor.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"

void Floor::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	GameObject::Initialize("stagefloor");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, "Floor", modelHandle_);
}

void Floor::Update() {
	collider_.AdjustmentScale();
	UpdateMatrix();
}

void Floor::Draw() {
	collider_.Draw();
	GameObject::Draw();
}

void Floor::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}
