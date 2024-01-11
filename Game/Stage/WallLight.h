#pragma once
#include "GameObject.h"
class WallLight : public GameObject {
public:
    void Initialize(Vector3 scale, Quaternion quaternion, Vector3 translate);
    void Update();
    void Draw();
    void DrawImGui();
    Vector3& GetLightPos() const;

private:
    Vector3 rotate;

};