
struct VSOutput {
	float32_t4  svpos : SV_POSITION;
	float32_t2 uv : TEXCOORD;
};
VSOutput main(float32_t4 pos : POSITION, float32_t2 uv : TEXCOORD) {
	VSOutput output;
	output.svpos = pos;
	output.uv = uv;
	return output;
}