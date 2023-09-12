// Dx/Ctx/Impl/Dx9CtxImpl.h - implementation for DirectX3D version 9
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl {
class Dx9CtxImpl {
public:
	static 
	Ty::StDxCtx_ptr< DxVer::v9 > 
	create(Sys::Wnd::StToken::cref_t crstWndToken, const Dx::Adapter::Dx9Adapter &oAdapter) 
	{
		Sys::Hr hr;

		CPtr< IDirect3D9 > pcD3D;
		if( !( *pcD3D.ReleaseAndGetAddressOf( ) = ::Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return nullptr;

		// Set up the structure used to create the D3DDevice
		D3DPRESENT_PARAMETERS stD3dpp = { };
		stD3dpp.Windowed = TRUE;
		stD3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		stD3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // D3DFMT_UNKNOWN

		CPtr< IDirect3DDevice9 > pcDevice;
		// Create the D3DDevice
		hr = pcD3D ->CreateDevice( 
				oAdapter.getIdx( )
				, D3DDEVTYPE_HAL
				, crstWndToken.m_hWnd
				, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE
				, &stD3dpp, pcDevice.ReleaseAndGetAddressOf( )
			);
		return Dx::Ctx::CtxDx< DxVer::v9 >::z_make_shared(
				pcDevice
			);
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl
