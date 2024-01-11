#include "LightNumBuffer.h"
#include "DirectionalLights.h"
#include "PointLights.h"
#include "SpotLights.h"

void LightNumBuffer::Create()
{
	lightNumBuffer_.Create(sizeof(LightNum));
	LightNum lightNum{ DirectionalLights::lightNum,PointLights::lightNum,SpotLights::lightNum };
	lightNumBuffer_.Copy(lightNum);
}
