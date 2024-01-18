#pragma once
#include "GameObject.h"
#include "Collider.h"
#include "ImGuiManager.h"
#include "UploadBuffer.h"

struct PlayerDate;

class BaseCharacter : public GameObject {
public:
	virtual void Initialize(const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f), const Quaternion& quaternion = Quaternion(0.0f, 0.0f, 0.0f, 1.0f), const Vector3& translate = Vector3(0.0f, 0.0f, 0.0f)) {};
	virtual void Update(const Vector3& playerPosition) {};
	virtual void OnCollision(Collider& collider, const PlayerDate& date) {}; // 衝突判定とその後の処理
	virtual void Draw() {};
	virtual void DrawImGui();

	Collider collider_;
	UploadBuffer buffer_;

protected:
	// bufferの作成
	void CreateInitialize();
	// 回転用
	Vector3 rotate;

};