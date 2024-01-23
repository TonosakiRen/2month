RWStructuredBuffer<uint32_t> Output : register(u0);
Texture2D<float4> colorTex : register(t0);
SamplerState smp : register(s0);
struct Enemy {
	uint32_t num;
};
ConstantBuffer<Enemy> enemy : register(b0);
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
	if (DTid.x == 0 && DTid.y == 0) {
		uint v;
		for (int i = 0; i < enemy.num;i++) {
			InterlockedExchange(Output[i], 0, v);
		}
	}

	float32_t2 centerData = colorTex[DTid.xy].xy;

	float32_t3 topData = float32_t3(0.0f, 0.0f,0.0f);
	float32_t3 bottomData = float32_t3(0.0f, 0.0f,0.0f);
	float32_t3 leftData = float32_t3(0.0f, 0.0f,0.0f);
	float32_t3 rightData = float32_t3(0.0f, 0.0f,0.0f);

	uint32_t2 topIndex = uint32_t2(DTid.x, DTid.y - 1.0f);
	uint32_t2 bottomIndex = uint32_t2(DTid.x, DTid.y + 1.0f);
	uint32_t2 leftIndex = uint32_t2(DTid.x - 1.0f, DTid.y);
	uint32_t2 rightIndex = uint32_t2(DTid.x + 1.0f, DTid.y);

	float32_t2 texSize;
	//テクスチャーのサイズ
	colorTex.GetDimensions(texSize.x, texSize.y);

	if (DTid.x == 0) {
		leftIndex = DTid.xy;
	}

	if (DTid.x == texSize.x) {
		rightIndex = DTid.xy;
	}

	if (DTid.y == 0) {
		topIndex = DTid.xy;
	}

	if (DTid.y == texSize.y) {
		bottomIndex = DTid.xy;
	}

	topData = colorTex[topIndex].xyz;
	bottomData = colorTex[bottomIndex].xyz;
	leftData = colorTex[leftIndex].xyz;
	rightData = colorTex[rightIndex].xyz;

	if (centerData.y == 1.0f){
		if (topData.y == 2.0f ) {
			uint v;
			InterlockedExchange(Output[topData.z], 1, v);
		}
		else if (bottomData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[bottomData.z], 1, v);
		}
		else if (leftData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[leftData.z], 1, v);
		}
		else if (rightData.y == 2.0f) {
			uint v;
			InterlockedExchange(Output[rightData.z], 1, v);
		}
	}
	
}