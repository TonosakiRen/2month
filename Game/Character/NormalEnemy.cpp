#include "NormalEnemy.h"
#include "ModelManager.h"

void NormalEnemy::Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate) {
	GameObject::Initialize("enemy");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	UpdateMatrix();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, "Enemy", modelHandle_);
}

void NormalEnemy::Update() {
	DrawImGui();

	Move();

	collider_.AdjustmentScale();
	UpdateMatrix();
}

void NormalEnemy::Draw() {
	collider_.Draw();
	GameObject::Draw();
}

void NormalEnemy::Move() {
	static bool flag = true;
	Vector3 vel;
	count++;
	if (count > 30) {
		flag = !flag;
		count = 0;
	}

	flag ? vel.x = -1.0f : vel.x = 1.0f;

	worldTransform_.translation_ += vel;
	UpdateMatrix();
}
