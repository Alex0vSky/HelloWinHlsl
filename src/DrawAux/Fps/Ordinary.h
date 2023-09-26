// DrawAux/Fps/Ordinary.h - second try "Direct2D/DXGI Interoperation Sample".
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
// It's interesting that everything works without "if (SUCCEEDED(CoInitialize(NULL)))".
// You can no longer use "D3DX10CreateFont"
//	https://stackoverflow.com/questions/42333236/cannot-open-source-file-d3dx10-h
// @insp https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/multimedia/DirectWrite/HelloWorld/SimpleText.cpp // NOLINT(whitespace/line_length)
// @insp https://docs.microsoft.com/en-us/samples/microsoft/windows-classic-samples/direct2ddxgi/ // NOLINT(whitespace/line_length)
// @insp https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/multimedia/Direct2D/DXGISample/DxgiSample.cpp // NOLINT(whitespace/line_length)
// @insp https://github.com/Microsoft/DirectXTK/wiki/ComPtr // NOLINT(whitespace/line_length)
// @insp https://www.unknowncheats.me/forum/d3d-tutorials-and-source/126704-drawing-function-directx-9-a.html // NOLINT(whitespace/line_length)
// TODO(Alex0vSky): rename Fps/Ordinary to Fps/ToWindowSufrace or Fps/ToTexture or Fps/ToImage

namespace prj_3d::HelloWinHlsl::DrawAux::Fps { 
template<class T> class Ordinary; // primary template

namespace detail_ {
// Drawing config
struct BaseDrawCfg {
	static INT getFontSize() {
		return 28;
	}
	static auto getFontFamily() {
		return std::wstring( L"Arial" );
	}
	static RECT getRect() {
		return RECT{ 0, 0, 200, 50 };
	}
	static D2D1::ColorF getColor() {
		return D2D1::ColorF( D2D1::ColorF::White );
	}
};
template<class T> struct DrawCfg; // primary template
template<> struct DrawCfg<DxVer::v9> : public BaseDrawCfg {
	static D3DCOLOR getColor() {
		auto color = BaseDrawCfg::getColor( );
		return D3DCOLOR_XRGB( BYTE(color.r*255.f), BYTE(color.g*255.f), BYTE(color.b*255.f) );
	}
};
template<> struct DrawCfg<DxVer::v10> : public BaseDrawCfg {
	static FLOAT getFontSize() {
		return (FLOAT)BaseDrawCfg::getFontSize( );
	}
	static D2D_RECT_F getRect() {
		auto r = BaseDrawCfg::getRect( );
		return D2D1::RectF( (FLOAT)r.left, (FLOAT)r.top, (FLOAT)r.right, (FLOAT)r.bottom );
	}
};

template<> struct DrawCfg<DxVer::v11> : public BaseDrawCfg {
	static FLOAT getFontSize() {
		return (FLOAT)BaseDrawCfg::getFontSize( );
	}
	static D2D_RECT_F getRect() {
		auto r = BaseDrawCfg::getRect( );
		return D2D1::RectF( (FLOAT)r.left, (FLOAT)r.top, (FLOAT)r.right, (FLOAT)r.bottom );
	}
};

template<> struct DrawCfg<DxVer::v12> : public BaseDrawCfg {
	// Only left-top corner possible right now
	static DirectX::XMFLOAT2 getRect() {
		auto r = BaseDrawCfg::getRect( );
		return DirectX::XMFLOAT2( 
				static_cast<float>( r.left )
				, static_cast<float>( r.top )
			);
	}
};

// Let's get the average FPS value on a certain segment so that the number does not jump.
// @insp bgfx.
class FrameTime {
	static constexpr float kFloatMin = 1.175494e-38f, kFloatMax = 3.402823e+38f;
	static constexpr uint32_t kNumSamples = 100;
	int32_t m_offset;
	float m_values[kNumSamples];
	template<typename Ty> inline constexpr Ty min_(const Ty& _a, const Ty& _b) {
		return _a < _b ? _a : _b;
	}

	template<typename Ty> inline constexpr Ty max_(const Ty& _a, const Ty& _b) {
		return _a > _b ? _a : _b;
	}
	
 public:
	float m_min, m_max, m_avg;
	FrameTime() // = default;
		: m_offset(0), m_values{ }, m_min(0), m_max(0), m_avg(0)
	 {}
	void pushSample(float value) {
		m_values[m_offset] = value;
		m_offset = (m_offset + 1) % kNumSamples;
		float fMin = kFloatMax;
		float fMax = -kFloatMax;
		float avg = 0.0f;
		for (uint32_t ii = 0; ii < kNumSamples; ++ii) {
			const float val = m_values[ii];
			fMin = this->min_( fMin, val );
			fMax = this->max_( fMax, val );
			avg += val;
		}
		m_min = fMin;
		m_max = fMax;
		m_avg = avg / kNumSamples;
	}
	int64_t getHPFrequency() {
		LARGE_INTEGER li;
		QueryPerformanceFrequency( &li );
		return li.QuadPart;
    }
	int64_t getHPCounter() {
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return li.QuadPart;
	}
};

} // namespace detail_

template<> class Ordinary<DxVer::v9> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;

    int64_t m_llFrameTimeLast, m_llCpuTimerFreq;
	detail_::FrameTime stFrameTime;

	CPtr< ID3DXFont > m_pcFont;

 public:
    typedef uptr< Ordinary > uptr_t;
    explicit Ordinary(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx)
        : m_stDxCtx( stDxCtx ) {
		Sys::Hr hr = ::D3DXCreateFontW( 
				m_stDxCtx ->m_pcD3dDevice.Get( )
				, detail_::DrawCfg<TInnerDxVer>::getFontSize( ), 0
				, FW_NORMAL, 1, 0 
				, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH
				, detail_::DrawCfg<TInnerDxVer>::getFontFamily( ).c_str( )
				, m_pcFont.ReleaseAndGetAddressOf( )
			);
        m_llFrameTimeLast = stFrameTime.getHPCounter( );
        m_llCpuTimerFreq = stFrameTime.getHPFrequency( );
	}
	void renderOnTarget() {
		int64_t llNow = stFrameTime.getHPCounter( );
        //!< CPU time between two `bgfx::frame` calls.
		int64_t llCpuTimeFrame = llNow - m_llFrameTimeLast;
		m_llFrameTimeLast = llNow;
    	const double toMsCpu = 1000.0 / m_llCpuTimerFreq;
    	const double frameMs = llCpuTimeFrame * toMsCpu;
    	stFrameTime.pushSample( static_cast<float>( frameMs ) );
        float fFps = 1000.0f / stFrameTime.m_avg;

        std::wstringstream stream;
        stream << L"FPS " << static_cast<int>( fFps );

		auto rect = detail_::DrawCfg<TInnerDxVer>::getRect( );
		//HRESULT hr = 
			m_pcFont ->DrawTextW( 
				NULL, stream.str( ).c_str( ), -1
				, &rect, DT_LEFT|DT_NOCLIP, detail_::DrawCfg<TInnerDxVer>::getColor( ) 
			);
		__nop( );
	}
};

template<> class Ordinary<DxVer::v10> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
    CPtr<ID2D1RenderTarget> m_pcBackBufferRT;
    CPtr<ID2D1SolidColorBrush> m_pcBackBufferTextBrush;
    CPtr<IDWriteTextFormat> m_pcTextFormat;

	int64_t m_llFrameTimeLast, m_llCpuTimerFreq;
	detail_::FrameTime stFrameTime;
	
 public:
    typedef uptr< Ordinary > uptr_t;
    explicit Ordinary(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx)
        : m_stDxCtx( stDxCtx ) {
        CPtr< IDWriteFactory > pcDWriteFactory;
        CPtr< ID2D1Factory > pcD2DFactory;
        Sys::Hr hr;

        // It's CreateDeviceIndependentResources()
        hr = ::D2D1CreateFactory( 
				D2D1_FACTORY_TYPE_SINGLE_THREADED 
				, pcD2DFactory.ReleaseAndGetAddressOf( ) 
			);

        hr = ::DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED
				, __uuidof( IDWriteFactory )
				, reinterpret_cast< IUnknown** >( pcDWriteFactory.ReleaseAndGetAddressOf( ) )
			);

        hr = pcDWriteFactory ->CreateTextFormat(
            detail_::DrawCfg<TInnerDxVer>::getFontFamily( ).c_str( ),
			// Font collection (NULL sets it to use the system font collection).
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            detail_::DrawCfg<TInnerDxVer>::getFontSize( ),
            L"en-us",
            m_pcTextFormat.ReleaseAndGetAddressOf( )
        );
        // Center align (horizontally) the text.
        hr = m_pcTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        hr = m_pcTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        CPtr< ID3D10Texture2D > pcOffscreenTexture;
        // Allocate a offscreen D3D surface for D2D to render our 2D content into
        D3D10_TEXTURE2D_DESC texDesc;
        texDesc.ArraySize = 1;
        texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.Height = 512;
        texDesc.Width = 512;
        texDesc.MipLevels = 1;
        texDesc.MiscFlags = 0;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D10_USAGE_DEFAULT;
        hr = m_stDxCtx ->m_pcD3dDevice ->CreateTexture2D( 
				&texDesc
				, NULL
				, pcOffscreenTexture.ReleaseAndGetAddressOf( ) 
			);
        CPtr< IDXGISurface > pcDxgiSurface;
		hr = pcOffscreenTexture.As( std::addressof( pcDxgiSurface ) );

		// Create the DXGI Surface Render Target.
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96,
            96
        );
        CPtr< ID2D1RenderTarget > pcRenderTarget;
        hr = pcD2DFactory ->CreateDxgiSurfaceRenderTarget(
				pcDxgiSurface.Get( )
				, &props
				, pcRenderTarget.ReleaseAndGetAddressOf( )
			);

        // Create a red brush for text drawn into the back buffer
        hr = pcRenderTarget ->CreateSolidColorBrush(
            detail_::DrawCfg<TInnerDxVer>::getColor( )
            , m_pcBackBufferTextBrush.ReleaseAndGetAddressOf()
        );
        CPtr< IDXGISurface > pcBackBuffer;
        // Get a surface in the swap chain
        hr = m_stDxCtx ->m_pcDxgiSwapChain->GetBuffer(
				0
				, IID_PPV_ARGS( pcBackBuffer.ReleaseAndGetAddressOf( ) )
			);

        // It is assumed that the window will change size.
        DXGI_SWAP_CHAIN_DESC stDxgiSwapChainDesc = {};
		// or IDXGISwapChain1::GetHwnd method (dxgi1_2.h)
        m_stDxCtx ->m_pcDxgiSwapChain->GetDesc(&stDxgiSwapChainDesc);
        FLOAT dpiX, dpiY;
		//pcD2DFactory ->GetDesktopDpi( &dpiX, &dpiY ); // @depr
        dpiX = dpiY = (FLOAT)::GetDpiForWindow(stDxgiSwapChainDesc.OutputWindow);
        D2D1_RENDER_TARGET_PROPERTIES props2 = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX,
            dpiY
        );

        // Create a D2D render target which can draw into the surface in the swap chain
        hr = pcD2DFactory ->CreateDxgiSurfaceRenderTarget(
				pcBackBuffer.Get( )
				, &props2
				, m_pcBackBufferRT.ReleaseAndGetAddressOf( )
			);

        m_llFrameTimeLast = stFrameTime.getHPCounter( );
        //!< CPU timer frequency. Timestamps-per-second
        m_llCpuTimerFreq = stFrameTime.getHPFrequency( );
    }
    void renderOnTarget() {
		int64_t llNow = stFrameTime.getHPCounter( );
        //!< CPU time between two `bgfx::frame` calls.
		int64_t llCpuTimeFrame = llNow - m_llFrameTimeLast;
		m_llFrameTimeLast = llNow;
    	const double toMsCpu = 1000.0 / m_llCpuTimerFreq;
    	const double frameMs = llCpuTimeFrame * toMsCpu;
    	stFrameTime.pushSample( static_cast<float>( frameMs ) );
        float fFps = 1000.0f / stFrameTime.m_avg;

        std::wstringstream stream;
        stream << L"FPS " << static_cast<int>( fFps );

        Sys::Hr hr;
        m_pcBackBufferRT ->BeginDraw( );
        m_pcBackBufferRT ->SetTransform( D2D1::Matrix3x2F::Identity( ) );
        // Text format object will center the text in layout
        m_pcBackBufferRT ->DrawTextW(
                stream.str( ).c_str( ), (UINT32)stream.str( ).length( ),
                m_pcTextFormat.Get( ),
                detail_::DrawCfg<TInnerDxVer>::getRect( ),
                m_pcBackBufferTextBrush.Get( )
            );
        hr = m_pcBackBufferRT->EndDraw( );
    }
};

// TODO(Alex0vSky): dont repeat yourself, common code with DxVer::v10
template<> class Ordinary<DxVer::v11> {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
    CPtr<ID2D1RenderTarget> m_pcBackBufferRT;
    CPtr<ID2D1SolidColorBrush> m_pcBackBufferTextBrush;
    CPtr<IDWriteTextFormat> m_pcTextFormat;

	int64_t m_llFrameTimeLast, m_llCpuTimerFreq;
	detail_::FrameTime stFrameTime;
	
 public:
    typedef uptr< Ordinary > uptr_t;
    explicit Ordinary(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx)
        : m_stDxCtx( stDxCtx ) {
        CPtr< IDWriteFactory > pcDWriteFactory;
        CPtr< ID2D1Factory > pcD2DFactory;
        Sys::Hr hr;

        // It's CreateDeviceIndependentResources()
        hr = D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED
				, pcD2DFactory.ReleaseAndGetAddressOf( )
			);

        hr = DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_SHARED
				, __uuidof( IDWriteFactory )
				, reinterpret_cast< IUnknown** >( pcDWriteFactory.ReleaseAndGetAddressOf( ) )
			);

        hr = pcDWriteFactory ->CreateTextFormat(
            detail_::DrawCfg<TInnerDxVer>::getFontFamily( ).c_str( ),
			// Font collection (NULL sets it to use the system font collection).
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            detail_::DrawCfg<TInnerDxVer>::getFontSize( ),
            L"en-us",
            m_pcTextFormat.ReleaseAndGetAddressOf( )
        );
        // Center align (horizontally) the text.
        hr = m_pcTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        hr = m_pcTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        CPtr< ID3D11Texture2D > pcOffscreenTexture;
        // Allocate a offscreen D3D surface for D2D to render our 2D content into
        D3D11_TEXTURE2D_DESC texDesc;
        texDesc.ArraySize = 1;
        texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;
        texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.Height = 512;
        texDesc.Width = 512;
        texDesc.MipLevels = 1;
        texDesc.MiscFlags = 0;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        hr = m_stDxCtx ->m_pcD3dDevice ->CreateTexture2D( 
				&texDesc
				, NULL
				, pcOffscreenTexture.ReleaseAndGetAddressOf( ) 
			);
		CPtr< IDXGISurface > pcDxgiSurface;
		hr = pcOffscreenTexture.As( std::addressof( pcDxgiSurface ) );

        // Create the DXGI Surface Render Target.
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            96,
            96
        );
		CPtr< ID2D1RenderTarget > pcRenderTarget;
        hr = pcD2DFactory ->CreateDxgiSurfaceRenderTarget(
				pcDxgiSurface.Get( )
				, &props
				, pcRenderTarget.ReleaseAndGetAddressOf( )
			);

        // Create a red brush for text drawn into the back buffer
        hr = pcRenderTarget ->CreateSolidColorBrush(
            detail_::DrawCfg<TInnerDxVer>::getColor( )
            , m_pcBackBufferTextBrush.ReleaseAndGetAddressOf()
        );
		CPtr< IDXGISurface > pcBackBuffer;
        // Get a surface in the swap chain
        hr = m_stDxCtx ->m_pcDxgiSwapChain->GetBuffer(
				0
				, IID_PPV_ARGS( pcBackBuffer.ReleaseAndGetAddressOf( ) )
			);

        // It is assumed that the window will change size.
        DXGI_SWAP_CHAIN_DESC stDxgiSwapChainDesc = {};
		// or IDXGISwapChain1::GetHwnd method (dxgi1_2.h)
        m_stDxCtx ->m_pcDxgiSwapChain->GetDesc(&stDxgiSwapChainDesc);
        FLOAT dpiX, dpiY;
		//pcD2DFactory ->GetDesktopDpi( &dpiX, &dpiY ); // @depr
        dpiX = dpiY = (FLOAT)::GetDpiForWindow(stDxgiSwapChainDesc.OutputWindow);
        D2D1_RENDER_TARGET_PROPERTIES props2 = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX,
            dpiY
        );

        // Create a D2D render target which can draw into the surface in the swap chain
        hr = pcD2DFactory ->CreateDxgiSurfaceRenderTarget(
				pcBackBuffer.Get( )
				, &props2
				, m_pcBackBufferRT.ReleaseAndGetAddressOf( )
			);

        m_llFrameTimeLast = stFrameTime.getHPCounter( );
        //!< CPU timer frequency. Timestamps-per-second
        m_llCpuTimerFreq = stFrameTime.getHPFrequency( );
    }
    void renderOnTarget() {
		int64_t llNow = stFrameTime.getHPCounter( );
        //!< CPU time between two `bgfx::frame` calls.
		int64_t llCpuTimeFrame = llNow - m_llFrameTimeLast;
		m_llFrameTimeLast = llNow;
    	const double toMsCpu = 1000.0 / m_llCpuTimerFreq;
    	const double frameMs = llCpuTimeFrame * toMsCpu;
    	stFrameTime.pushSample( static_cast<float>( frameMs ) );
        float fFps = 1000.0f / stFrameTime.m_avg;

        std::wstringstream stream;
        stream << L"FPS " << static_cast<int>( fFps );

        Sys::Hr hr;
        m_pcBackBufferRT ->BeginDraw( );
        m_pcBackBufferRT ->SetTransform( D2D1::Matrix3x2F::Identity( ) );
        // Text format object will center the text in layout
        m_pcBackBufferRT ->DrawTextW(
                stream.str( ).c_str( ), (UINT32)stream.str( ).length( ),
                m_pcTextFormat.Get( ),
                detail_::DrawCfg<TInnerDxVer>::getRect( ),
                m_pcBackBufferTextBrush.Get( )
            );
        hr = m_pcBackBufferRT->EndDraw( );
    }
};

template<> class Ordinary<DxVer::v12> {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;

	int64_t m_llFrameTimeLast, m_llCpuTimerFreq;
	detail_::FrameTime stFrameTime;

	uptr< Spec::D12::DescriptorHeap > m_resourceDescriptors;
	uptr< Spec::D12::SpriteFont > m_font;
	enum Descriptors {
		MyFont,
		Count
	};
	uptr< Spec::D12::SpriteBatch > m_spriteBatch;
	DirectX::XMFLOAT2 m_fontPos;

	CPtr< ID3D12GraphicsCommandList > m_pc_CommandList;

 public:
    typedef uptr< Ordinary > uptr_t;
    explicit Ordinary(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx)
        : m_stDxCtx( stDxCtx ) {

#pragma region TakeFont
		const auto &arrayFont = DxtkFont::CompiledToBinary::getArial28( );
		typedef std::remove_cv_t< std::remove_reference_t< decltype( arrayFont ) > > return_t;
		static_assert( std::is_array_v< return_t >, "expect only array" );
		static_assert( std::rank_v< return_t > == 1, "expect only one dimension array" );
		static_assert( std::extent_v< return_t > > 0, "expect only bounded array" );
#pragma endregion

		auto device = m_stDxCtx ->m_pcD3dDevice12.Get( );
		m_resourceDescriptors = std::make_unique< Spec::D12::DescriptorHeap >(
				device
				, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
				, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
				, Descriptors::Count
			);
		Spec::D12::ResourceUploadBatch resourceUpload( device );

		resourceUpload.Begin( );

		m_font = std::make_unique< Spec::D12::SpriteFont >(
				device
				, resourceUpload
				, arrayFont, sizeof( arrayFont )
				, m_resourceDescriptors ->GetCpuHandle( Descriptors::MyFont )
				, m_resourceDescriptors ->GetGpuHandle( Descriptors::MyFont )
			);

		Spec::D12::RenderTargetState rtState(
				//m_deviceResources ->GetBackBufferFormat( ), m_deviceResources ->GetDepthBufferFormat( )
				DXGI_FORMAT_R8G8B8A8_UNORM
				, DXGI_FORMAT_R8G8B8A8_UNORM
			);

		Spec::D12::SpriteBatchPipelineStateDescription pd( rtState );
		m_spriteBatch = std::make_unique< Spec::D12::SpriteBatch >( device, resourceUpload, pd );

		m_fontPos = detail_::DrawCfg<TInnerDxVer>::getRect( );

		auto uploadResourcesFinished = resourceUpload.End(
				m_stDxCtx ->m_pcCommandQueue.Get( )
			);
#pragma region FormattingFps
		uploadResourcesFinished.wait( );
        m_llFrameTimeLast = stFrameTime.getHPCounter( );
        m_llCpuTimerFreq = stFrameTime.getHPFrequency( );
#pragma endregion
    }

    void renderOnTarget() {
#pragma region FormattingFps
		int64_t llNow = stFrameTime.getHPCounter( );
        //!< CPU time between two `bgfx::frame` calls.
		int64_t llCpuTimeFrame = llNow - m_llFrameTimeLast;
		m_llFrameTimeLast = llNow;
    	const double toMsCpu = 1000.0 / m_llCpuTimerFreq;
    	const double frameMs = llCpuTimeFrame * toMsCpu;
    	stFrameTime.pushSample( static_cast<float>( frameMs ) );
        float fFps = 1000.0f / stFrameTime.m_avg;

        std::wstringstream stream;
        stream << L"FPS " << static_cast<int>( fFps );
#pragma endregion
		Sys::Hr hr;
		ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors ->Heap( ) };
		m_pc_CommandList ->SetDescriptorHeaps(
				static_cast< UINT >( std::size( heaps ) )
				, heaps
			);

		m_spriteBatch ->Begin( m_pc_CommandList.Get( ) );

		auto origin = DirectX::g_XMZero;
		auto scale = DirectX::XMVectorReplicate( 1 );
		auto effects = Spec::D12::SpriteEffects::SpriteEffects_None;
		float layerDepth = 0;
		m_font ->DrawString(
				m_spriteBatch.get( )
				, stream.str( ).c_str( )
				, XMLoadFloat2( &m_fontPos )
				, DirectX::Colors::White
				, 0.f
				, origin
				, scale
				, effects
				, layerDepth
			);

		m_spriteBatch ->End( );
	}

	void setDx12Viewport(const D3D12_VIEWPORT &stViewport) {
		m_spriteBatch ->SetViewport( stViewport );
	}
	void setDx12CommandList(const CPtr< ID3D12GraphicsCommandList > &pcCommandList) {
		m_pc_CommandList = pcCommandList;
	}
};
} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps
