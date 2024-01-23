#include "NormalLightEnemy.h"

void NormalLightEnemy::SetLight(ShadowSpotLights::SpotLight shadowlight) {
	shadowSpotLights_ = shadowlight;
	shadowSpotLights_.worldTransform = worldTransform_;

}
