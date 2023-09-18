// pixel shader
cbuffer cb0
{
	float2 ve2Resolution;
	float fTime;
};
static const bool bFromBottomToTop = true;
float4 main(float4 position : SV_POSITION) : SV_TARGET { // psh
	float fSpeed = 2;
	float qwe = ((fTime) % fSpeed) / fSpeed;
	
	float2 uv;
	
	uv = position.xy / ve2Resolution.xy;
	if ( bFromBottomToTop )
		uv.y = 1 - uv.y;
	uv -= qwe;
	uv.x *= ( ve2Resolution.x / ve2Resolution.y );
	

	float2 r = float2( uv.x, uv.y );
	float length_r = length(r);
	float ball = 0.05 / length_r;
	float col = ball;
	//col = max(fmod(col, 0.4), min(col, 2.0)); // особо не нужно
	return float4(col * 0.8, 0, 0, 1.0);
}
