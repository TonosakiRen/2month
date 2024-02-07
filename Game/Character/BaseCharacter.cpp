#include "BaseCharacter.h"
#include "EnemyManager.h"

decltype(BaseCharacter::masterCount) BaseCharacter::masterCount = 0u;
BaseCharacter::BaseCharacter() : kNumber_(masterCount) {
	if (++masterCount >= EnemyManager::kMaxEnemyCount) {
		masterCount = 0u;
	}
}

BaseCharacter::~BaseCharacter(){
	
}

void BaseCharacter::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	if (ImGui::Button("isDraw")) {
		shadowOnly_ = !shadowOnly_;
	}

	UpdateTransform();
#endif // _DEBUG
}

void BaseCharacter::PushBackCollision(Collider& collider) {
	Vector3 pushBackVector;
	if (this->collider_.Collision(collider, pushBackVector)) {
		worldTransform_.translation_ += pushBackVector;
		UpdateTransform();
	}
}

void BaseCharacter::BaseInitialize(const uint32_t& modelNumber, std::vector<std::string> names) {
	models_.resize(modelNumber);
	modelsTransform_.resize(modelNumber);

	worldTransform_.Initialize();
	for (uint32_t index = 0u; index < modelNumber; index++) {
		models_.at(index).Initialize(names.at(index));
		modelsTransform_.at(index).Initialize();
	}
}

void BaseCharacter::UpdateTransform() {
	worldTransform_.Update();
	for (auto& trans : modelsTransform_) {
		trans.Update();
	}
}

void BaseCharacter::BaseDraw() {
	if (shadowOnly_) { return; }
	for (uint32_t index = 0u; index < models_.size(); index++) {
		models_.at(index).Draw(modelsTransform_.at(index));
	}
}

void BaseCharacter::BaseEnemyDraw() {
	for (uint32_t index = 0u; index < models_.size(); index++) {
		models_.at(index).EnemyDraw(Vector2(2, static_cast<float>(kNumber_)), modelsTransform_.at(index));
	}
}

void BaseCharacter::BaseCoinDraw() {
	for (uint32_t index = 0u; index < models_.size(); index++) {
		models_.at(index).EnemyDraw(Vector2(3, static_cast<float>(kNumber_)), modelsTransform_.at(index));
	}
}

void BaseCharacter::BaseHeartDraw() {
	for (uint32_t index = 0u; index < models_.size(); index++) {
		models_.at(index).EnemyDraw(Vector2(4, static_cast<float>(kNumber_)), modelsTransform_.at(index));
	}
}

