#pragma once
#include "NormalEnemy.h"
#include "ShadowSpotLights.h"

class NormalLightEnemy : public NormalEnemy {
public:
	void SetLight(ShadowSpotLights::SpotLight shadowlight);

private:
	ShadowSpotLights::SpotLight shadowSpotLights_;


};