#include "MoveFloor.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

void MoveFloor::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	std::string name = "stagefloor";
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_, "movefloor", modelHandle_);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	moveParam_.startPos_ = Vector3(-10.0f, 0.0f, 0.0f);
	moveParam_.endPos_ = Vector3(10.0f, 0.0f, 0.0f);
	moveParam_.speed_ = 30.0f;
	moveParam_.T = 0.0f;

}

void MoveFloor::Initialize(const Vector3& startPos, const Vector3& endPos, const Quaternion& quaternion, const Vector3& scale, const float& speed) {
	std::string name = "stagefloor";
	GameObject::Initialize(name);
	collider_.Initialize(&worldTransform_, "movefloor", modelHandle_);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = startPos;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	moveParam_.startPos_ = startPos;
	moveParam_.endPos_ = endPos;
	moveParam_.speed_ = speed;
	moveParam_.T = 0.0f;
}

void MoveFloor::Update() {
	Move();

	collider_.AdjustmentScale();
	UpdateMatrix();
}

void MoveFloor::Draw() {
	collider_.Draw();
	GameObject::Draw();
}

void MoveFloor::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("start", &moveParam_.startPos_.x, 0.1f);
	ImGui::DragFloat3("end", &moveParam_.endPos_.x, 0.1f);
	ImGui::DragFloat("speed", &moveParam_.speed_, 0.1f);
	

	UpdateMatrix();
#endif // _DEBUG
}

void MoveFloor::Move() {
	static bool flag = true;

	if (moveParam_.T >= 1.0f) {
		flag = false;
		moveParam_.T = 1.0f;
	}
	else if (moveParam_.T <= 0.0f) {
		flag = true;
		moveParam_.T = 0.0f;
	}

	if (flag) {
		moveParam_.T += 1.0f / moveParam_.speed_;
	}
	else {
		moveParam_.T -= 1.0f / moveParam_.speed_;
	}


	worldTransform_.translation_ = Lerp(moveParam_.startPos_, moveParam_.endPos_, moveParam_.T);
	
}
