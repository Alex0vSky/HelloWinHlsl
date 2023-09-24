// Dx/Spec/D12/WindowResizer.h - make windows resize 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 {
class WindowResizer : public Sys::Wnd::AWndProcHolder {
	using AWndProcHolder::AWndProcHolder;

	Ty::StDxCtx_ptr< DxVer::v12 > m_psstDxCtx;
	std::mutex *m_pMutex = nullptr;
	Dx::Spec::D12::Waiter *m_pWaiter = nullptr;
	Dx::Spec::D12::Resizer *m_pResizer = nullptr;
	bool m_bWindowedMode = true;
	bool m_bFullscreenMode = false;
	RECT m_stWindowRect = { };
	UINT m_uWindowStyle = 0;
	HMENU m_hMenu = nullptr;
	bool m_bIsToggleFullscreenWindow = false;

	typedef std::mutex mutex_t;
	typedef std::lock_guard< mutex_t > lock_t;

	void onWindowSizeChanged(UINT width, UINT height) override {
		if ( !m_psstDxCtx ) 
			return;
		uptr< lock_t > scoped;
		if ( m_pMutex ) {
			if ( !m_bIsToggleFullscreenWindow ) {
				scoped = std::make_unique< lock_t >( *m_pMutex );
			}
		}

		LONG lWidth = static_cast< LONG >( width )
			, lHeight = static_cast< LONG >( height );
		float fWidth = static_cast< float >( width )
			, fHeight = static_cast< float >( height );

		char buff[64] = {};
		sprintf_s( buff, "OnWindowSizeChanged. w: %u, h: %u\n"
				, width, height
			);
		::OutputDebugStringA( buff );

		Sys::Hr hr;
		m_pWaiter ->waitForGpu( );

		// Release resources that are tied to the swap chain and update fence values.
		for (UINT n = 0; n < m_psstDxCtx ->FrameCount; n++) {
			m_pResizer ->getRenderTarget( n ).Reset( );
			m_pWaiter ->afterResetRenderTarget( n );
		}
		// Determine the render target size in pixels.
		const UINT backBufferWidth = std::max< UINT >( width, 1u );
		const UINT backBufferHeight = std::max< UINT >( height, 1u );

		// If the swap chain already exists, resize it.
        DXGI_SWAP_CHAIN_DESC desc = {};
        m_psstDxCtx ->m_pcDxgiSwapChain ->GetDesc( &desc );
		//m_pWaiter ->waitForGpu( );
		hr = m_psstDxCtx ->m_pcDxgiSwapChain ->ResizeBuffers(
				m_psstDxCtx ->FrameCount
				, backBufferWidth
				, backBufferHeight
				, desc.BufferDesc.Format
				, desc.Flags // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
			);

		// Obtain the back buffers for this window which will be the final render targets
		// and create render target views for each of them.
		for (UINT n = 0; n < m_psstDxCtx ->FrameCount; n++) {
			hr = m_psstDxCtx ->m_pcDxgiSwapChain ->GetBuffer(
					n
					, IID_PPV_ARGS( m_pResizer ->getRenderTarget( n ).GetAddressOf( ) )
				);

			wchar_t name[25] = { };
			swprintf_s( name, L"Render target %u", n );
			m_pResizer ->getRenderTarget( n ) ->SetName( name );

			auto cpuHandle = m_psstDxCtx ->m_rtvHeap ->GetCPUDescriptorHandleForHeapStart( );
			const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
					cpuHandle
					, static_cast< INT >( n )
					, m_psstDxCtx ->m_rtvDescriptorSize
				);
			m_psstDxCtx ->m_pcD3dDevice12 ->CreateRenderTargetView(
						m_pResizer ->getRenderTarget( n ).Get( )
					, nullptr
					, rtvDescriptor
				);
		}

		// Reset the index to the current back buffer.
		m_pWaiter ->set_frameIndex( m_psstDxCtx ->m_pcDxgiSwapChain ->GetCurrentBackBufferIndex( ) );

		m_pResizer ->setViewport( CD3DX12_VIEWPORT( 0.0f, 0.0f, fWidth, fHeight ) );
		m_pResizer ->setScissorRect( CD3DX12_RECT( 0, 0, lWidth, lHeight ) );

		BOOL bIsFullscreen;
		hr = m_psstDxCtx ->m_pcDxgiSwapChain ->GetFullscreenState( &bIsFullscreen, nullptr );
		m_bWindowedMode = !bIsFullscreen;
	}
	void toggleFullscreenWindow() override {
		if ( !m_psstDxCtx ) 
			return;
		uptr< lock_t > scoped;
		if ( m_pMutex ) {
			scoped = std::make_unique< lock_t >( *m_pMutex );
		}
		m_bIsToggleFullscreenWindow = true;
		sptr<void> resetFalse(nullptr, [&](void*){ m_bIsToggleFullscreenWindow = false; });


		Sys::Hr hr;
		auto stWndToken = AWndProcHolder::getWndToken( );
		HWND hWnd = stWndToken.m_hWnd;
		// TODO(Alex0vSky): remakeme, lazy to make calling of parent ctor
		if ( m_stWindowRect == RECT{ } ) {
			m_stWindowRect = stWndToken.m_stWindowRect;
			m_uWindowStyle = stWndToken.m_uWindowStyle;
		}

		if ( !m_psstDxCtx ->detail_.m_bTearingSupport ) {
            BOOL bIsFullscreen;
            hr = m_psstDxCtx ->m_pcDxgiSwapChain ->GetFullscreenState( &bIsFullscreen, nullptr );
			bool bFailed = FAILED( 
					m_psstDxCtx ->m_pcDxgiSwapChain ->SetFullscreenState( !bIsFullscreen, nullptr ) 
				);
            if ( bFailed ) {
                // Transitions to fullscreen mode can fail when running apps over
                // terminal services or for some other unexpected reason.  Consider
                // notifying the user in some way when this happens.
                Sys::ErrorHandler::showMsgAndExit( "Fullscreen transition failed" );
            }
			return;
        }

		if ( m_bFullscreenMode ) {
			// Restore the window's attributes and size.
			::SetWindowLongW( hWnd, GWL_STYLE, m_uWindowStyle );
			::SetWindowPos(
					hWnd
					, HWND_NOTOPMOST
					, m_stWindowRect.left
					, m_stWindowRect.top
					, m_stWindowRect.right - m_stWindowRect.left
					, m_stWindowRect.bottom - m_stWindowRect.top
					, SWP_FRAMECHANGED | SWP_NOACTIVATE
				);
			// Restore menu
			::SetMenu( hWnd, m_hMenu );
			::ShowWindow( hWnd, SW_NORMAL);
		} else {
			// Save the old window rect so we can restore it when exiting fullscreen mode.
			::GetWindowRect( hWnd, &m_stWindowRect );
			// Make the window borderless so that the client area can fill the screen.
			UINT uWindowStyleMask = WS_CAPTION | WS_MAXIMIZEBOX 
				| WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
			::SetWindowLongW( 
					hWnd
					, GWL_STYLE
					, m_uWindowStyle & ~( uWindowStyleMask )
				);
			// Remove menu
			m_hMenu = ::GetMenu( hWnd );
			::SetMenu( hWnd, NULL );
			RECT fullscreenWindowRect;
			try {
				// Get the settings of the display on which the app's window is currently displayed
				CPtr< IDXGIOutput > pOutput;
				hr = m_psstDxCtx ->m_pcDxgiSwapChain ->GetContainingOutput( pOutput.ReleaseAndGetAddressOf( ) );
				DXGI_OUTPUT_DESC Desc;
				hr = pOutput ->GetDesc( &Desc );
				fullscreenWindowRect = Desc.DesktopCoordinates;
			}
			catch (_com_error& e) {
				UNREFERENCED_PARAMETER( e );
				// Get the settings of the primary display
				DEVMODEW devMode = { };
				devMode.dmSize = sizeof( devMode );
				::EnumDisplaySettingsW( nullptr, ENUM_CURRENT_SETTINGS, &devMode );
				fullscreenWindowRect = {
						devMode.dmPosition.x
						, devMode.dmPosition.y
						, devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth)
						, devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
					};
			}

			::SetWindowPos(
					hWnd
					, HWND_TOPMOST
					, fullscreenWindowRect.left
					, fullscreenWindowRect.top
					, fullscreenWindowRect.right
					, fullscreenWindowRect.bottom
					, SWP_FRAMECHANGED | SWP_NOACTIVATE
				);
			::ShowWindow( hWnd, SW_MAXIMIZE);
		}
		m_bFullscreenMode = !m_bFullscreenMode;
	}

 public:
	typedef uptr< WindowResizer > uptr_t;

	void setDxCtx(
		Ty::StDxCtx_crefPtr< DxVer::v12 > psstDxCtx
		, std::mutex *pMutex
		, Dx::Spec::D12::Waiter *pWaiter
		, Dx::Spec::D12::Resizer *pResizer
	) {
		m_psstDxCtx = psstDxCtx;
		m_pMutex = pMutex;
		m_pWaiter = pWaiter;
		m_pResizer = pResizer;
	}
	bool isWindowed() const {
		return m_bWindowedMode;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12
