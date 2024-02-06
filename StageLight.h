#pragma once
#include "GameObject.h"
#include "Collider.h"
class ShadowSpotLights;
class StageLight :
    public GameObject
{
public:

    ~StageLight();

    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate ,ShadowSpotLights* shadowSpotLights,float distance, float shadeDistance, float intensity, float decay);
    void Update(const Vector3& playerWorldPosition);
    void Draw();
    void DrawImGui();

public:
    Collider collider_;
    Vector3 rotate;
    int lightIndex_ = -1;
    float distance_ = 20.0f;
    float shadeDistance_ = 5.0f;
    float intensity_ = 5.5f;
    float decay_ = 1.0f;

private:
    ShadowSpotLights* shadowSpotLights_;
    bool ActiveChack(const Vector3& playerWorldPosition) const;
    bool isActive_ = true;

};
