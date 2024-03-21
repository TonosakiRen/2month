#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
public:
	static const int kWindowWidth = 1920;
	static const int kWindowHeight = 1080;
	static const wchar_t kWindowClassName[];

	RECT windowRect_;
	HWND hwnd_ = nullptr;
	WNDCLASS wndClass_{};

public:
	static WinApp* GetInstance();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void CreateGameWindow(const char* title = "DirextXGame");
	bool ProcessMessage();
	void TerminateGameWindow();
	
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }
private: 
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;
};

