#include "ThornEnemy.h"
#include "ModelManager.h"

void ThornEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"toge", // 親
	};

	BaseInitialize(1, names);
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(models_.at(0).modelHandle_);

	// とりあえず一個だけ
	modelsTransform_.at(0).SetParent(&worldTransform_);
	modelsTransform_.at(0).translation_ = Vector3(0.0f, 0.0f, 0.0f);
	//modelsTransform_.at(0).translation_.y += modelSize.y / 2.0f;
	modelsTransform_.at(0).Update();

	amplitude_ = Vector3(1.0f, 0.0f, 0.0f);

	collider_.Initialize(&worldTransform_, "Enemy", models_.at(0).modelHandle_);
}

void ThornEnemy::Update(const Vector3& playerPosition) {
	Move();

	collider_.AdjustmentScale();
	UpdateTransform();
}

void ThornEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector)) {
		date;
	}
}

void ThornEnemy::Draw() {
	collider_.Draw();
	BaseDraw();
}

void ThornEnemy::EnemyDraw() {
	BaseEnemyDraw();
}

void ThornEnemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.1f, -360.0f, 360.0f);
	Vector3 handle = Vector3(Radian(rotate.x), Radian(rotate.y), Radian(rotate.z));
	worldTransform_.quaternion_ = MakeFromEulerAngle(handle);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("amplitude", &amplitude_.x, 0.1f);
	ImGui::DragInt("Count", &kMaxTime_, 1, 1, 1000);
	UpdateTransform();
#endif // _DEBUG
}

void ThornEnemy::SetState(const Vector3& amplitube, const uint32_t time) {
	amplitude_ = amplitube;
	kMaxTime_ = time;
	timer_ = time / 2u;
}

void ThornEnemy::Move() {
	Vector3 moveVec;

	if (timer_++ > static_cast<int>(kMaxTime_)) {
		isMovingToWhich = !isMovingToWhich;
		timer_ = 0u;
	}

	const Vector3 velocity = amplitude_ / static_cast<float>(kMaxTime_);
	isMovingToWhich ? moveVec += velocity : moveVec -= velocity;
	worldTransform_.translation_ += moveVec;
}