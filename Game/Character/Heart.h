#pragma once
#include "BaseCharacter.h"
class Heart : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;

protected:
	void CollisionProcess(); // 衝突時の処理
	void WaitProcess();

	int count = 0;
	bool isHit_ = false;

	class Audio* audio_ = nullptr;

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
};

