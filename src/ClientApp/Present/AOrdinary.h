// ClientApp/Present/AOrdinary.h - the usual display of the rendered frame on the screen.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::ClientApp::Present { 

template<class T> class AOrdinary { };

template<> class AOrdinary<DxVer::v9> : public ABaseClientApp< DxVer::v9 > { 
	using TInnerDxVer = DxVer::v9;
	
 public:
    typedef uptr< AOrdinary > uptr_t;
	void present_to_screen(
		typename Dx::Ctx::CtxDx<TInnerDxVer>::cref_ptr_t crpustDxCtx
		, Dx::ClientDynamicData<TInnerDxVer>::cref_ptr_t
	) override {
	    crpustDxCtx ->m_pcD3dDevice ->Present( NULL, NULL, NULL, NULL );
	}
};

template<> class AOrdinary<DxVer::v10> : public ABaseClientApp< DxVer::v10 > { 
	using TInnerDxVer = DxVer::v10;
	
 public:
    typedef uptr< AOrdinary > uptr_t;
	void present_to_screen(
		typename Dx::Ctx::CtxDx<TInnerDxVer>::cref_ptr_t crpustDxCtx
		, Dx::ClientDynamicData<TInnerDxVer>::cref_ptr_t
	) override {
// TODO(Alex0vSky): if( GetDXUTState().GetRenderingOccluded() ) Do not present the frame to the output. 
//	The status of the swap chain will be tested and appropriate errors returned. 
//	DXGI_PRESENT_TEST is intended for use only when switching from the idle state; 
//	do not use it to determine when to switch to the idle state because doing so can 
//	leave the swap chain unable to exit full-screen mode.
		//dwFlags = DXGI_PRESENT_TEST; else {
		// ...\directx-sdk-legacy-samples\DXUT\Core\DXUT.cpp:3684:DXUTSetD3D10Device
		//// Fill out the rest of the device settings struct
		//DeviceSettings = { 0 };
		//DeviceSettings.d3d10.DriverType = D3D10_DRIVER_TYPE_HARDWARE;
		//dwFlags = GetDXUTState().GetCurrentDeviceSettings()->d3d10.PresentFlags;
		//} but you also need to disable frame rendering, i.e. gameLoop change.
		DWORD dwFlags = 0; // ...\directx-sdk-legacy-samples\DXUT\Core\DXUT.cpp:1421:DXUTStaticWndProc
		UINT uiSyncInterval = DXGI_SWAP_EFFECT_DISCARD;
        HRESULT hrPresent = crpustDxCtx ->m_pcDxgiSwapChain ->Present( uiSyncInterval, dwFlags );
		// The window is blocked completely (or, for example, minimized).
		if( SUCCEEDED( hrPresent ) ) {
		} else {
			switch ( hrPresent ) {
				case DXGI_STATUS_OCCLUDED: break;
				case DXGI_ERROR_DEVICE_RESET: break;
				case DXGI_ERROR_DEVICE_REMOVED: break;
			}
		}
	}
    virtual ~AOrdinary() {
    }
};

template<> class AOrdinary<DxVer::v11> : public ABaseClientApp< DxVer::v11 > { 
	using TInnerDxVer = DxVer::v11;
	
 public:
    typedef uptr< AOrdinary > uptr_t;
	void present_to_screen(
		typename Dx::Ctx::CtxDx<TInnerDxVer>::cref_ptr_t crpustDxCtx
		, Dx::ClientDynamicData<TInnerDxVer>::cref_ptr_t
	) override {
		DWORD dwFlags = 0; // ...\directx-sdk-legacy-samples\DXUT\Core\DXUT.cpp:1421:DXUTStaticWndProc
		UINT uiSyncInterval = DXGI_SWAP_EFFECT_DISCARD;
        HRESULT hrPresent = crpustDxCtx ->m_pcDxgiSwapChain ->Present( uiSyncInterval, dwFlags );
		// The window is blocked completely (or, for example, minimized).
		if( SUCCEEDED( hrPresent ) ) {
		} else {
			switch ( hrPresent ) {
				case DXGI_STATUS_OCCLUDED: break;
				case DXGI_ERROR_DEVICE_RESET: break;
				case DXGI_ERROR_DEVICE_REMOVED: break;
			}
		}
	}
    virtual ~AOrdinary() {
    }
};

template<> class AOrdinary<DxVer::v12> : public ABaseClientApp< DxVer::v12 > { 
	using TInnerDxVer = DxVer::v12;

 public:
    typedef uptr< AOrdinary > uptr_t;
	void present_to_screen(
		typename Dx::Ctx::CtxDx<TInnerDxVer>::cref_ptr_t crpustDxCtx
		, Dx::ClientDynamicData<TInnerDxVer>::cref_ptr_t crpsoDynamic
	) override {
		UINT presentFlags = 0;
		if ( crpustDxCtx ->detail_.m_bTearingSupport && crpsoDynamic ->m_isWindowed )
			presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
		// DXGI_SWAP_EFFECT_SEQUENTIAL
		UINT uiSyncInterval = DXGI_SWAP_EFFECT_DISCARD;
        HRESULT hrPresent = crpustDxCtx ->m_pcDxgiSwapChain ->
			Present( uiSyncInterval, presentFlags );
		// The window is blocked completely (or, for example, minimized).
		if( SUCCEEDED( hrPresent ) ) {
		} else {
			switch ( hrPresent ) {
				case DXGI_STATUS_OCCLUDED: break;
				case DXGI_ERROR_DEVICE_RESET: break;
				case DXGI_ERROR_DEVICE_REMOVED: break;
			}
		}
	}
    virtual ~AOrdinary() {
    }
};
} // namespace prj_3d::HelloWinHlsl::ClientApp::Present
