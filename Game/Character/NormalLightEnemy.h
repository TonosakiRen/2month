#pragma once
#include "NormalEnemy.h"

class ShadowSpotLights;

class NormalLightEnemy : public NormalEnemy {
public:
	~NormalLightEnemy();
	void SetLight(ShadowSpotLights* shadowlight, const uint32_t& num);

	void Update(const Vector3& playerPosition) override;

private:
	ShadowSpotLights* shadowSpotLights_ = nullptr;
	void MoveLight();

	uint32_t number_ = 0u; // ライト所持number
};