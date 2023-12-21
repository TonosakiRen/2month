
struct cbuff0 {
	float32_t4  color; 
	float32_t4x4 mat;   
};
ConstantBuffer<cbuff0> gCbuff0  : register(b0);
struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2 uv : TEXCOORD;   
};
VSOutput main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD) {
	VSOutput output;
	output.svpos = mul(pos, gCbuff0.mat);
	output.uv = uv;
	return output;
}