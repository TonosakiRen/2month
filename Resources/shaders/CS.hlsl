RWStructuredBuffer<uint> Output : register(u0);
Texture2D<float4> colorTex : register(t0);
SamplerState smp : register(s0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
	if (DTid.x == 0 && DTid.y == 0) {
		uint v;
		InterlockedExchange(Output[0], 0, v);
	}

	float32_t2 texSize;
	//テクスチャーのサイズ
	colorTex.GetDimensions(texSize.x, texSize.y);
	float32_t2 pixelSize = 1.0f / texSize;

	float32_t2 centerData = colorTex[DTid.xy].xy;

	float32_t2 topData = float32_t2(0.0f, 0.0f);
	float32_t2 bottomData = float32_t2(0.0f, 0.0f);
	float32_t2 leftData = float32_t2(0.0f, 0.0f);
	float32_t2 rightData = float32_t2(0.0f, 0.0f);

	uint32_t2 topIndex = uint32_t2(DTid.x, DTid.y - 1.0f);
	uint32_t2 bottomIndex = uint32_t2(DTid.x, DTid.y + 1.0f);
	uint32_t2 leftIndex = uint32_t2(DTid.x - 1.0f, DTid.y);
	uint32_t2 rightIndex = uint32_t2(DTid.x + 1.0f, DTid.y);

	if (DTid.x == 0) {
		leftIndex = DTid.xy;
	}

	if (DTid.x == 1024) {
		rightIndex = DTid.xy;
	}

	if (DTid.y == 0) {
		topIndex = DTid.xy;
	}

	if (DTid.y == 1024) {
		bottomIndex = DTid.xy;
	}

	topData = colorTex[topIndex].xy;
	bottomData = colorTex[bottomIndex].xy;
	leftData = colorTex[leftIndex].xy;
	rightData = colorTex[rightIndex].xy;

	if (centerData.x == 1.0f){
		if (topData.x == 2.0f ) {
			uint v;
			InterlockedExchange(Output[0], 1, v);
			InterlockedExchange(Output[1], topData.y,v );
		}
		else if (bottomData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[0], 1, v);
			InterlockedExchange(Output[1], bottomData.y, v);
		}
		else if (leftData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[0], 1, v);
			InterlockedExchange(Output[1], rightData.y, v);
		}
		else if (rightData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[0], 1, v);
			InterlockedExchange(Output[1], leftData.y, v);
		}
	}
	
}