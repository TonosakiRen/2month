RWStructuredBuffer<uint> Output : register(u0);
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Output[DTid.x] = DTid.x;
}