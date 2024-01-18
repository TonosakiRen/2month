#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Audio.h"
#include "Collider.h"
class Player :
    public GameObject
{
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

public:
    Collider collider_;
private:
    WorldTransform modelWorldTransform_;
    Vector3 modelSize_;
    struct JumpParameter {
        Vector3 velocisity_;
        Vector3 acceleration_;
        bool isJumped_ = false; // jump中か
    };
    JumpParameter jumpParam_;

    Input* input_ = nullptr;
    Audio* audio_ = nullptr;
    
    GameObject HeadModel_;
    WorldTransform headModelTransform_;
    Vector3 headRotate;
    Vector3 rotate;

    struct AttackParameter {
        int phase;
        bool isAttacked = false; // 攻撃中か
        
    };
    AttackParameter attackParam_;

public:
    Collider headCollider_;
private:

};