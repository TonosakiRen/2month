#pragma once
#include "Wall.h"
#include <memory>
#include <vector>
#include <filesystem>
class SpotLights;
class PointLights;

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(const std::filesystem::path& loadFile, SpotLights* spotLight, PointLights* pointLight);
	void Update();
	void Draw();
	void DrawImGui();
	void Load(const std::filesystem::path& loadFile);

	Collider& GetWallCollider(uint32_t number) const { return walls_.at(number)->collider_; }
	const std::vector<std::unique_ptr<Wall>>& GetWalls() const { return walls_; }
	
	/// <summary>
	/// *playerを渡し座標を移動させる
	/// </summary>
	/// <param name="player">playerのポインタ</param>
	void SetPlayerRespawn(class Player* const player) const;
private:
	std::vector<std::unique_ptr<Wall>> walls_;
public:
	SpotLights* spotLights_;
	PointLights* pointLights_;


	struct SRT {
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
	};
	SRT playerRespawnPoint_;

};
