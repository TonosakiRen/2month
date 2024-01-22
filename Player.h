#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Audio.h"
#include "Collider.h"


// enemyなどに送るための情報
struct PlayerDate {
    Vector3 position_; // 現在地
    uint32_t damage_ = 0u; // ダメージ量
    bool isAttack_ = false; // 攻撃中か
    uint32_t id = 0u; //
};

class Player : public GameObject {
public:

    void Initialize(const std::string name);
    void Update();
    void Collision(Collider& otherCollider);
    void Draw();
    void DrawImGui();
private:
    void Move();
    void Jump();
    void Attack();
    void MoveLimit();
    void InsertData();
    void UpdateTrans();

public:
    Collider bodyCollider_;
private:
    WorldTransform bodyWorldTransform_;
    Vector3 bodyModelSize_;
    Vector3 headModelSize_;
    float modelHeight_;
    struct JumpParameter {
        Vector3 velocisity_;
        Vector3 acceleration_;
        bool isJumped_ = false; // jump中か
    };
    JumpParameter jumpParam_;

    Input* input_ = nullptr;
    Audio* audio_ = nullptr;
    
    uint32_t headModelHandle_;
    WorldTransform headWorldTransform_;
    Vector3 headRotate;
    Vector3 rotate;

    struct AttackParameter {
        int phase = 0;
        bool isAttacked = false; // 攻撃中か
        uint32_t damage_ = 0u;
        uint32_t id_; //
    };
    AttackParameter attackParam_;

public:
    Collider headCollider_;
private:

public:
    PlayerDate date_;

};
