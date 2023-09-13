// LikeDxut/Camera/Default.h - the camera is just with the values ​​immediately after 
//	the "DXUT" initialization, these values ​​are in the initial "OnFrameMove".
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::LikeDxut::Camera { 
class Default { 
	D3DXMATRIXA16 m_mWorldViewProj;
	
 public:
    Default() 
		: m_mWorldViewProj( 
			  static_cast<float>( 1.58632612 )
			, static_cast<float>( -0.150780037 )
			, static_cast<float>( -0.478069544 )
			, static_cast<float>( -0.478059977 )
			, static_cast<float>( -0.319199800 )
			, static_cast<float>( 2.12791204 )
			, static_cast<float>( -0.438230395 )
			, static_cast<float>( -0.438221633 )
			, static_cast<float>( 0.812508523 )
			, static_cast<float>( 1.13034546 )
			, static_cast<float>( 0.761211812 )
			, static_cast<float>( 0.761196554 )
			, static_cast<float>( 5.39618732e-08 )
			, static_cast<float>( 4.31695014e-07 )
			, static_cast<float>( 5.59130096 )
			, static_cast<float>( 5.69118881 )
		)
	 {}
    D3DXMATRIXA16 getWorldViewProj() const {
		return m_mWorldViewProj; // mWorldViewProj = mWorld * mView * mProj;
    }
};
} // namespace prj_3d::HelloWinHlsl::LikeDxut::Camera
