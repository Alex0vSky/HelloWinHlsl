// Dx/Descr.h - get description 
#pragma once
namespace prj_3d::HelloWinHlsl::Dx { 
template<class T> struct Descr; // primary template

template<>
struct Descr<DxVer::v9> { 
	using T=DxVer::v9;
	static 
	D3DPRESENT_PARAMETERS 
	presentParameters(Ty::StDxCtx_crefPtr<T> crpustCtxDx) {
		CPtr< IDirect3DSwapChain9 > pcSwapChain;
		crpustCtxDx ->m_pcD3dDevice ->GetSwapChain( 0, pcSwapChain.ReleaseAndGetAddressOf( ) ); 
		D3DPRESENT_PARAMETERS st = { };
		pcSwapChain ->GetPresentParameters( &st );
		return st;
	}
};

template<>
struct Descr<DxVer::v10> { 
	using T=DxVer::v10;
	static 
	DXGI_SWAP_CHAIN_DESC 
	swapChain(Ty::StDxCtx_crefPtr<T> crpustCtxDx) {
		DXGI_SWAP_CHAIN_DESC stDesc = { };
		crpustCtxDx ->m_pcDxgiSwapChain ->GetDesc( &stDesc );
		return stDesc;
	}
};

template<>
struct Descr<DxVer::v11> { 
	using T=DxVer::v11;
	static 
	DXGI_SWAP_CHAIN_DESC 
	swapChain(Ty::StDxCtx_crefPtr<T> crpustCtxDx) {
		DXGI_SWAP_CHAIN_DESC stDesc = { };
		crpustCtxDx ->m_pcDxgiSwapChain ->GetDesc( &stDesc );
		return stDesc;
	}
};

template<>
struct Descr<DxVer::v12> { 
	using T=DxVer::v12;
	static 
	DXGI_SWAP_CHAIN_DESC 
	swapChain(Ty::StDxCtx_crefPtr<T> crpustCtxDx) {
		DXGI_SWAP_CHAIN_DESC stDesc = { };
		crpustCtxDx ->m_pcDxgiSwapChain ->GetDesc( &stDesc );
		return stDesc;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Descr
