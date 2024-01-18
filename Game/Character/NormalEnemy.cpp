#include "NormalEnemy.h"
#include "ModelManager.h"
#include "Player.h"

void NormalEnemy::Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) {
	GameObject::Initialize("enemy01");
	worldTransform_.scale_ = scale;
	worldTransform_.quaternion_ = quaternion;
	worldTransform_.translation_ = translate;
	worldTransform_.Update();
	rotate = EulerAngle(worldTransform_.quaternion_);
	rotate.x = Degree(rotate.x) - 180.0f; rotate.y = Degree(rotate.y) - 180.0f; rotate.z = Degree(rotate.z) - 180.0f;

	Vector3 modelSize = ModelManager::GetInstance()->GetModelSize(modelHandle_);

	// とりあえず一個だけ
	auto& modelTrans = modelTransform_.emplace_back(WorldTransform());
	modelTrans.Initialize();
	modelTrans.SetParent(&worldTransform_);
	//modelTrans.translation_.y += modelSize.y / 2.0f;
	modelTrans.Update();

	collider_.Initialize(&worldTransform_, "Enemy", modelHandle_);
}

void NormalEnemy::Update(const Vector3& playerPosition) {
	playerPosition_ = playerPosition;
	float distance = Distance(playerPosition, worldTransform_.translation_);
	ImGui::Begin("Enemy");
	DrawImGui();
	ImGui::Text("distance : %f", distance);
	ImGui::End();
	
	const float kMaxDistance = 50.0f;
	// Playerとの距離が一定数以下なら早期リターン
	// 後で調整。画面外で処理を走らせないのが目的
	if (distance > kMaxDistance) {
		return;
	}
	
	if (!isHit_) {
		Move(playerPosition);
	}
	else {
		KnockBack();
	}
	
	UpdateTransform();
}

void NormalEnemy::OnCollision(Collider& collider, const PlayerDate& date) {
	Vector3 pushBackVector;
	date;
	if (collider_.Collision(collider, pushBackVector)) {
		
		if (!isHit_ ) {
			isHit_ = true;
			knockBackVector = playerPosition_ - worldTransform_.translation_;
		}

		UpdateTransform();
	}
}

void NormalEnemy::Draw() {
	collider_.Draw();
	GameObject::Draw(modelTransform_.at(0));
}

void NormalEnemy::Move(const Vector3& playerPosition) {

	Vector3 vec = playerPosition - worldTransform_.translation_;
	vec = Normalize(vec);
	if (isnan(vec.x) || isnan(vec.y) || isnan(vec.z)) {
		vec = Vector3(0.0f, 0.0f, 0.0f);
	}
	vec.y = 0.0f;


	const float kSpeed = 0.05f;

	worldTransform_.quaternion_ = MakeLookRotation(-vec);

	worldTransform_.translation_ += vec * kSpeed;
	UpdateMatrix();

}

void NormalEnemy::UpdateTransform() {
	collider_.AdjustmentScale();
	worldTransform_.Update();
	for (auto& model : modelTransform_) {
		model.Update();
	}
}

void NormalEnemy::KnockBack() {
	Vector3 vec = knockBackVector;
	vec = Normalize(vec);
	vec.y = 0.0f;
	const float speed = 1.5f;

	worldTransform_.translation_ += -vec * speed;
	if (++count >= 5) {
		isHit_ = false;
		count = 0;
	}

}
