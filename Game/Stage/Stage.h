#pragma once
#include "Wall.h"
#include "WallLight.h"
#include "Floor.h"
#include "Truck.h"
#include "WoodBox.h"
#include "MoveFloor.h"
#include "StageLight.h"
#include "TrapButton.h"
#include <memory>
#include <vector>
#include <filesystem>
class SpotLights;
class PointLights;
class ShadowSpotLights;
class Player;
class EnemyManager;

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(const std::filesystem::path& loadFile, PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowspotLight);
	void Update(const Vector3& playerWorldPosition);
	void Draw();
	void ShadowDraw();
	void SpotLightShadowDraw();
	void DrawImGui();
	void Load(const std::filesystem::path& loadFile);
	void Save(const char* itemName);

	void Collision(Player* player);
	void Collision(EnemyManager* enemis);
	Collider& GetWallCollider(uint32_t number) const { return walls_.at(number)->collider_; }
	const std::vector<std::unique_ptr<Wall>>& GetWalls() const { return walls_; }
	
	/// <summary>
	/// *playerを渡し座標を移動させる
	/// </summary>
	/// <param name="player">playerのポインタ</param>
	void SetPlayerRespawn(class Player* const player) const;

private:
	void SetSpotLight();

private:
	std::vector<std::unique_ptr<Wall>> walls_;
	std::vector<std::unique_ptr<WallLight>> wallLights_;
	std::vector<std::unique_ptr<Floor>> floors_;
	std::vector<std::unique_ptr<Truck>> trucks_;
	std::vector<std::unique_ptr<WoodBox>> woodboxs_;
	std::vector<std::unique_ptr<MoveFloor>> moveFloors_;
	std::vector<std::unique_ptr<StageLight>> stagelights_;
	std::vector<std::unique_ptr<TrapButton>> trapButtons_;

	SpotLights* spotLights_;
	PointLights* pointLights_;
	ShadowSpotLights* shadowSpotLights_;

	struct SRT {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	SRT playerRespawnPoint_;

private: // モンスターハウス用
	struct MonstarHouseParam {
		uint32_t boxNumber; // 生成した木箱のindex
		uint32_t kBoxCount; // 木箱の数
		bool isFalled_ = false; // 落ちてくるフラグ
		bool isBreaked_ = false; // 壊れるフラグ
	};
	MonstarHouseParam mHouse_;
	void ConfineInitialize(const Vector3& position); // 閉じ込めるための初期化
	void Confine(); // 閉じ込める処理
	void ConfineBreak(); // 閉じ込める処理

};
