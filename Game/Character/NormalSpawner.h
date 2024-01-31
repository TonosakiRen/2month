#pragma once
#include "BaseCharacter.h"
#include <vector>

class NormalEnemy;

class NormalSpawner : public BaseCharacter {
public: // 仮想関数
	void Initialize(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate, const uint32_t& interval = 60u, const uint32_t& MaxPop = 10u, const int& hp = 1);
	bool UpdateSpawn(const Vector3& playerPosition);
	void OnCollision(Collider& collider, const PlayerDate& date) override;
	void Draw() override;
	void EnemyDraw() override;
	void DrawImGui() override;

private:
	bool EnemySpawn();
	void InsertData();

private:
	struct SRT {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
		// 以下入出力用パラメーター
		int interval;
		int MaxPopEnemy;
		int hp;
	};
	SRT respawnPoint_;

	bool isHit_ = false;
	bool isSpawn_ = false; // trueになった時に生成
	uint32_t popInterval_ = 60u; // 沸き間隔
	uint32_t poptimer_ = 0u; // スポーン用タイマー
	int hp_ = 1; // スポナーの体力
	uint32_t kMaxPopEnemy_ = 10u; // 敵が最大何体まで沸けるか
	uint32_t popCount_ = 0u; // 敵が何体出たかのカウント
	Vector4 color_;


public:
	const SRT& GetSRT() const { return respawnPoint_; }

};