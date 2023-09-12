// Dx/Ctx/Impl/Dx11CtxImpl.h - implementation for DirectX3D version 11
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl {
class Dx11CtxImpl {
public:
	static 
	Ty::StDxCtx_ptr<DxVer::v11> 
	create(Sys::Wnd::StToken::cref_t crstWndToken, const Dx::Adapter::Dx11Adapter &oAdapter) 
	{
		Sys::Hr hr;

		// Create Direct3D device and swap chain (double buffered)
		DXGI_SWAP_CHAIN_DESC struDxgiSwapChainDesc = { };
		CPtr< IDXGISwapChain > pcDxgiSwapChain;
		CPtr< ID3D11Device > pcD3dDevice11;
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
		struDxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		UINT uFlags = 0
#ifdef _DEBUG
				// To display messages in Output, you still need to learn how to use ReportLiveObjects.
				| D3D11_CREATE_DEVICE_DEBUG 
#endif // _DEBUG
				// Something new
				| D3D11_CREATE_DEVICE_SINGLETHREADED
				// To output FPS text.
				| D3D11_CREATE_DEVICE_BGRA_SUPPORT
			;
		const D3D_FEATURE_LEVEL arrFl[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 };
		static const UINT uSDKVersion = D3D11_SDK_VERSION;
		CPtr< ID3D11DeviceContext > pcDeviceContext11;
		hr = ::D3D11CreateDeviceAndSwapChain( 
				// If you pass NULL here, then the warning is like this -- {DXGI WARNING: IDXGIFactory::CreateSwapChain: Blt-model swap effects (DXGI_SWAP_EFFECT_DISCARD and DXGI_SWAP_EFFECT_SEQUENTIAL) are legacy swap effects that are predominantly superceded by their flip-model counterparts (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD). Please consider updating your application to leverage flip-model swap effects to benefit from modern presentation enhancements. More information is available at http://aka.ms/dxgiflipmodel. [ MISCELLANEOUS WARNING #294: ]}
				oAdapter
				// When creating a device from an existing adapter (i.e. pAdapter is non-NULL), DriverType must be D3D_DRIVER_TYPE_UNKNOWN
				, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN
				, NULL
				, uFlags
				, arrFl
				, _countof( arrFl )
				, uSDKVersion
				, &struDxgiSwapChainDesc
				, pcDxgiSwapChain.ReleaseAndGetAddressOf( )
				, pcD3dDevice11.ReleaseAndGetAddressOf( )
				, nullptr
				, pcDeviceContext11.ReleaseAndGetAddressOf( )
			);

		// Create primary render target
		CPtr< ID3D11Texture2D > pcD3dTexture2DBackBuffer; // _Post_ _Notnull_ 
		CPtr< ID3D11RenderTargetView > pcD3dRenderTargetView;
		hr = pcDxgiSwapChain ->GetBuffer( 0, IID_PPV_ARGS( pcD3dTexture2DBackBuffer.ReleaseAndGetAddressOf( ) ) );
		hr = pcD3dDevice11 ->CreateRenderTargetView( pcD3dTexture2DBackBuffer.Get( ), NULL, pcD3dRenderTargetView.ReleaseAndGetAddressOf( ) );
		pcDeviceContext11 ->OMSetRenderTargets( 1, pcD3dRenderTargetView.GetAddressOf( ), NULL );

		// Set Viewport
		D3D11_VIEWPORT stD3dViewport = { };
		stD3dViewport.Width = (FLOAT)crstWndToken.m_uInitialWidth;
		stD3dViewport.Height = (FLOAT)crstWndToken.m_uInitialHeight;
		stD3dViewport.MinDepth = 0.0f;
		stD3dViewport.MaxDepth = 1.0f;
		stD3dViewport.TopLeftX = 0;
		stD3dViewport.TopLeftY = 0;
		pcDeviceContext11 ->RSSetViewports( 1, &stD3dViewport );

		return Dx::Ctx::CtxDx< DxVer::v11 >::z_make_shared(
				pcDxgiSwapChain
				, pcD3dRenderTargetView
				, pcD3dDevice11
				, pcDeviceContext11
			);
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl
