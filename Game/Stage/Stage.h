#pragma once
#include "Wall.h"
#include "WallLight.h"
#include "Floor.h"
#include "Truck.h"

#include <memory>
#include <vector>
#include <filesystem>
class SpotLights;
class PointLights;
class ShadowSpotLights;

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(const std::filesystem::path& loadFile, PointLights* pointLight, SpotLights* spotLight, ShadowSpotLights* shadowspotLight);
	void Update();
	void Draw();
	void DrawImGui();
	void Load(const std::filesystem::path& loadFile);
	void Save(const char* itemName);

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
public:
	SpotLights* spotLights_;
	PointLights* pointLights_;
	ShadowSpotLights* shadowSpotLights_;


	struct SRT {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	SRT playerRespawnPoint_;

};
