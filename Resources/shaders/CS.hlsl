RWStructuredBuffer<uint32_t> Output : register(u0);
RWStructuredBuffer<float32_t3> HitPos : register(u1);
Texture2D<float4> colorTex : register(t0);
Texture2D<float4> depthTex : register(t1);
SamplerState smp : register(s0);

float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
	// xは0~1から-1~1, yは0~1から1~-1に上下反転
	float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float4 position = float4(xy, depth, 1.0f);
	position = mul(position, viewProjectionInverseMatrix);
	position.xyz /= position.w;
	return position.xyz;
}

struct Enemy {
	uint32_t num;
};
ConstantBuffer<Enemy> enemy : register(b0);

struct ViewProjection {
	float32_t4x4 viewProjection;
	float32_t4x4 inverseViewProjection;
	float32_t3 viewPosition;
};
ConstantBuffer<ViewProjection> gViewProjection  : register(b1);

[numthreads(30, 30, 1)]
void main( uint3 DTid : SV_DispatchThreadID)
{
	if (DTid.x == 0 && DTid.y == 0) {
		uint v;
		for (int i = 0; i < enemy.num;i++) {
			InterlockedExchange(Output[i], 0, v);
		}
	}

	float32_t2 centerData = colorTex[DTid.xy].xy;

	if (centerData.y == 1.0f ) {
	    
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
		float32_t2 uv = DTid.xy / texSize;
	    
	    topData = colorTex[topIndex].xyz;
	    bottomData = colorTex[bottomIndex].xyz;
	    leftData = colorTex[leftIndex].xyz;
	    rightData = colorTex[rightIndex].xyz;
	    
	    
	    if (topData.y == 2.0f && DTid.y != 0.0f) {
	    	uint v;
			float32_t v3;
	    	InterlockedExchange(Output[topData.z], 1, v);
			uint32_t3 worldPos = GetWorldPosition(uv,depthTex[DTid.xy].x, gViewProjection.inverseViewProjection);
			InterlockedExchange(HitPos[0].x, worldPos.x, v3);
			InterlockedExchange(HitPos[0].y, worldPos.y, v3);
			InterlockedExchange(HitPos[0].z, worldPos.z, v3);
	    }
	    else if (bottomData.y == 2.0f && DTid.y != 720) {
	    	uint v;
			float32_t v3;
	    	InterlockedExchange(Output[bottomData.z], 1, v);
			uint32_t3 worldPos = GetWorldPosition(uv, depthTex[DTid.xy].x, gViewProjection.inverseViewProjection);
			InterlockedExchange(HitPos[0].x, worldPos.x, v3);
			InterlockedExchange(HitPos[0].y, worldPos.y, v3);
			InterlockedExchange(HitPos[0].z, worldPos.z, v3);
	    }
	    else if (leftData.y == 2.0f && DTid.x != 0.0f) {
	    	uint v;
			float32_t v3;
	    	InterlockedExchange(Output[leftData.z], 1, v);
			uint32_t3 worldPos = GetWorldPosition(uv, depthTex[DTid.xy].x, gViewProjection.inverseViewProjection);
			InterlockedExchange(HitPos[0].x, worldPos.x, v3);
			InterlockedExchange(HitPos[0].y, worldPos.y, v3);
			InterlockedExchange(HitPos[topData.z].z, worldPos.z, v3);
	    }
	    else if (rightData.y == 2.0f && DTid.x != 1280) {
	    	uint v;
			float32_t v3;
	    	InterlockedExchange(Output[rightData.z], 1, v);
			uint32_t3 worldPos = GetWorldPosition(uv, depthTex[DTid.xy].x, gViewProjection.inverseViewProjection);
			InterlockedExchange(HitPos[0].x, worldPos.x, v3);
			InterlockedExchange(HitPos[0].y, worldPos.y, v3);
			InterlockedExchange(HitPos[0].z, worldPos.z, v3);
	    }
	}
	
}