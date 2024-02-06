#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Audio.h"
#include "Sprite.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"
#include "ShadowSpotLights.h"
#include "Compute.h"
#include "GameObject.h"

#include "Skydome.h"
#include "Player.h"
#include "DustParticle.h"
#include "WhiteParticle.h"

#include "Game/Stage/Stage.h"
#include "Game/Scene/InGameScene.h"
#include "Game/Scene/CreateStageScene.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/StageSelectScene.h"

#include <optional>
class GameScene 
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update(CommandContext& commandContext);
	void ModelDraw();
	void DepthDraw();
	void ShadowDraw();
	void SpotLightShadowDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
	void Draw(CommandContext& commandContext);
	void ShadowMapDraw(CommandContext& commandContext);
	void SpotLightShadowMapDraw(CommandContext& commandContext);
	void UIDraw(CommandContext& commandContext);

	static void SetHitStop(int frame) {
		hitStopFrame_ = frame;
	}

	DirectionalLights& GetDirectionalLights() {
		return directionalLights_;
	}

	PointLights& GetPointLights() {
		return pointLights_;
	}

	SpotLights& GetSpotLights() {
		return spotLights_;
	}

	ShadowSpotLights& GetShadowSpotLights() {
		return shadowSpotLights_;
	}

	const ViewProjection& GetViewProjection() {
		return *currentViewProjection_;
	}

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	static int hitStopFrame_;

	std::unique_ptr <DebugCamera> debugCamera_;
	ViewProjection* currentViewProjection_ = nullptr;
	
	std::unique_ptr<Camera> camera_;
	DirectionalLights directionalLights_;
	PointLights pointLights_;
	ShadowSpotLights shadowSpotLights_;
	SpotLights spotLights_;

	uint32_t textureHandle_ = 0u;

	std::unique_ptr<Sprite> sprite_;

	std::unique_ptr<Sprite> ui_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<GameObject> sphere_;

	std::unique_ptr<DustParticle> dustParticle_;

	std::unique_ptr<WhiteParticle> whiteParticle_;

	Vector4 color = {1.0f,1.0f,1.0f,1.0f};

	// Scene用クラス
	std::unique_ptr<InGameScene> inGameScene_;
	std::unique_ptr<CreateStageScene> editorScene_;
	std::unique_ptr<TitleScene> titleScene_;
	std::unique_ptr<StageSelectScene> selectScene_;

	//Scene
	enum class Scene {
		Title,
		StageSelect,
		InGame,
		Editor,

		SceneNum
	};

	Scene scene_ = Scene::SceneNum;
	static void (GameScene::* SceneInitializeTable[])();
	static void (GameScene::* SceneUpdateTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;
	std::optional<Scene> saveSceneRequest_ = std::nullopt;

	//タイトル
	void TitleInitialize();
	void TitleUpdate();
	// ステージセレクト
	void StageSelectInitialize();
	void StageSelectUpdate();

	//インゲーム
	void InGameInitialize();
	void InGameUpdate();
	
	// エディター
	void EditorInitialize();
	void EditorUpdate();


};

