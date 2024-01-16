#pragma once
#include "BaseCharacter.h"
#include <vector>

class NormalEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const float& sensedDistance) override;
	void OnCollision(Collider& collider) override;
	void Draw() override;

private:
	void Move();
	void UpdateTransform();

	int count = 0;
	std::vector<WorldTransform> modelTransform_;
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


};

