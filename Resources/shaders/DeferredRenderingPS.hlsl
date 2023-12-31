
struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2  uv : TEXCOORD;
};

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
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
};
ConstantBuffer<DirectionLight> gDirectionLight  : register(b1);


struct PointLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t radius;
	float32_t decay;
};
StructuredBuffer<PointLight> gPointLights  : register(t3);

struct SpotLight {
	float32_t4 color;
	float32_t3 position;
	float32_t intensity;
	float32_t3 direction;
	float32_t distance;
	float32_t decay;
	float32_t cosAngle;
};
StructuredBuffer<SpotLight> gSpotLights  : register(t4);

struct LightNum {
	int32_t directionalLight;
	int32_t pointLight;
	int32_t spotLight;
};

ConstantBuffer<LightNum> lightNum : register(b2);

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

SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	PixelShaderOutput output;

	float32_t3 color = colorTex.Sample(smp, input.uv).xyz;
	float32_t3 normal = normalTex.Sample(smp, input.uv).xyz;
	normal = normal * 2.0f - 1.0f;
	float32_t depth = depthTex.Sample(smp, input.uv).x;

	float32_t3 worldPos = GetWorldPosition(input.uv, depth, gViewProjection.inverseViewProjection);

	float32_t3 lighting = {0.0f,0.0f,0.0f};

	//directionalLightDiffuse
	float32_t NdotL = dot(normal, -gDirectionLight.direction);
	float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
	float32_t3 directionalLightdiffuse = gDirectionLight.color.xyz * cos * gDirectionLight.intensity;

	//directionalLightSpecular
	float32_t3 viewDirection = normalize(gViewProjection.viewPosition - worldPos.xyz);

	float32_t3 reflectVec = reflect(gDirectionLight.direction, normal);
	float32_t specluerPower = 10.0f;
	float32_t RdotE = dot(reflectVec, viewDirection);
	float32_t specularPow = pow(saturate(RdotE), specluerPower);
	float32_t3 directionalLightSpecluer = gDirectionLight.color.xyz * gDirectionLight.intensity * specularPow;

	lighting += (directionalLightdiffuse + directionalLightSpecluer);

	//pointLight

	for (int i = 0; i < lightNum.pointLight;i++) {
		float32_t3 pointLightDirection = normalize(worldPos - gPointLights[i].position);
		float32_t distance = length(gPointLights[i].position - worldPos);
		float32_t factor = pow(saturate(-distance / gPointLights[i].radius + 1.0), gPointLights[i].decay);


		//pointLightDiffuse
		NdotL = dot(normal, -pointLightDirection);
		cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 pointLightdiffuse = gPointLights[i].color.xyz * cos * gPointLights[i].intensity * factor;

		//pointLightSpecular
		viewDirection = normalize(gPointLights[i].position - worldPos.xyz);
		reflectVec = reflect(pointLightDirection, normal);
		specluerPower = 10.0f;
		RdotE = dot(reflectVec, viewDirection);
		specularPow = pow(saturate(RdotE), specluerPower);
		float32_t3 pointLightSpecluer = gPointLights[i].color.xyz * gPointLights[i].intensity * specularPow * factor;

		lighting += (pointLightdiffuse + pointLightSpecluer);
	}

	//spotLight
	for (int j = 0; j < lightNum.spotLight; j++) {

		float32_t3 spotLightDirectionOnSurface = normalize(worldPos - gSpotLights[j].position);
		float32_t distance = length(gSpotLights[j].position - worldPos);
		float32_t factor = pow(saturate(-distance / gSpotLights[j].distance + 1.0), gSpotLights[j].decay);
		float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLights[j].direction);
		float32_t falloffFactor = saturate((cosAngle - gSpotLights[j].cosAngle) / (1.0f - gSpotLights[j].cosAngle));


		//spotLightDiffuse
		NdotL = dot(normal, -spotLightDirectionOnSurface);
		cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		float32_t3 spotLightdiffuse = gSpotLights[j].color.xyz * cos * gSpotLights[j].intensity * factor * falloffFactor;


		//spotLightSpecular
		viewDirection = normalize(gSpotLights[j].position - worldPos.xyz);

		reflectVec = reflect(spotLightDirectionOnSurface, normal);
		specluerPower = 10.0f;
		RdotE = dot(reflectVec, viewDirection);
		specularPow = pow(saturate(RdotE), specluerPower);
		float32_t3 spotLightSpecluer = gSpotLights[j].color.xyz * gSpotLights[j].intensity * specularPow * factor * falloffFactor;

		lighting += (spotLightdiffuse + spotLightSpecluer);
	}


	//アンビエント
	float32_t3 ambient = float32_t3(0.0f, 0.0f, 0.0f);

	// フレネル
		//float32_t3 fresnelColor = float32_t3(1.0f, 0.0f, 0.0f);
		//float32_t power = 2.0f;
		//float32_t fresnel = pow((1.0f - saturate(dot(normal, viewDirection))), power);
		////output.color.xyz = lerp(1.0f - fresnel, output.color.xyz, fresnelColor);
		//output.color.xyz += fresnelColor * fresnel;

	lighting += ambient;
	
	color.xyz *= lighting;

	output.color.xyz = color.xyz;
	output.color.w = 1.0f;

	return output.color;
}