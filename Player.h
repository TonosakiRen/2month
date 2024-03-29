#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Audio.h"
#include "Collider.h"
#include "Sprite.h"
#include "ShadowHitParticle.h"
#include "HitParticle.h"
#include "Particle.h"
#include "DeadParticle.h"

// enemyなどに送るための情報
struct PlayerDate {
    Vector3 position_; // 現在地
    uint32_t damage_ = 0u; // ダメージ量
    bool isAttack_ = false; // 攻撃中か
    uint32_t id = 0u; //
    int attackIndex = -1;
};

class GlobalVariables;

class Player : public GameObject {
public:

    enum HitReaction {
        knockBack,
        damage,
        heal,
        coin
    };

    //そのフレームの当たった敵のインデックス　-1は当たっていない
    static int32_t hitShadowEnemyIndex_ ;
    static Vector3 hitShadowEnemyPos_;
    static Collider* hitCollider_;
    static Vector3 playerPos_;
    static HitReaction hitReaction_;


    void Initialize(const std::string name);
    void SetGlobalVariable();
    void ApplyGlobalVariable();
    void Update();
    void UIUpdate();
    void EnemyShadowCollision();
    void EnemyCollision();
    void Draw();
    void DrawImGui();
    void DrawUI();
    void DrawParticle();
    void DrawParticleBox();

    void CollisionProcess(const Vector3& pushBackVector);
private:
    void Move();
    void Jump();
    void Attack();
    void MoveLimit();
    void InsertData();
    void UpdateTrans();

    void DeadUpdate();
    void ClearUpdate();

public:
    Collider bodyCollider_;
    int MUTEKITime_ = 120;
private:
    WorldTransform bodyWorldTransform_;
    Vector3 bodyModelSize_;
    Vector3 headModelSize_;
    float modelHeight_;
    struct JumpParameter {
        Vector3 velocity_;
        Vector3 acceleration_;
        bool isJumped_ = false; // jump中か
    };
    JumpParameter jumpParam_;
    Vector3 pushBackVector_;

    Input* input_ = nullptr;
    Audio* audio_ = nullptr;
    GlobalVariables* globalVariables_ = nullptr;
    
    uint32_t headModelHandle_;
    WorldTransform headWorldTransform_;
    Vector3 headRotate;
    Vector3 rotate;
    bool isKnockBack_ = false;

    struct AttackParameter {
        int phase = 0;
        bool isAttacked = false; // 攻撃中か
        uint32_t damage_ = 0u;
        uint32_t id_; //
    };
    AttackParameter attackParam_;
    float easing_t = 0;
    float easing_tBack = 0;

    //調整項目
    float jumpPower_ = 0.5f;
    float speed_ = 0.3f;
    float attackSpeed_ = 2.0f;
    float attackCoolTime_ = 0.0f;
    float kmaxattackCoolTime_ = 2.0f;
    float backHeadSpeed_ = 0.3f;
    float attackReadySpeed_ = 0.3f;
    int maxHp_ = 100;
    int hp_ = 100;
    int damage_ = 5;
    int maxMUTEKITime_ = 120;
    int heal_ = 30;
    int blinkingTime_ = 6;
    float dashSpeed_ = 0.6f;

    bool isBlink_ = false;
    bool isDash_ = false;

    float knockBackPowerX_ = 0.5f;
    float knockBackPowerY_ = 0.8f;
    Vector3 knockBackDirection_ = { 0.0f,0.0f,0.0f };

    //UI
    Sprite hpSprite_;
    Sprite HpbarSprite_;

    Sprite coinUI_;

    Sprite one_;
    Sprite ten_;

    ShadowHitParticle shadowHitParticle_;
    HitParticle hitParticle_;


    Quaternion inputQuaternion_;

    int coinNum_ = 0;

public:
    Collider headCollider_;

    bool isDead_ = false;

    bool isEndDeadAnimation_ = false;

    int deadFrame_ = 0;

    DeadParticle deadParticle_;

    uint32_t deadParticleHandle_ = 0;

    float deadT = 0.0f;

    bool isClear_ = false;

    bool isEndClearAnimation = false;

    GameObject ty_;

    float tySaveY_ = 0.0f;
private:

public:
    PlayerDate date_;

    Vector3 rotate_{};

};
