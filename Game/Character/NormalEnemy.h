#pragma once
#include "BaseCharacter.h"
#include <vector>

class NormalEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;

private:
	void Move(const Vector3& playerPosition);
	void UpdateTransform();
	void KnockBack();

	int count = 0;
	bool isHit_ = false;
	uint32_t id_;
	std::vector<WorldTransform> modelTransform_;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	Vector3 playerPosition_;
	Vector3 knockBackVector_;

};

