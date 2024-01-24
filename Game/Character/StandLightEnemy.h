#pragma once
#include "BaseCharacter.h"

// 画面の前側にいてShadowSpotLightを使う敵
class StandLightEnemy : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate) override;
	void Update(const Vector3& playerPosition) override;
	void Draw() override;
	void EnemyDraw() override;
	void DrawImGui() override;

	void SetParameters(const uint32_t& maxTime, const uint32_t& MaxFlashCount,const uint32_t& Interval);

private:
	Vector3 FindDirection(const Vector3& playerPosition);
	void MoveLight();
	void Flashing();

private:
	Vector3 lightDirection_;

	bool isLightActive_ = true; // lightがオンかオフか
	uint32_t timer_ = 0u;
	uint32_t flashCount_ = 0u;
public:
	uint32_t kMaxTime_ = 0u; // 何フレーム明かりがついたり消えたりしているか
	uint32_t kMaxFlashCount_ = 0u; // 何回点滅させるか かならず偶数にしないと逆にならない
	uint32_t kInterval_ = 0u; // 点滅間隔フレーム

};

