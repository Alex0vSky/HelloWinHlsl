// Dx/Ctx/Impl/Dx10CtxImpl.h - implementation for DirectX3D version 10
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl {
class Dx10CtxImpl {
public:
	static 
	Ty::StDxCtx_ptr<DxVer::v10>
	create(Sys::Wnd::StToken::cref_t crstWndToken, const Dx::Adapter::Dx10Adapter &oAdapter) 
	{
		Sys::Hr hr;

		// Create Direct3D device and swap chain (double buffered)
		DXGI_SWAP_CHAIN_DESC struDxgiSwapChainDesc = { };
		CPtr< IDXGISwapChain > pcDxgiSwapChain;
		CPtr< ID3D10Device1 > pcD3dDevice10_1;
		struDxgiSwapChainDesc.BufferDesc.Width = crstWndToken.m_uInitialWidth;
		struDxgiSwapChainDesc.BufferDesc.Height = crstWndToken.m_uInitialHeight;
		struDxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		struDxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		struDxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		struDxgiSwapChainDesc.SampleDesc.Count = 1;
		struDxgiSwapChainDesc.SampleDesc.Quality = 0;
		struDxgiSwapChainDesc.BufferUsage = (DXGI_USAGE)DXGI_USAGE_RENDER_TARGET_OUTPUT;
		struDxgiSwapChainDesc.BufferCount = 2;
		struDxgiSwapChainDesc.OutputWindow = crstWndToken.m_hWnd;
		struDxgiSwapChainDesc.Windowed = TRUE;
		// If it is recommended to use DXGI_SWAP_EFFECT_FLIP_DISCARD or DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL for a quick action, then the functionality needs to be redone.
		struDxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		UINT uFlags = 0
#ifdef _DEBUG
				// To display messages in Output, you still need to learn how to use ReportLiveObjects.
				| D3D10_CREATE_DEVICE_DEBUG 
#endif // _DEBUG
				// To output FPS text.
				| D3D10_CREATE_DEVICE_BGRA_SUPPORT
			;
		static const UINT uSDKVersion = D3D10_1_SDK_VERSION;
		hr = ::D3D10CreateDeviceAndSwapChain1( 
				// If you pass NULL here, then the warning is like this -- {DXGI WARNING: IDXGIFactory::CreateSwapChain: Blt-model swap effects (DXGI_SWAP_EFFECT_DISCARD and DXGI_SWAP_EFFECT_SEQUENTIAL) are legacy swap effects that are predominantly superceded by their flip-model counterparts (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD). Please consider updating your application to leverage flip-model swap effects to benefit from modern presentation enhancements. More information is available at http://aka.ms/dxgiflipmodel. [ MISCELLANEOUS WARNING #294: ]}
				oAdapter
				, D3D10_DRIVER_TYPE_HARDWARE
				, NULL
				, uFlags
				, D3D10_FEATURE_LEVEL_10_1
				, uSDKVersion
				, &struDxgiSwapChainDesc
				, pcDxgiSwapChain.ReleaseAndGetAddressOf( )
				, pcD3dDevice10_1.ReleaseAndGetAddressOf( )
			);
		// Get a D3D10.0 device pointer from the D3D10.1 device
		//	for ID3D10Device::CreateTexture2D in format B8G8R8A8_UNORM
		CPtr< ID3D10Device > pcD3dDevice10_0;
		hr = pcD3dDevice10_1 ->QueryInterface( IID_PPV_ARGS( pcD3dDevice10_0.ReleaseAndGetAddressOf( ) ) );

		// Create primary render target
		CPtr< ID3D10Texture2D > pcD3dTexture2DBackBuffer; // _Post_ _Notnull_ 
		CPtr< ID3D10RenderTargetView > pcD3dRenderTargetView;
		hr = pcDxgiSwapChain ->GetBuffer( 0, IID_PPV_ARGS( pcD3dTexture2DBackBuffer.ReleaseAndGetAddressOf( ) ) );
		hr = pcD3dDevice10_0 ->CreateRenderTargetView( pcD3dTexture2DBackBuffer.Get( ), NULL, pcD3dRenderTargetView.ReleaseAndGetAddressOf( ) );
		pcD3dDevice10_0 ->OMSetRenderTargets( 1, pcD3dRenderTargetView.GetAddressOf( ), NULL );

		// Set Viewport
		D3D10_VIEWPORT stD3dViewport = { };
		stD3dViewport.Width = crstWndToken.m_uInitialWidth;
		stD3dViewport.Height = crstWndToken.m_uInitialHeight;
		stD3dViewport.MinDepth = 0.0f;
		stD3dViewport.MaxDepth = 1.0f;
		stD3dViewport.TopLeftX = 0;
		stD3dViewport.TopLeftY = 0;
		pcD3dDevice10_0 ->RSSetViewports( 1, &stD3dViewport );

		return Dx::Ctx::CtxDx< DxVer::v10 >::z_make_shared(
				pcDxgiSwapChain
				, pcD3dRenderTargetView
				, pcD3dDevice10_0 
			);
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl
