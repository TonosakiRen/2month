struct cbuff0 {
	float4 color; 
	matrix mat;  
};
ConstantBuffer<cbuff0> gCbuff0  : register(b0);

struct VSOutput {
	float4 svpos : SV_POSITION; 
	float2 uv : TEXCOORD;       
};
