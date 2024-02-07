#include <Windows.h>
#include "WinApp.h"
#include "TextureManager.h"
#include "ModelManager.h"
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
#include "GlobalVariables.h"
#include "./DepthRenderer.h"

#include <d3d12.h>
#include <dxgidebug.h>
#include <dxgi1_3.h>

#pragma comment(lib,"dxguid.lib")

struct ResourceLeakChecker {
	~ResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		}
	}
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	ResourceLeakChecker leakCheck;
	WinApp* win = nullptr;
	Renderer* renderer = nullptr;
	//汎用機能
	GameScene* gameScene = nullptr;
	GlobalVariables* globalVariables = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow("2309_シャドォーン");

	renderer = Renderer::GetInstance();
	renderer->Initialize();


	// 汎用機能
#pragma region 汎用機能初期化
	input = Input::GetInstance();
	input->Initialize(win->GetHInstance(), win->GetHwnd());

	audio = Audio::GetInstance();
	audio->Initialize();

	globalVariables = GlobalVariables::GetInstance();

	// テクスチャマネージャの初期化
	TextureManager* textureManager = TextureManager::GetInstance();
	textureManager->Initialize();
	textureManager->Load("white1x1.png");

	ModelManager* modelManager = ModelManager::GetInstance();

	// 3Dオブジェクト静的初期化
	Model::StaticInitialize();
	
	ShadowMap::StaticInitialize();
	SpotLightShadowMap::StaticInitialize();
	Particle::StaticInitialize();
	ParticleBox::StaticInitialize();
	Sprite::StaticInitialize();
	DepthRenderer::StaticInitialize();

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
		globalVariables->Update();

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

		renderer->EndMainRender(gameScene->GetViewProjection());

		renderer->BeginUIRender();

		gameScene->UIDraw(renderer->GetCommandContext());

		renderer->EndUIRender();

	}


	Model::Finalize();
	ShadowMap::Finalize();
	modelManager->Finalize();
	textureManager->Finalize();
	SpotLightShadowMap::Finalize();
	Particle::Finalize();
	ParticleBox::Finalize();
	Sprite::Finalize();
	DepthRenderer::Finalize();

	delete gameScene;
	// ImGui解放
	renderer->Shutdown();
	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}