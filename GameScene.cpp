#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "CommandContext.h"
#include "Renderer.h"
#include "ModelManager.h"

void (GameScene::* GameScene::SceneUpdateTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::InGameUpdate,
};

void (GameScene::* GameScene::SceneInitializeTable[])() = {
	&GameScene::TitleInitialize,
	&GameScene::InGameInitialize,
};


GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	Vector3 initializeCameraPos = { 0.0f,8.45f,-26.0f };
	Vector3 initializeCameraRotate = { 0.21f,0.0f,0.0f };

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(initializeCameraPos, initializeCameraRotate);


	camera_ = std::make_unique<Camera>();
	camera_->Initialize(initializeCameraPos, MakeFromEulerAngle(initializeCameraRotate));

	currentViewProjection_ = debugCamera_.get();

	directionalLights_.Initialize();
	directionalLights_.Update();


	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(textureHandle_, { 0.0f,0.0f });

	player_ = std::make_unique<Player>();
	player_->Initialize("GoalWell");

	walls_.resize(3);
	uint32_t wallModelHandle_ = ModelManager::Load("wall");
	float sizeX = ModelManager::GetInstance()->ModelManager::GetModelSize(wallModelHandle_).x;
	walls_[0] = std::make_unique<Wall>();
	walls_[0]->Initialize({ -sizeX ,-2.0f,0.0f});
	walls_[1] = std::make_unique<Wall>();
	walls_[1]->Initialize({ 0.0f,-2.0f,0.0f });
	walls_[2] = std::make_unique<Wall>();
	walls_[2]->Initialize({ +sizeX,-2.0f,0.0f });
	
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome");
	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor");
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

	compute_ = std::make_unique<Compute>();
	compute_->Initialize();
}

void GameScene::Update(CommandContext& commandContext){
	Collider::SwitchIsDrawCollider();
	ViewProjection::SwitchIsUseDebugCamera();
	//fps表示
#ifdef _DEBUG
	ImGui::Begin("fps");
	auto io = ImGui::GetIO();
	ImGui::Text("%f", io.Framerate);
	ImGui::End();
#endif
	//camera light
	{
		if (ViewProjection::isUseDebugCamera) {
			currentViewProjection_ = debugCamera_.get();
			debugCamera_->Update();
		}
		else {
			currentViewProjection_ = camera_.get();
			camera_->Update(player_->GetWorldTransform()->translation_.x);
		}
		
		// light
#ifdef _DEBUG
		ImGui::Begin("light");
		ImGui::DragFloat3("lightDirection", &directionalLights_.lights_[0].direction.x, 0.01f);
		ImGui::DragFloat4("lightColor", &directionalLights_.lights_[0].color.x, 1.0f,0.0f,255.0f);
		ImGui::End();
#endif
		directionalLights_.lights_[0].direction = Normalize(directionalLights_.lights_[0].direction);
		directionalLights_.Update();
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

	//コンピュートシェーダテスト
	{
		compute_->Dispatch(commandContext);
		uint32_t* date = static_cast<uint32_t*>(compute_->GetData());

		int a = date[1];
		ImGui::Text("%d", int(a));
	}
	
	//パーティクル
	ImGui::Begin("particle");
	ImGui::DragFloat4("particleColor", &color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("emitterPos", &whiteParticle_->emitterWorldTransform_.translation_.x, 0.01f);
	ImGui::End();
	whiteParticle_->Update();
}

void GameScene::TitleInitialize() {

}
void GameScene::TitleUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::InGame;
	}

}
void GameScene::InGameInitialize() {

}
void GameScene::InGameUpdate() {

	if (input_->TriggerKey(DIK_P)) {
		sceneRequest_ = Scene::Title;
	}

	skydome_->Update();

	for (auto& wall : walls_) {
		wall->Update();
	}

	player_->Update();

	for (auto& wall : walls_) {
		player_->Collision(wall->collider_);
	}
}

void GameScene::ModelDraw()
{
	switch (scene_)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::InGame:
		skydome_->Draw();
		for (auto& wall : walls_) {
			wall->Draw();
		}
		player_->Draw();
		sphere_->Draw();
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
		sprite_->Draw();
		break;
	default:
		break;
	}
}

void GameScene::Draw(CommandContext& commandContext) {

	// 背景スプライト描画
	Sprite::PreDraw(commandContext);
	PreSpriteDraw();
	Sprite::PostDraw();

	Renderer::GetInstance()->ClearMainDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(commandContext, *currentViewProjection_, directionalLights_);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(commandContext, *currentViewProjection_);
	ParticleDraw();
	Particle::PostDraw();

	//Particle描画
	ParticleBox::PreDraw(commandContext, *currentViewProjection_, directionalLights_);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

}

void GameScene::UIDraw(CommandContext& commandContext)
{
	// 前景スプライト描画
	Sprite::PreDraw(commandContext);
	PostSpriteDraw();
	Sprite::PostDraw();
}
