#pragma once
#include "BaseCharacter.h"
#include <list>

// 大砲の敵
class CannonEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;
	void DrawImGui() override;
	std::vector<Collider> colliders_;

private:

	uint32_t interval_ = 0u; // 弾の発射間隔



};
