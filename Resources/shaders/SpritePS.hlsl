

struct cbuff0 {
	float32_t4  color; 
	float32_t4x4 mat; 
};
ConstantBuffer<cbuff0> gCbuff0  : register(b0);
struct VSOutput {
	float32_t4  svpos : SV_POSITION; 
	float32_t2  uv : TEXCOORD; 
};

Texture2D<float4> tex : register(t0); 
SamplerState smp : register(s0);   

float32_t4 main(VSOutput input) : SV_TARGET{
	return tex.Sample(smp, input.uv) * gCbuff0.color; }