#pragma once
#include "BaseSpawner.h"
#include <vector>

class NormalEnemy;

class NormalSpawner : public BaseSpawner {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void OnCollision(Collider& collider, const PlayerDate& date) override;

	bool EnemySpawn();

private:
	struct SRT {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	SRT respawnPoint_;

	bool isSpawn_ = false; // trueになった時に生成
public:
	const SRT& GetSRT() const { return respawnPoint_; }

};