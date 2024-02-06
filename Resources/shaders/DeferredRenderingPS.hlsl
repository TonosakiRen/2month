
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
	uint32_t shadowDescriptorIndex;
};
StructuredBuffer<DirectionLight> gDirectionLights  : register(t4);

struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
	float32_t isActive;
};
StructuredBuffer<PointLight> gPointLights  : register(t5);

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
StructuredBuffer<SpotLight> gSpotLights  : register(t6);

struct ShadowSpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
	float32_t isActive;
	uint32_t shadowDescriptorIndex;
	uint32_t collisionDescriptorIndex;
	uint32_t padding;
	uint32_t padding1;
	uint32_t padding2;
	float32_t4x4 viewProjection;
};
StructuredBuffer<ShadowSpotLight> gShadowSpotLights  : register(t7);

struct LightNum {
	int32_t directionalLight;
	int32_t pointLight;
	int32_t spotLight;
	int32_t shadowSpotLight;
};

ConstantBuffer<LightNum> lightNum : register(b1);

struct Shade {
	float value;
};

ConstantBuffer<Shade> shade : register(b2);

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
Texture2D<float4> nonCharacterDepthTex : register(t3);
Texture2D<float4> materialTex : register(t8);

Texture2D<float4> Texture2DTable[]  : register(t0, MY_TEXTURE2D_SPACE);

SamplerState smp : register(s0);
SamplerState pointSmp : register(s1);


PixelShaderOutput main(VSOutput input)
{
	PixelShaderOutput output;

	output.shadow.xyzw = float32_t4(0.0f, 0.0f, 0.0f,0.0f);

	float32_t enableLighting = materialTex.Sample(smp, input.uv).x;
	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;

	if (enableLighting) {
		
		float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
		normal = normal * 2.0f - 1.0f;
		float32_t depth = depthTex.Sample(smp, input.uv).x;
		float32_t nonCharacterDepth = nonCharacterDepthTex.Sample(smp, input.uv).x;

		float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);
		float32_t3 nonCharacterWorldPos = GetWorldPosition(input.uv, nonCharacterDepth, gViewProjection.inverseViewProjection);

		float32_t3 lighting = { 0.0f,0.0f,0.0f };

		float32_t3 shading = { 1.0f,1.0f,1.0f };


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


				float32_t4 nonWp = float4(nonCharacterWorldPos.xyz, 1.0f);
				float32_t4 nonLightViewPosition = mul(nonWp, gShadowSpotLights[l].viewProjection);

				if (nonLightViewPosition.z > 0.0f) {

					float32_t2 nonShadowMapUV = nonLightViewPosition.xy / nonLightViewPosition.w;
					nonShadowMapUV *= float32_t2(0.5f, -0.5f);
					nonShadowMapUV += 0.5f;

					if (nonShadowMapUV.x > 0.0f && nonShadowMapUV.x < 1.0f
						&& nonShadowMapUV.y > 0.0f && nonShadowMapUV.y < 1.0f
						) {

						float32_t nonZInShadowMap = Texture2DTable[gShadowSpotLights[l].shadowDescriptorIndex].Sample(pointSmp, nonShadowMapUV).r;

						if (nonZInShadowMap != 1.0f) {

							float32_t nonZInLVP = nonLightViewPosition.z / nonLightViewPosition.w;

							if (nonZInLVP > nonZInShadowMap && nonZInLVP < 1.0f) {
								//キャラクターに影を落とさない処理
								float32_t4 enemyIndex = Texture2DTable[gShadowSpotLights[l].collisionDescriptorIndex].Sample(pointSmp, nonShadowMapUV);

								if (nonWp.z <= wp.z) {

									if (enemyIndex.x == 4.0f) {
										//♡
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;
										output.shadow.w = 2.0f;

										color.xyz = float32_t3(0.5f, 1.0f, 0.5f);
									}
									else if (enemyIndex.x == 3.0f) {
										//コイン
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;
										output.shadow.w = 1.0f;

										color.xyz = float32_t3(1.0f, 1.0f, 0.0f);
									}
									else if (enemyIndex.x == 2.0f) {
										//敵
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;

										color.xyz = float32_t3(1.0f, 0.2f, 0.4f);
										

									}
									else if (enemyIndex.x == 1.0f) {
										//player
										output.shadow.y = 1.0f;

										//color.xyz = float32_t3(1.0f, 0.0f, 0.0f);
										
									}
					
									shading *= shade.value;
									output.shadow.x = 1.0f;

								}
								else {

									if (enemyIndex.x == 4.0f) {
										//♡
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;
										output.shadow.w = 2.0f;
									}
									else if (enemyIndex.x == 3.0f) {
										//コイン
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;
										output.shadow.w = 1.0f;
									}
									else if (enemyIndex.x == 2.0f) {
										output.shadow.y = 2.0f;
										output.shadow.z = enemyIndex.y;
									}
									else if (enemyIndex.x == 1.0f) {
										output.shadow.y = 1.0f;
									}

								}


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
				float32_t zInShadowMap = Texture2DTable[gDirectionLights[k].shadowDescriptorIndex].Sample(smp, shadowMapUV).r;
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

		color.xyz *= shading;

		//pointLight

		for (int i = 0; i < lightNum.pointLight; i++) {
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


		lighting += ambient;

		color.xyz *= lighting;

	}

	
	output.color.xyz = color.xyz;
	output.color.w = 1.0f;

	return output;
}