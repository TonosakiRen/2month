RWStructuredBuffer<uint> Output : register(u0);
Texture2D<float4> colorTex : register(t0);
SamplerState smp : register(s0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Output[DTid.x] = DTid.x;
}