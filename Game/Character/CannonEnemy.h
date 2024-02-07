#pragma once
#include "BaseCharacter.h"
#include <list>
#include <memory>

// 大砲の敵
class CannonEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;
	void DrawImGui() override;

private:
	void Attack(const Vector3& playerPosition);
	void CollisionProcess(); // 衝突時の処理
	void DownAnimation();
private:
	class Bullet : public BaseCharacter {
	public:
		Bullet();
		void Initialize(const Vector3& translate, const Vector3& shotVec, const float& speed);
		void Update(const Vector3& playerPosition) override;
		void OnCollision(Collider& collider, const PlayerDate& date) override;
		void Draw() override;
		void EnemyDraw() override;
		bool isDead_ = false;
		Vector3 shotVec_;
		float speed_ = 1.0f;
	};

public:
	uint32_t kInterval_ = 60u; // 弾の発射間隔
	float bulletSpeed_ = 1.0f;
private:
	uint32_t timer_ = 0u;
	std::list<std::unique_ptr<Bullet>> bullets_;

	bool isHit_ = false;
	uint32_t hp_ = 1;
	uint32_t id_ = 0u;
	uint32_t count = 0;
	Vector3 knockBackVector_;

};
