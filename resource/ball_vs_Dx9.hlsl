// vertex shader
float4 main(float3 position : POSITION) : POSITION { 
	float w = 1;
	float4 f = float4( position, w );
	return f;
}
