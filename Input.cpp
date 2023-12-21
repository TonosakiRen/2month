#include "Input.h"
#include "Helper.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment (lib, "xinput.lib")

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	Helper::AssertIfFailed(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dInput_, nullptr));

	//keyBoard
	Helper::AssertIfFailed(dInput_->CreateDevice(GUID_SysKeyboard, &devKeyboard_, nullptr));
	Helper::AssertIfFailed(devKeyboard_->SetDataFormat(&c_dfDIKeyboard));
	Helper::AssertIfFailed(devKeyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY));

	//mouse
	Helper::AssertIfFailed(dInput_->CreateDevice(GUID_SysMouse, &devMouse_, nullptr));
	Helper::AssertIfFailed(devMouse_->SetDataFormat(&c_dfDIMouse));
	Helper::AssertIfFailed(devMouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY));
}

void Input::Update() {
	devKeyboard_->Acquire();
	devMouse_->Acquire();

	keyPre_ = key_;
	preMouseState_ = mouseState_;

	devKeyboard_->GetDeviceState((DWORD)size(key_), key_.data());
	devMouse_->GetDeviceState(sizeof(mouseState_.state), &mouseState_.state);

	preXInputState_ = xInputState_;
	if (XInputGetState(0, &xInputState_) == ERROR_SUCCESS) {
		isGamePadConnect = true;
	}
	else {
		isGamePadConnect = false;
	}
}

bool Input::PushKey(BYTE keyNumber) {

	if (key_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {

	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::ReleaseKey(BYTE keyNumber) {

	if (keyPre_[keyNumber] && !key_[keyNumber]) {
		return true;
	}

	return false;
}

Vector2 Input::GetMouseMove() {
	return { static_cast<float>(mouseState_.state.lX) ,static_cast<float>(mouseState_.state.lY) };
}

float Input::GetWheel() {
	return static_cast<float>(mouseState_.state.lZ);
}

bool Input::IsPressMouse(int32_t mouseNumber) {
	return mouseState_.state.rgbButtons[mouseNumber] & 0x80;
}