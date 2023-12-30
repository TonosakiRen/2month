#include "Wall.h"
#include "ModelManager.h"
#include "externals/imgui/imgui.h"
void Wall::Initialize(Vector3 position)
{
	GameObject::Initialize("wall");
	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, "wall", { modelSize.x, modelSize.y, 2.0f }, { 0.0f, 5.0f, 0.0f });
	worldTransform_.translation_ = position;
	UpdateMatrix();
}

void Wall::Update()
{
	collider_.AdjustmentScale();
	UpdateMatrix();
}

void Wall::Draw() {
	collider_.Draw();
	GameObject::Draw();
}

void Wall::DrawImGui() {
#ifdef _DEBUG
	static Vector3 rotate;
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);

	UpdateMatrix();
#endif // _DEBUG
}
