
#define MY_TEXTURE2D_SPACE space1

struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
	float32_t4 shadow : SV_TARGET1;
};

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b0);

struct DirectionLight {
	float32_t4 color;     
	float32_t3 direction; 
	float32_t intensity;
	float32_t4x4 viewProjection;
	uint32_t descriptorIndex;
};
StructuredBuffer<DirectionLight> gDirectionLights  : register(t3);

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<PointLight> gPointLights  : register(t4);

struct SpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
};
StructuredBuffer<SpotLight> gSpotLights  : register(t5);

struct ShadowSpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
	uint32_t descriptorIndex;
	float32_t4x4 viewProjection;
};
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t6);

struct LightNum {
	int32_t directionalLight;
	int32_t pointLight;
	int32_t spotLight;
	int32_t shadowSpotLight;
};

ConstantBuffer<LightNum> lightNum : register(b1);

float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
	// xは0~1から-1~1, yは0~1から1~-1に上下反転
	float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float4 position = float4(xy, depth, 1.0f);
	position = mul(position, viewProjectionInverseMatrix);
	position.xyz /= position.w;
	return position.xyz;
}

Texture2D<float4> colorTex : register(t0);
Texture2D<float4> normalTex : register(t1);
Texture2D<float4> depthTex : register(t2);

Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);

SamplerState smp : register(s0);

PixelShaderOutput main(VSOutput input)
{
	PixelShaderOutput output;

	output.shadow.xyzw = float32_t4(0.0f, 0.0f, 0.0f,0.0f);

	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;
	float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
	normal = normal * 2.0f - 1.0f;
	float32_t depth = depthTex.Sample(smp, input.uv).x;

	float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);

	float32_t3 lighting = {0.0f,0.0f,0.0f};

	float32_t3 shading = { 1.0f,1.0f,1.0f };

	//pointLight

	for (int i = 0; i < lightNum.pointLight;i++) {
		if (gPointLights[i].isActive) {
			float32_t3 pointLightDirection = normalize(worldPos - gPointLights[i].position);
			float32_t distance = length(gPointLights[i].position - worldPos);
			float32_t factor = pow(saturate(-distance / gPointLights[i].radius + 1.0), gPointLights[i].decay);


			//pointLightDiffuse
			float32_t NdotL = dot(normal, -pointLightDirection);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 pointLightdiffuse = gPointLights[i].color.xyz * cos * gPointLights[i].intensity * factor;

			//pointLightSpecular
			float32_t3 viewDirection = normalize(gPointLights[i].position - worldPos.xyz);
			float32_t3 reflectVec = reflect(pointLightDirection, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 pointLightSpecluer = gPointLights[i].color.xyz * gPointLights[i].intensity * specularPow * factor;

			lighting += (pointLightdiffuse + pointLightSpecluer);
		}
	}

	//spotLight
	for (int j = 0; j < lightNum.spotLight; j++) {
		if (gSpotLights[j].isActive) {
			float32_t3 spotLightDirectionOnSurface = normalize(worldPos - gSpotLights[j].position);
			float32_t distance = length(gSpotLights[j].position - worldPos);
			float32_t factor = pow(saturate(-distance / gSpotLights[j].distance + 1.0), gSpotLights[j].decay);
			float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLights[j].direction);
			float32_t falloffFactor = saturate((cosAngle - gSpotLights[j].cosAngle) / (1.0f - gSpotLights[j].cosAngle));


			//spotLightDiffuse
			float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 spotLightdiffuse = gSpotLights[j].color.xyz * cos * gSpotLights[j].intensity * factor * falloffFactor;


			//spotLightSpecular
			float32_t3 viewDirection = normalize(gSpotLights[j].position - worldPos.xyz);

			float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 spotLightSpecluer = gSpotLights[j].color.xyz * gSpotLights[j].intensity * specularPow * factor * falloffFactor;

			lighting += (spotLightdiffuse + spotLightSpecluer);
		}
	}

	//shadowSpotLight
	for (int l = 0; l < lightNum.shadowSpotLight; l++) {
		if (gShadowSpotLights[l].isActive) {
			float32_t3 spotLightDirectionOnSurface = normalize(worldPos - gShadowSpotLights[l].position);
			float32_t distance = length(gShadowSpotLights[l].position - worldPos);
			float32_t factor = pow(saturate(-distance / gShadowSpotLights[l].distance + 1.0), gShadowSpotLights[l].decay);
			float32_t cosAngle = dot(spotLightDirectionOnSurface, gShadowSpotLights[l].direction);
			float32_t falloffFactor = saturate((cosAngle - gShadowSpotLights[l].cosAngle) / (1.0f - gShadowSpotLights[l].cosAngle));


			//spotLightDiffuse
			float32_t NdotL = dot(normal, -spotLightDirectionOnSurface);
			float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
			float32_t3 spotLightdiffuse = gShadowSpotLights[l].color.xyz * cos * gShadowSpotLights[l].intensity * factor * falloffFactor;


			//spotLightSpecular
			float32_t3 viewDirection = normalize(gShadowSpotLights[l].position - worldPos.xyz);

			float32_t3 reflectVec = reflect(spotLightDirectionOnSurface, normal);
			float32_t specluerPower = 10.0f;
			float32_t RdotE = dot(reflectVec, viewDirection);
			float32_t specularPow = pow(saturate(RdotE), specluerPower);
			float32_t3 spotLightSpecluer = gShadowSpotLights[l].color.xyz * gShadowSpotLights[l].intensity * specularPow * factor * falloffFactor;

			lighting += (spotLightdiffuse + spotLightSpecluer);

			//影
			float32_t4 wp = float4(worldPos.xyz, 1.0f);
			float32_t4 lightViewPosition = mul(wp, gShadowSpotLights[l].viewProjection);
			float32_t2 shadowMapUV = lightViewPosition.xy / lightViewPosition.w;
			shadowMapUV *= float32_t2(0.5f, -0.5f);
			shadowMapUV += 0.5f;

			if (lightViewPosition.z > 0.0f) {
				float32_t zInLVP = lightViewPosition.z / lightViewPosition.w;

				if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
					&& shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
					) {
					float32_t zInShadowMap = Texture2DTable[gShadowSpotLights[l].descriptorIndex].Sample(smp, shadowMapUV).r;
					if (zInShadowMap != 1.0f) {
						if (zInLVP - 0.00001 > zInShadowMap) {
							shading *= 0.5f;
							output.shadow.x = 1.0f;
						}
					}
				}
			}
		}
	}


	for (int k = 0; k < lightNum.directionalLight; k++) {

		//directionalLightDiffuse
		float32_t NdotL = dot(normal, -gDirectionLights[k].direction);
		float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 directionalLightdiffuse = gDirectionLights[k].color.xyz * cos * gDirectionLights[k].intensity;

		//directionalLightSpecular
		float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos.xyz);

		float32_t3 reflectVec = reflect(gDirectionLights[k].direction, normal);
		float32_t specluerPower = 10.0f;
		float32_t RdotE = dot(reflectVec, viewDirection);
		float32_t specularPow = pow(saturate(RdotE), specluerPower);
		float32_t3 directionalLightSpecluer = gDirectionLights[k].color.xyz * gDirectionLights[k].intensity * specularPow;

		lighting += (directionalLightdiffuse + directionalLightSpecluer);

		//影
		float32_t4 wp = float4(worldPos.xyz, 1.0f);
		float32_t4 lightViewPosition = mul(wp, gDirectionLights[k].viewProjection);
		float32_t2 shadowMapUV = lightViewPosition.xy / lightViewPosition.w;
		shadowMapUV *= float32_t2(0.5f, -0.5f);
		shadowMapUV += 0.5f;

		float32_t zInLVP = lightViewPosition.z / lightViewPosition.w;

		if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
			&& shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
			) {
			float32_t zInShadowMap = Texture2DTable[gDirectionLights[k].descriptorIndex].Sample(smp, shadowMapUV).r;
			if (zInLVP - 0.00001 > zInShadowMap) {
				shading *= 0.5f;
			}
		}

		
	}

	//アンビエント
	float32_t3 ambient = float32_t3(0.0f, 0.0f, 0.0f);

	// フレネル
		//float32_t3 fresnelColor = float32_t3(1.0f, 0.0f, 0.0f);
		//float32_t power = 2.0f;
		//float32_t fresnel = pow((1.0f - saturate(dot(normal, viewDirection))), power);
		////output.color.xyz = lerp(1.0f - fresnel, output.color.xyz, fresnelColor);
		//output.color.xyz += fresnelColor * fresnel;

	lighting *= shading;

	lighting += ambient;
	
	color.xyz *= lighting;

	output.color.xyz = color.xyz;
	output.color.w = 1.0f;

	return output;
}