#include "NormalEnemy.h"
#include "ModelManager.h"

void NormalEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	GameObject::Initialize("player");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);
	collider_.Initialize(&worldTransform_, "Enemy", modelHandle_);

	// とりあえず一個だけ
	auto& modelTrans = modelTransform_.emplace_back(WorldTransform());
	modelTrans.Initialize();
	modelTrans.SetParent(&worldTransform_);
	modelTrans.Update();

}

void NormalEnemy::Update(const float& sensedDistance) {
	DrawImGui();

	Move();

	
	UpdateTransform();
}

void NormalEnemy::OnCollision(Collider& collider) {
	Vector3 pushBackVector;
	if (collider_.Collision(collider, pushBackVector)) {
		
	}
}

void NormalEnemy::Draw() {
	collider_.Draw();
	GameObject::Draw(modelTransform_.at(0), color);
}

void NormalEnemy::Move() {
	static bool flag = true;
	Vector3 vel;
	count++;
	if (count > 30) {
		flag = !flag;
		count = 0;
	}
	const float speed = 0.5f;

	flag ? vel.x = -speed : vel.x = speed;

	worldTransform_.translation_ += vel;
	UpdateMatrix();
}

void NormalEnemy::UpdateTransform() {
	collider_.AdjustmentScale();
	worldTransform_.Update();
	for (auto& model : modelTransform_) {
		model.Update();
	}
}
