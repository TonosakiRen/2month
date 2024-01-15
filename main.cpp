#include <Windows.h>
#include "WinApp.h"
#include "TextureManager.h"
#include "Input.h"
#include "Audio.h"
#include "GameScene.h"
#include "Particle.h"
#include "ParticleBox.h"
#include "PostEffect.h"
#include "GaussianBlur.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Compute.h"
#include "ShadowMap.h"
#include "SpotLightShadowMap.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	WinApp* win = nullptr;
	Renderer* renderer = nullptr;
	//汎用機能
	GameScene* gameScene = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	renderer = Renderer::GetInstance();
	renderer->Initialize();


	// 汎用機能
#pragma region 汎用機能初期化
	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize();
	TextureManager::Load("white1x1.png");

	// 3Dオブジェクト静的初期化
	Model::StaticInitialize();

	ShadowMap::StaticInitialize();

	SpotLightShadowMap::StaticInitialize();

	// 3Dオブジェクト静的初期化
	Particle::StaticInitialize();
	ParticleBox::StaticInitialize();

	GaussianBlur::StaticInitialize();

	//　スプライト静的初期化
	Sprite::StaticInitialize();

	Compute::StaticInitialize();


#pragma endregion 変数

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		renderer->BeginFrame();

		// 入力関連の毎フレーム処理
		input->Update();
		audio->Update();

		// ゲームシーンの毎フレーム処理
		gameScene->Update(renderer->GetCommandContext());

		// 描画開始
		renderer->BeginShadowMapRender(gameScene->GetDirectionalLights());

		gameScene->ShadowMapDraw(renderer->GetCommandContext());

		renderer->EndShadowMapRender(gameScene->GetDirectionalLights());

		renderer->BeginSpotLightShadowMapRender(gameScene->GetShadowSpotLights());

		gameScene->SpotLightShadowMapDraw(renderer->GetCommandContext());

		renderer->EndSpotLightShadowMapRender(gameScene->GetShadowSpotLights());
		
		renderer->BeginMainRender();

		// ゲームシーンの描画
		gameScene->Draw(renderer->GetCommandContext());
		
		renderer->DeferredRender(gameScene->GetViewProjection(), gameScene->GetDirectionalLights(),gameScene->GetPointLights(), gameScene->GetSpotLights(), gameScene->GetShadowSpotLights());

		renderer->EndMainRender();

		renderer->BeginUIRender();

		gameScene->UIDraw(renderer->GetCommandContext());

		renderer->EndUIRender();

	}


	// ImGui解放
	renderer->Shutdown();
	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}