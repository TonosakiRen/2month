#pragma once
#include "BaseCharacter.h"

class ThornEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;
	void DrawImGui() override;

	void SetState(const Vector3& amplitube, const uint32_t time);


	Collider collider_;

private:
	void Move();
public:
	Vector3 amplitude_; // 振れ幅
	int kMaxTime_ = 0; // 持続時間
private:
	bool isMovingToWhich = false; // 例 false = 左 / true = 右
	uint32_t timer_ = 0u; // 持続時間

};