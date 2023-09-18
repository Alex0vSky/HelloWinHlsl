// vertex shader

float4 main(float3 position : POSITION) : SV_POSITION { // vsh
	float w = 1;
	float4 f = float4( position, w );
	//f.y = f.y + 0.3; // NEW
	return f;
}
