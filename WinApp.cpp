#include "WinApp.h"
#include <string>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "Helper.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
const wchar_t WinApp::kWindowClassName[] = L"DirectXGame";

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

//　ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::CreateGameWindow(const char* title) {
	// ウィンドウプロシージャ
	wndClass_.lpfnWndProc = WindowProc;
	// ウィンドウクラス名(何でもよい)
	wndClass_.lpszClassName = kWindowClassName;
	// インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//　ウィンドウクラスを登録する	
	RegisterClass(&wndClass_);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	windowRect_ = { 0,0,kWindowWidth,kWindowHeight };

	//クライアント領域をもとに実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&windowRect_, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウタイトルをwchar_tに変換
	std::wstring titleWString = Helper::ConvertString(title);

	hwnd_ = CreateWindow(
		wndClass_.lpszClassName,//利用するクラス名
		titleWString.c_str(),	//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//よく見るウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標
		CW_USEDEFAULT,			//表示Y座標
		windowRect_.right - windowRect_.left,	//ウィンドウ横幅
		windowRect_.bottom - windowRect_.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		wndClass_.hInstance,	//インスタンスハンドル
		nullptr					//オプション
	);

	//ウィンドウを表示する	
	ShowWindow(hwnd_, SW_SHOW);
}

bool WinApp::ProcessMessage() {
	MSG msg{};//メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // メッセージがある？
	{
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
	{
		return true;
	}

	return false;
}

void WinApp::TerminateGameWindow() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);
}