RWStructuredBuffer<uint> Output : register(u0);
Texture2D<float4> colorTex : register(t0);
SamplerState smp : register(s0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float32_t2 texSize;
	//テクスチャーのサイズ
	colorTex.GetDimensions(texSize.x, texSize.y);

	uint v;
	InterlockedExchange(Output[0], 1, v);


}