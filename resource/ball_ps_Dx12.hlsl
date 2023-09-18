// pixel shader
cbuffer PerFrameConstants : register (b0) { float iTime; }
cbuffer PerFrameConstants : register (b1) { float2 iResolution; }

static const bool bFromBottomToTop = true;

float4 main(float4 position : SV_POSITION) : SV_TARGET { // psh

	float fSpeed = 2;
	float qwe = ((iTime) % fSpeed) / fSpeed;
	
	float2 uv;
	
	uv = position.xy / iResolution.xy;
	if ( bFromBottomToTop )
		uv.y = 1 - uv.y;
	uv -= qwe;
	uv.x *= ( iResolution.x / iResolution.y );
	

	float2 r = float2( uv.x, uv.y );
	float length_r = length(r);
	float ball = 0.05 / length_r;
	float col = ball;
	return float4(col * 0.8, 0, 0, 1.0);
}
