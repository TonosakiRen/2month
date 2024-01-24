#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "CommandContext.h"
#include "Renderer.h"
#include "ModelManager.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"

void (GameScene::* GameScene::SceneUpdateTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::InGameUpdate,
	&GameScene::EditorUpdate,
};

void (GameScene::* GameScene::SceneInitializeTable[])() = {
	&GameScene::TitleInitialize,
	&GameScene::InGameInitialize,
	&GameScene::EditorInitialize,
};


GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	Vector3 initializeCameraPos = { 0.0f,8.45f,-26.0f };
	Vector3 initializeCameraRotate = { 0.21f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(initializeCameraPos, initializeCameraRotate);

	camera_ = std::make_unique<Camera>();
	camera_->Initialize(initializeCameraPos, MakeFromEulerAngle(initializeCameraRotate));

	currentViewProjection_ = debugCamera_.get();

	directionalLights_.Initialize();
	directionalLights_.lights_[0].color = { 0.4f,0.49f,1.0f };
	directionalLights_.lights_[0].intensity = 0.31f;

	directionalLights_.Update();

	pointLights_.Initialize();
	pointLights_.Update();

	spotLights_.Initialize();
	spotLights_.lights_[0].worldTransform.translation_ = { -5.43f,6.71f,4.02f };
	spotLights_.lights_[0].color = {1.0f,1.0f,0.58f };
	spotLights_.lights_[0].intensity = 5.85f;
	spotLights_.lights_[0].direction = { 0.0f,-1.0f,0.0f };
	spotLights_.lights_[0].distance = 10.460f ;
	spotLights_.lights_[0].isActive = false;

	spotLights_.lights_[1].worldTransform.translation_ = { 5.85f,6.71f,4.02f };
	spotLights_.lights_[1].color = { 1.0f,1.0f,0.58f };
	spotLights_.lights_[1].intensity = 5.85f;
	spotLights_.lights_[1].direction = { 0.0f,-1.0f,0.0f };
	spotLights_.lights_[1].distance = 10.460f;
	spotLights_.lights_[1].isActive = false;

	spotLights_.lights_[2].worldTransform.translation_ = {0.03f,6.71f,-5.36f };
	spotLights_.lights_[2].color = { 1.0f,1.0f,0.58f };
	spotLights_.lights_[2].intensity = 5.85f;
	spotLights_.lights_[2].direction = { 0.0f,-1.0f,0.0f };
	spotLights_.lights_[2].distance = 10.460f;
	spotLights_.lights_[2].isActive = false;
	spotLights_.Update();

	shadowSpotLights_.Initialize();
	shadowSpotLights_.lights_[0].worldTransform.translation_ = { 0.0f,1.3f,-12.0f };
	shadowSpotLights_.lights_[0].color = { 1.0f,1.0f,0.58f };
	shadowSpotLights_.lights_[0].intensity = 5.5f;
	shadowSpotLights_.lights_[0].direction = { 0.0f,0.0f,1.0f };
	shadowSpotLights_.lights_[0].distance = 20.0f;
	shadowSpotLights_.lights_[0].cosAngle = 0.8f;
	shadowSpotLights_.lights_[0].isActive = true;

	shadowSpotLights_.lights_[1].worldTransform.translation_ = { 0.0f + 5.0f,1.3f,-12.0f };
	shadowSpotLights_.lights_[1].color = { 1.0f,1.0f,0.58f };
	shadowSpotLights_.lights_[1].intensity = 5.5f;
	shadowSpotLights_.lights_[1].direction = { 0.0f,0.0f,1.0f };
	shadowSpotLights_.lights_[1].distance = 20.0f;
	shadowSpotLights_.lights_[1].cosAngle = 0.8f;
	shadowSpotLights_.lights_[1].isActive = false;

	shadowSpotLights_.Update();

	// InGameSceneの生成と初期化
	inGameScene_ = std::make_unique<InGameScene>();
	inGameScene_->Initialize(&pointLights_,&spotLights_,&shadowSpotLights_);
	
	// 
	editorScene_ = std::make_unique<CreateStageScene>();
	editorScene_->Initialize(&pointLights_, &spotLights_,&shadowSpotLights_);

	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, { 0.0f,0.0f });

	ui_ = std::make_unique<Sprite>();
	uint32_t uiHandle = TextureManager::Load("sousa.png");
	ui_->Initialize(uiHandle, { 1140.0f,90.0f });

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome");

	sphere_ = std::make_unique<GameObject>();
	sphere_->Initialize("sphere");
	sphere_->SetPosition({ 0.0f,8.0f,0.0f });
	sphere_->UpdateMatrix();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->SetIsEmit(true);
	dustParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });

	whiteParticle_ = std::make_unique<WhiteParticle>();
	whiteParticle_->SetIsEmit(true);
	whiteParticle_->Initialize(Vector3{ -1.0f,-1.0f,-1.0f }, Vector3{ 1.0f,1.0f,1.0f });


	size_t bgmHandle = audio_->SoundLoadWave("BGM.wav");
	size_t bgmPlayHandle = audio_->SoundPlayLoopStart(bgmHandle);
	audio_->SetValume(bgmPlayHandle, 0.1f);

	// シーンリクエスト
	// editor使用時のみ初期からDebugCameraを使用
	sceneRequest_ = Scene::Editor;
	if (sceneRequest_ == Scene::Editor) {
		ViewProjection::isUseDebugCamera = true;
	}
}

void GameScene::Update(CommandContext& commandContext){
	Collider::SwitchIsDrawCollider();
	ViewProjection::SwitchIsUseDebugCamera();
	//fps表示
#ifdef _DEBUG
	ImGui::Begin("fps");
	auto& io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);
	ImGui::End();
#endif
	//camera light
	{
		if (scene_ == Scene::Editor) {
			ViewProjection::isUseDebugCamera = editorScene_->GetPlay();
		}

		if (ViewProjection::isUseDebugCamera) {
			currentViewProjection_ = debugCamera_.get();
			debugCamera_->Update();
		}
		else {
			currentViewProjection_ = camera_.get();
			float float_x = inGameScene_->GetPlayerTrans()->translation_.x;
			if (scene_ == Scene::Editor) {
				float_x = editorScene_->GetPlayerTrans()->translation_.x;
			}
			camera_->Update(float_x);
		}
		
		// light
#ifdef _DEBUG
		ImGui::Begin("DirectionalLight");
		ImGui::DragFloat3("lightDirection", &directionalLights_.lights_[0].direction.x, 0.01f);
		ImGui::DragFloat3("lightPosition", &directionalLights_.lights_[0].position.x, 1.0f);
		ImGui::DragFloat4("lightColor", &directionalLights_.lights_[0].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &directionalLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::End();

#endif
		directionalLights_.lights_[0].direction = Normalize(directionalLights_.lights_[0].direction);
		directionalLights_.Update();

#ifdef _DEBUG
		ImGui::Begin("pointLight1");
		ImGui::DragFloat3("lightPosition", &pointLights_.lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &pointLights_.lights_[0].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &pointLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat("radius", &pointLights_.lights_[0].radius, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &pointLights_.lights_[0].decay, 0.01f, 0.0f);
		ImGui::End();

		ImGui::Begin("pointLight3");
		ImGui::DragFloat3("lightPosition", &pointLights_.lights_[1].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &pointLights_.lights_[1].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &pointLights_.lights_[1].intensity, 0.01f, 0.0f);
		ImGui::DragFloat("radius", &pointLights_.lights_[1].radius, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &pointLights_.lights_[1].decay, 0.01f, 0.0f);
		ImGui::End();
#endif
		pointLights_.lights_[0].isActive = true;
		pointLights_.lights_[1].isActive = true;

		pointLights_.Update();

#ifdef _DEBUG
		ImGui::Begin("spotLight");
		ImGui::DragFloat3("lightPosition", &spotLights_.lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &spotLights_.lights_[0].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &spotLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &spotLights_.lights_[0].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &spotLights_.lights_[0].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &spotLights_.lights_[0].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &spotLights_.lights_[0].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();

		ImGui::Begin("spotLight2");
		ImGui::DragFloat3("lightPosition", &spotLights_.lights_[1].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &spotLights_.lights_[1].color.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat("intensity", &spotLights_.lights_[1].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &spotLights_.lights_[1].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &spotLights_.lights_[1].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &spotLights_.lights_[1].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &spotLights_.lights_[1].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();
#endif
		spotLights_.lights_[0].direction = Normalize(spotLights_.lights_[0].direction);
		spotLights_.lights_[1].direction = Normalize(spotLights_.lights_[1].direction);
		spotLights_.Update();

#ifdef _DEBUG
		ImGui::Begin("shadowSpotLight");
		ImGui::DragFloat3("lightPosition", &shadowSpotLights_.lights_[0].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &shadowSpotLights_.lights_[0].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &shadowSpotLights_.lights_[0].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &shadowSpotLights_.lights_[0].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &shadowSpotLights_.lights_[0].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &shadowSpotLights_.lights_[0].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &shadowSpotLights_.lights_[0].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();

		ImGui::Begin("shadowSpotLight2");
		ImGui::DragFloat3("lightPosition", &shadowSpotLights_.lights_[1].worldTransform.translation_.x, 0.01f);
		ImGui::DragFloat3("lightColor", &shadowSpotLights_.lights_[1].color.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("intensity", &shadowSpotLights_.lights_[1].intensity, 0.01f, 0.0f);
		ImGui::DragFloat3("direction", &shadowSpotLights_.lights_[1].direction.x, 0.01f, 0.0f);
		ImGui::DragFloat("distance", &shadowSpotLights_.lights_[1].distance, 0.01f, 0.0f);
		ImGui::DragFloat("decay", &shadowSpotLights_.lights_[1].decay, 0.01f, 0.0f);
		ImGui::DragFloat("cosAngle", &shadowSpotLights_.lights_[1].cosAngle, Radian(1.0f), 0.0f, Radian(179.0f));
		ImGui::End();

#endif
		shadowSpotLights_.lights_[0].lockUp = { 0.0f,1.0f,0.0f };
		shadowSpotLights_.lights_[0].direction = Normalize(shadowSpotLights_.lights_[0].direction);
		shadowSpotLights_.Update();
	}
	//Scene
	{
		//Scene初期化
		if (sceneRequest_) {
			scene_ = sceneRequest_.value();
			(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
			sceneRequest_ = std::nullopt;
		}
		//SceneUpdate
		(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();
	}

	uint32_t* a = static_cast<uint32_t*>(Compute::data_);
	if (a[1]) {
		int a = 0;
		a++;
	}
	
	
#ifdef _DEBUG
	//パーティクル
	ImGui::Begin("particle");
	ImGui::DragFloat4("particleColor", &color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("emitterPos", &whiteParticle_->emitterWorldTransform_.translation_.x, 0.01f);
	ImGui::End();
	whiteParticle_->SetIsEmit(false);
	whiteParticle_->Update();
#endif
}

void GameScene::TitleInitialize() {

}
void GameScene::TitleUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::InGame;
	}

}
void GameScene::InGameInitialize() {
	if (inGameScene_) {
		inGameScene_.reset(new InGameScene());
		inGameScene_->Initialize(&pointLights_, &spotLights_,&shadowSpotLights_);
	}
}
void GameScene::InGameUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::Title;
	}

	inGameScene_->Update();

	skydome_->Update(inGameScene_->GetPlayerTrans()->translation_);
}

void GameScene::EditorInitialize() {
	if (editorScene_) {
		editorScene_.reset(new CreateStageScene());
		editorScene_->Initialize(&pointLights_, &spotLights_,&shadowSpotLights_);
	}
}

void GameScene::EditorUpdate() {
	editorScene_->Update();
}

void GameScene::ModelDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		skydome_->Draw();
		inGameScene_->Draw();
		break;
	case GameScene::Scene::Editor:
		editorScene_->Draw();
		break;
	default:
		break;
	}
	
}

void GameScene::ShadowDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		inGameScene_->ShadowDraw();
		break;
	case GameScene::Scene::Editor:
		editorScene_->ShadowDraw();
		break;
	default:
		break;
	}
}

void GameScene::SpotLightShadowDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		inGameScene_->SpotLightShadowDraw();
		break;
	case GameScene::Scene::Editor:
		editorScene_->SpotLightShadowDraw();
		break;
	default:
		break;
	}
}


void GameScene::ParticleDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		whiteParticle_->Draw(color);
		break;
	default:
		break;
	}
}

void GameScene::ParticleBoxDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		dustParticle_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::PreSpriteDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		break;
	default:
		break;
	}
}

void GameScene::PostSpriteDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		ui_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::Draw(CommandContext& commandContext) {


	// 背景スプライト描画
	Sprite::PreDraw(&commandContext);
	PreSpriteDraw();
	Sprite::PostDraw();

	Renderer::GetInstance()->ClearMainDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(&commandContext, *currentViewProjection_, directionalLights_);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(&commandContext, *currentViewProjection_);
	ParticleDraw();
	Particle::PostDraw();

	//Particle描画
	ParticleBox::PreDraw(&commandContext, *currentViewProjection_);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

}

void GameScene::ShadowMapDraw(CommandContext& commandContext)
{
	ShadowMap::PreDraw(&commandContext, directionalLights_);
	ShadowDraw();
	ShadowMap::PostDraw();
}

void GameScene::SpotLightShadowMapDraw(CommandContext& commandContext)
{
	SpotLightShadowMap::PreDraw(&commandContext, shadowSpotLights_);
	SpotLightShadowDraw();
	SpotLightShadowMap::PostDraw();
}

void GameScene::UIDraw(CommandContext& commandContext)
{
	// 前景スプライト描画
	Sprite::PreDraw(&commandContext);
	PostSpriteDraw();
	Sprite::PostDraw();
}
