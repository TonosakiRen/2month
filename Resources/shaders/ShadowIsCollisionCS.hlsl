RWStructuredBuffer<uint> Output : register(u0);
Texture2D<float4> colorTex : register(t0);
Texture2D<float4> indexTex : register(t1);
struct Index {
	float32_t2 data;
};
ConstantBuffer<Index> index : register(b0);
struct Luminance {
	float32_t value;
};
ConstantBuffer<Luminance> collisionLuminance : register(b1);
SamplerState smp : register(s0);

[numthreads(30, 30, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
	if (DTid.x == 0 && DTid.y == 0) {
		uint v;
		InterlockedExchange(Output[0], 1, v);
	}

	float32_t2 indexData = indexTex[DTid.xy].xy;
	if (indexData.x == 1.0f) {
		if (indexData.y == index.data.x) {
			float32_t3 colorData = colorTex[DTid.xy].xyz;
			float32_t luminance = colorData.x * 0.299f + colorData.y * 0.587f + colorData.z * 0.114f;
			if (luminance > collisionLuminance.value) {
				uint v;
				InterlockedExchange(Output[0], 0, v);
			}
		}
	}
}