#pragma once
#include "NormalEnemy.h"
#include <vector>
#include <memory>

// InGameSceneでのみ実体化。シングルトン運用
class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize(); // 将来的にリスポーン構造体ptrを渡す
	void Update(const Vector3& playerPosition);
	void OnCollisionPlayer(Collider& collider, const PlayerDate& date); // playerとの衝突判定を取得

	void Draw();
	void ShadowDraw();
	void SpotLightShadowDraw();

private:

public:
	static const uint32_t kMaxEnemyCount = 10u;
	std::vector<std::unique_ptr<NormalEnemy>> nEnemis_;

};
