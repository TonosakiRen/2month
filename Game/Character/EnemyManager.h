#pragma once
#include <vector>
#include <memory>
#include <filesystem>

#include "NormalEnemy.h"
#include "NormalLightEnemy.h"
#include "ThornEnemy.h"
#include "StandLightEnemy.h"
#include "CannonEnemy.h"
#include "Coin.h"

class SpotLights;
class PointLights;
class ShadowSpotLights;
struct SRT;

// InGameSceneでのみ実体化。シングルトン運用
class EnemyManager {
public:
	EnemyManager();
	~EnemyManager();

	void Initialize(PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowSpotLight); // 将来的にリスポーン構造体ptrを渡す
	void Update(const Vector3& playerPosition);
	void OnCollisionPlayer(Collider& collider, const PlayerDate& date); // playerとの衝突判定を取得
	void OnCollisionStage(Collider& collider);

	void DrawImGui();
	void Save(const char* itemName);
	void Load(const std::filesystem::path& loadFilee);

	void Draw();
	void ShadowDraw();
	void SpotLightShadowDraw();

private:
	SpotLights* spotLights_ = nullptr;
	PointLights* pointLights_ = nullptr;
	ShadowSpotLights* shadowSpotLights_ = nullptr;

public:
	static const uint32_t kMaxEnemyCount = 100u;
	std::vector<std::unique_ptr<NormalEnemy>> nEnemis_;
	std::vector<std::unique_ptr<NormalLightEnemy>> nLightEnemis_;
	std::vector<std::unique_ptr<ThornEnemy>> tEnemis_;
	std::vector<std::unique_ptr<StandLightEnemy>> sLightEnemis_;
	std::vector<std::unique_ptr<CannonEnemy>> cEnemis_;
	std::vector<std::unique_ptr<Coin>> coins_;

};
