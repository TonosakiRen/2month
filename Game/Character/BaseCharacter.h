#pragma once
#include "GameObject.h"
#include "Collider.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include <vector>
#include <string>

struct PlayerDate;

class BaseCharacter {
public:
	static uint32_t masterCount;
	BaseCharacter();
	~BaseCharacter();
public:
	virtual void Initialize(const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f), const Quaternion& quaternion = Quaternion(0.0f, 0.0f, 0.0f, 1.0f), const Vector3& translate = Vector3(0.0f, 0.0f, 0.0f)) {};
	virtual void Update(const Vector3& playerPosition) {};
	virtual void OnCollision(Collider& collider, const PlayerDate& date) {}; // 衝突判定とその後の処理
	virtual void Draw() {};
	virtual void EnemyDraw() {};
	virtual void DrawImGui();

	void PushBackCollision(Collider& collider); // 押し戻し処理

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position, worldTransform_.Update(); }

protected:
	// 回転用
	Vector3 rotate;

	void BaseInitialize(const uint32_t& modelNumber, std::vector<std::string> names);
	void UpdateTransform();
	void BaseDraw();
	void BaseEnemyDraw();
	void BaseCoinDraw();
	void BaseHeartDraw();

protected:
	WorldTransform worldTransform_;
	std::vector<GameObject> models_; // 複数モデル用
	std::vector<WorldTransform> modelsTransform_;
	const uint32_t kNumber_;
	bool isAlive_ = true; // 生きているか
	bool isActive_ = true; // 起動しているか

public:
	bool shadowOnly_ = false; // 影だけ描画するか
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Collider collider_;
	const WorldTransform* GetWorldTransform() { return &worldTransform_; }
	const bool& GetIsAlive() const { return isAlive_; }

};

const float kMaxDistance = 70.0f; // playerとの距離が一定以上なら処理されない