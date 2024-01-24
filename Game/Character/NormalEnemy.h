#pragma once
#include "BaseCharacter.h"

class NormalEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;


	Collider collider_;
protected:
	Vector3 Move(const Vector3& playerPosition);
	void KnockBack();
	void DownAnimation();

	void CollisionProcess(); // 衝突時の処理

	int count = 0;
	bool isHit_ = false;
	uint32_t id_;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector3 playerPosition_;
	Vector3 knockBackVector_;

	int hp_ = 0;

};

