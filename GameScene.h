#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Sprite.h"
#include "DirectionalLights.h"
#include "Compute.h"
#include "GameObject.h"

#include "Skydome.h"
#include "Floor.h"
#include "Player.h"
#include "DustParticle.h"
#include "WhiteParticle.h"
#include "Wall.h"


#include <optional>
class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update(CommandContext& commandContext);
	void ModelDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
	void Draw(CommandContext& commandContext);
	void UIDraw(CommandContext& commandContext);

	const DirectionalLights& GetDirectionalLights() {
		return directionalLights_;
	}

	const ViewProjection& GetViewProjection() {
		return *currentViewProjection_;
	}

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	std::unique_ptr <DebugCamera> debugCamera_;
	ViewProjection* currentViewProjection_ = nullptr;
	
	std::unique_ptr<Camera> camera_;
	DirectionalLights directionalLights_;

	uint32_t textureHandle_;

	std::unique_ptr<Sprite>sprite_;
	WorldTransform spriteTransform_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<DustParticle> dustParticle_;

	std::unique_ptr<WhiteParticle> whiteParticle_;

	std::vector<std::unique_ptr<Wall>> walls_;

	Vector4 color = {1.0f,1.0f,1.0f,1.0f};

	std::unique_ptr<Compute> compute_;

	//Scene
	enum class Scene {
		Title,
		InGame,

		SceneNum
	};

	Scene scene_ = Scene::InGame;
	Scene nextScene = Scene::InGame;
	static void (GameScene::* SceneInitializeTable[])();
	static void (GameScene::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	//タイトル
	void TitleInitialize();
	void TitleUpdate();
	//インゲーム
	void InGameInitialize();
	void InGameUpdate();
};

