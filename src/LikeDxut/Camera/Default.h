// LikeDxut/Camera/Default.h - the camera is just with the values ​​immediately after the "DXUT" initialization, these values ​​are in the initial "OnFrameMove".
#pragma once
namespace prj_3d::HelloWinHlsl::LikeDxut::Camera { 
class Default { 
	D3DXMATRIXA16 m_mWorldViewProj;
public:
    Default() : m_mWorldViewProj( 
			  (float)1.58632612
			, (float)-0.150780037
			, (float)-0.478069544
			, (float)-0.478059977
			, (float)-0.319199800
			, (float)2.12791204
			, (float)-0.438230395
			, (float)-0.438221633
			, (float)0.812508523
			, (float)1.13034546
			, (float)0.761211812
			, (float)0.761196554
			, (float)5.39618732e-08
			, (float)4.31695014e-07
			, (float)5.59130096
			, (float)5.69118881
		){}
    D3DXMATRIXA16 getWorldViewProj() const {
		return m_mWorldViewProj; // mWorldViewProj = mWorld * mView * mProj;
    }
};
} // namespace prj_3d::HelloWinHlsl::LikeDxut::Camera
