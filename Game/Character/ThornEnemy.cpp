#include "ThornEnemy.h"
#include "ModelManager.h"

void ThornEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	std::vector<std::string> names = {
		"enemy01", // 親
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
	auto& modelTrans = modelsTransform_.at(0);
	modelTrans.SetParent(&worldTransform_);
	modelTrans.translation_.y += modelSize.y / 2.0f;
	modelTrans.Update();

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

void ThornEnemy::SetState(const Vector3& amplitube, const uint32_t time) {
	amplitude_ = amplitube;
	kMaxTime_ = time;
	timer_ = time / 2u;
}

void ThornEnemy::Move() {
	Vector3 moveVec;

	if (timer_++ > kMaxTime_) {
		isMovingToWhich = !isMovingToWhich;
		timer_ = 0u;
	}

	const Vector3 velocity = amplitude_ / static_cast<float>(kMaxTime_);
	isMovingToWhich ? moveVec += velocity : moveVec -= velocity;
	worldTransform_.translation_ += moveVec;
}
