// Dx/AdjustAux.h - setting auxiliary DirectX features via callback
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// TODO(Alex0vSky): avoid need create spectial data struct for client
// Fwd decl for friend-s
namespace prj_3d::HelloWinHlsl::GameLoop {
namespace detail_ { template<class T, class TWndProcHolder> class ABase; }
} // namespace prj_3d::HelloWinHlsl::GameLoop
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 { 
class GameLoopPart;
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12

namespace prj_3d::HelloWinHlsl::Dx { 

template<class T> class AdjustAux; // primary template

template<> class AdjustAux<DxVer::v9> { 
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_psstDxCtx;

 public:
    typedef uptr< AdjustAux > uptr_t;
	explicit AdjustAux(Ty::StDxCtx_crefPtr<TInnerDxVer> psstDxCtx) 
		: m_psstDxCtx( psstDxCtx )
	 {}

	// ?TODO(Alex0vSky): make me?
	void adjust() {
	}
};
template<> class AdjustAux<DxVer::v10> { 
	using TInnerDxVer = DxVer::v10;

	const Ty::StDxCtx_ptr<TInnerDxVer> m_psstDxCtx;

	// TODO(Alex0vSky): store in CPtr, because user can release memory
	//	@use C:\Prj\3d\HelloShader\poc\withoutEffects\main.cpp
	ID3D10RasterizerState** m_pifcRasterizerStateUser;
	std::function< void (D3D10_RASTERIZER_DESC *) > m_pfnRasterizerState;

	ID3D10DepthStencilState** m_pifcDepthStencilStateUser;
	std::function< void (D3D10_DEPTH_STENCIL_DESC *) > m_pfnDepthStencilState;

	ID3D10BlendState** m_pifcBlendStateUser;
	std::function< void (D3D10_BLEND_DESC *) > m_pfnBlendState;

	ID3D10DepthStencilView** m_pifcID3D10DepthStencilViewUser;
	std::function< void (D3D10_TEXTURE2D_DESC *, D3D10_DEPTH_STENCIL_VIEW_DESC *) > m_pfnDepthStencilView;
	
 public:
    typedef uptr< AdjustAux > uptr_t;
	explicit AdjustAux(Ty::StDxCtx_crefPtr<TInnerDxVer> psstDxCtx) 
		: m_psstDxCtx( psstDxCtx )
		, m_pifcRasterizerStateUser( nullptr )
		, m_pifcDepthStencilStateUser( nullptr )
		, m_pifcBlendStateUser( nullptr )
		, m_pifcID3D10DepthStencilViewUser( nullptr )
     {}

	// Why "decltype" and heavy function type -- 
// stackoverflow.com/questions/28509273/get-types-of-c-function-parameters
	void setRasterizerState(ID3D10RasterizerState** pifc, decltype(m_pfnRasterizerState) pfn = nullptr) {
		m_pifcRasterizerStateUser = pifc;
		m_pfnRasterizerState = pfn;
	}
	void setDepthStencilState(ID3D10DepthStencilState** pifc, decltype(m_pfnDepthStencilState) pfn = nullptr) {
		m_pifcDepthStencilStateUser = pifc;
		m_pfnDepthStencilState = pfn;
	}
	void setBlendState(ID3D10BlendState** pifc, decltype(m_pfnBlendState) pfn = nullptr) {
		m_pifcBlendStateUser = pifc;
		m_pfnBlendState = pfn;
	}
	void setDepthStencilView(ID3D10DepthStencilView** pifc, decltype(m_pfnDepthStencilView) pfn = nullptr) {
		m_pifcID3D10DepthStencilViewUser = pifc;
		m_pfnDepthStencilView = pfn;
	}
	// TODO(Alex0vSky): can take values ​​like default "DXUT" from 
	//	"DXUTSetupD3D10Views" code. Now most of '...\HLSLWithoutFX10\HLSLWithoutFX10.cpp'.
	void adjust() {
        Sys::Hr hr;
		// Set up rasterizer
		if ( m_pifcRasterizerStateUser ) {
			// TODO(Alex0vSky): you need to make your own "memset", because it is "{ 0 }",
			//	does not guarantee nulling.
			// @insp https://stackoverflow.com/questions/61240589/how-to-initialize-a-struct-to-0-in-c
			D3D10_RASTERIZER_DESC stD3dRasterizerDesc = { };
			stD3dRasterizerDesc.AntialiasedLineEnable = FALSE;
			stD3dRasterizerDesc.CullMode = D3D10_CULL_NONE;
			stD3dRasterizerDesc.DepthBias = 0;
			stD3dRasterizerDesc.DepthBiasClamp = 0;
			stD3dRasterizerDesc.DepthClipEnable = TRUE;
			stD3dRasterizerDesc.FillMode = D3D10_FILL_SOLID;
			stD3dRasterizerDesc.FrontCounterClockwise = FALSE; // Must be FALSE for 10on9
			stD3dRasterizerDesc.MultisampleEnable = FALSE;
			stD3dRasterizerDesc.ScissorEnable = FALSE;
			stD3dRasterizerDesc.SlopeScaledDepthBias = 0;
			if ( m_pfnRasterizerState ) 
				m_pfnRasterizerState( &stD3dRasterizerDesc );
			CPtr<ID3D10RasterizerState> pcRasterizerStateDef;
			hr = m_psstDxCtx ->m_pcD3dDevice ->CreateRasterizerState( 
				&stD3dRasterizerDesc, pcRasterizerStateDef.ReleaseAndGetAddressOf( ) );
			*m_pifcRasterizerStateUser = pcRasterizerStateDef.Detach( );
		}
		// Create a depth stencil state to enable less-equal depth testing
		if ( m_pifcDepthStencilStateUser ) {
			D3D10_DEPTH_STENCIL_DESC stDSDesc = { };
			stDSDesc.DepthEnable = TRUE;
			stDSDesc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
			stDSDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
			if ( m_pfnDepthStencilState ) 
				m_pfnDepthStencilState( &stDSDesc );
			CPtr<ID3D10DepthStencilState> pcLessEqualDepth;
			hr = m_psstDxCtx ->m_pcD3dDevice ->CreateDepthStencilState( 
				&stDSDesc, pcLessEqualDepth.ReleaseAndGetAddressOf( ) );
			*m_pifcDepthStencilStateUser = pcLessEqualDepth.Detach( );
		}
		// Create a blend state to disable alpha blending
		if ( m_pifcBlendStateUser ) {
			D3D10_BLEND_DESC stBlendState = { };
			stBlendState.BlendEnable[0] = FALSE;
			stBlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
			if ( m_pfnBlendState ) 
				m_pfnBlendState( &stBlendState );
			CPtr<ID3D10BlendState> pcBlendStateNoBlend;
			hr = m_psstDxCtx ->m_pcD3dDevice ->CreateBlendState( 
				&stBlendState, pcBlendStateNoBlend.ReleaseAndGetAddressOf( ) );
			*m_pifcBlendStateUser = pcBlendStateNoBlend.Detach( );
		}

		// Sets the viewport, creates a render target view, and depth scencil texture and view. 
		//	@insp HRESULT DXUTSetupD3D10Views(ID3D10Device*, DXUTDeviceSettings*)
		if ( m_pifcID3D10DepthStencilViewUser ) {
			// Get the back buffer and desc
			D3D10_TEXTURE2D_DESC stBackBufferSurfaceDesc;
			 {
				CPtr<ID3D10Texture2D> pBackBuffer;
				hr = m_psstDxCtx ->m_pcDxgiSwapChain ->GetBuffer( 
						0
						, IID_PPV_ARGS( pBackBuffer.ReleaseAndGetAddressOf( ) )
					);
				pBackBuffer->GetDesc( &stBackBufferSurfaceDesc );
			}
			// Create depth stencil texture
			CPtr<ID3D10Texture2D> pcDepthStencil;
			D3D10_TEXTURE2D_DESC stTex2dDesc = { };
			stTex2dDesc.Width = stBackBufferSurfaceDesc.Width;
			stTex2dDesc.Height = stBackBufferSurfaceDesc.Height;
			stTex2dDesc.MipLevels = 1;
			stTex2dDesc.ArraySize = 1;
			// @insp ...\DXUT\Core\DXUTenum.cpp. DXUTBuildOptimalD3D10DeviceSettings() -> 
			//	DXUTBuildValidD3D10DeviceSettings() ->bestMultiSampleCount
			stTex2dDesc.Format = DXGI_FORMAT_D32_FLOAT; // pDeviceSettings->d3d10.AutoDepthStencilFormat;
			stTex2dDesc.SampleDesc.Count = 1; // pDeviceSettings->d3d10.sd.SampleDesc.Count;
			stTex2dDesc.SampleDesc.Quality = 0; // pDeviceSettings->d3d10.sd.SampleDesc.Quality;
			stTex2dDesc.Usage = D3D10_USAGE_DEFAULT;
			stTex2dDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
			stTex2dDesc.CPUAccessFlags = 0;
			stTex2dDesc.MiscFlags = 0;

			// Create the depth stencil view
			D3D10_DEPTH_STENCIL_VIEW_DESC stDsvDesc = { };
			stDsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			stDsvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
			stDsvDesc.Texture2D.MipSlice = 0;

			if ( m_pfnDepthStencilView ) 
				m_pfnDepthStencilView( &stTex2dDesc, &stDsvDesc );
			hr = m_psstDxCtx ->m_pcD3dDevice ->CreateTexture2D( 
				&stTex2dDesc, NULL, pcDepthStencil.ReleaseAndGetAddressOf( ) );
			CPtr<ID3D10DepthStencilView> pcDSV;
			hr = m_psstDxCtx ->m_pcD3dDevice ->CreateDepthStencilView( 
				pcDepthStencil.Get( ), &stDsvDesc, pcDSV.ReleaseAndGetAddressOf( ) );
			*m_pifcID3D10DepthStencilViewUser = pcDSV.Detach( );
		}
	}
};
// TODO(Alex0vSky): cleanme
template<> class AdjustAux<DxVer::v11> { 
	using TInnerDxVer = DxVer::v11;

	const Ty::StDxCtx_ptr<TInnerDxVer> m_psstDxCtx;

	// TODO(Alex0vSky): store in CPtr, because user can release memory
	//	@use C:\Prj\3d\HelloShader\poc\withoutEffects\main.cpp
	ID3D10RasterizerState** m_pifcRasterizerStateUser;
	std::function< void (D3D10_RASTERIZER_DESC *) > m_pfnRasterizerState;

	ID3D10DepthStencilState** m_pifcDepthStencilStateUser;
	std::function< void (D3D10_DEPTH_STENCIL_DESC *) > m_pfnDepthStencilState;

	ID3D10BlendState** m_pifcBlendStateUser;
	std::function< void (D3D10_BLEND_DESC *) > m_pfnBlendState;

	ID3D10DepthStencilView** m_pifcID3D10DepthStencilViewUser;
	std::function
		< void (D3D10_TEXTURE2D_DESC *, D3D10_DEPTH_STENCIL_VIEW_DESC *) > 
		m_pfnDepthStencilView;
	
 public:
    typedef uptr< AdjustAux > uptr_t;
	explicit AdjustAux(Ty::StDxCtx_crefPtr<TInnerDxVer> psstDxCtx) 
		: m_psstDxCtx( psstDxCtx )
		, m_pifcRasterizerStateUser( nullptr )
		, m_pifcDepthStencilStateUser( nullptr )
		, m_pifcBlendStateUser( nullptr )
		, m_pifcID3D10DepthStencilViewUser( nullptr )
	 {}

	// Why "decltype" and heavy function type -- 
// stackoverflow.com/questions/28509273/get-types-of-c-function-parameters
	void setRasterizerState(ID3D10RasterizerState** pifc, decltype(m_pfnRasterizerState) pfn = nullptr) {
		m_pifcRasterizerStateUser = pifc;
		m_pfnRasterizerState = pfn;
	}
	void setDepthStencilState(
		ID3D10DepthStencilState** pifc, decltype(m_pfnDepthStencilState) pfn = nullptr
	) {
		m_pifcDepthStencilStateUser = pifc;
		m_pfnDepthStencilState = pfn;
	}
	void setBlendState(ID3D10BlendState** pifc, decltype(m_pfnBlendState) pfn = nullptr) {
		m_pifcBlendStateUser = pifc;
		m_pfnBlendState = pfn;
	}
	void setDepthStencilView(ID3D10DepthStencilView** pifc, decltype(m_pfnDepthStencilView) pfn = nullptr) {
		m_pifcID3D10DepthStencilViewUser = pifc;
		m_pfnDepthStencilView = pfn;
	}
	void adjust() {
	}
};
template<> class AdjustAux<DxVer::v12> { 
	using TInnerDxVer = DxVer::v12;

	const Ty::StDxCtx_ptr<TInnerDxVer> m_psstDxCtx;

	std::function< void (D3D12_GRAPHICS_PIPELINE_STATE_DESC *) > m_pfnChangePso;

	Dx::Spec::D12::Waiter *m_pWaiter;
	Dx::Spec::D12::SetterRootSignature m_setterRootSignature;
	CPtr< ID3D12PipelineState > m_pcPipelineState;
	CPtr< ID3D12GraphicsCommandList > m_pc_CommandList;

 public:
    typedef uptr< AdjustAux > uptr_t;
	AdjustAux(
			Ty::StDxCtx_crefPtr<TInnerDxVer> psstDxCtx
			, Dx::Spec::D12::Waiter *psoWaiter
			, Dx::Spec::D12::SetterRootSignature::cref_t setterRootSignature
		) 
		: m_psstDxCtx( psstDxCtx )
		, m_pWaiter( psoWaiter )
		, m_setterRootSignature( setterRootSignature )
     {}

	void onSetPso(decltype( m_pfnChangePso ) pfn) {
		m_pfnChangePso = pfn;
	}

 private:
	friend class GameLoop::detail_::ABase< DxVer::v12, Dx::Spec::Any::WindowResizerStub >;
	friend class GameLoop::detail_::ABase< DxVer::v12, Dx::Spec::D12::WindowResizer >;
	void adjust() {
        Sys::Hr hr;
		// part2 Create the pipeline state, which includes compiling and loading shaders.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = m_psstDxCtx ->m_psoDesc;
		if ( m_pfnChangePso ) {
			m_pfnChangePso( &psoDesc );
			if ( m_psstDxCtx ->m_initialRootSignature.Get( ) != psoDesc.pRootSignature ) {
				m_setterRootSignature.set( psoDesc.pRootSignature );
			}
		}

		hr = m_psstDxCtx ->m_pcD3dDevice12 ->CreateGraphicsPipelineState( 
				&psoDesc
				, IID_PPV_ARGS( m_pcPipelineState.ReleaseAndGetAddressOf( ) )
			);

		// Create the command list.
		hr = m_psstDxCtx ->m_pcD3dDevice12 ->CreateCommandList( 
				0
				, D3D12_COMMAND_LIST_TYPE_DIRECT
				, m_psstDxCtx ->m_arpcCommandAllocators[ m_pWaiter ->get_frameIndex( ) ].Get( )
				, m_pcPipelineState.Get( )
				, IID_PPV_ARGS( m_pc_CommandList.ReleaseAndGetAddressOf( ) )
			);
		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		hr = m_pc_CommandList ->Close( );

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		m_pWaiter ->waitForGpu( );
	}

 private:
	friend class Dx::Spec::D12::GameLoopPart;
	auto getPipelineState() {
		return m_pcPipelineState;
	}
	auto getCommandList() {
		return m_pc_CommandList;
	}
};

namespace detail_ {
template<class T, typename... Args> static auto z_make_unique(Args&&... args) {
	return std::make_unique< Tpl::Trait::aggregate_adapter< AdjustAux< T > > >( 
		std::forward<Args>(args)... );
}
} // namespace detail_

// Factory of AdjustAux. To fork call ctors. primary template, 
//	"typename = void" for partial specialization
template<class T, typename = void> struct FactoryAdjustAux;

// the partial specialization other Dx
template<class T> 
struct FactoryAdjustAux<T> {
	static
	auto create(Ty::StDxCtx_crefPtr<T> pustDxCtx) {
		return detail_::z_make_unique<T>( pustDxCtx );
	}
};

// the full specialization Dx12
template<> struct FactoryAdjustAux<DxVer::v12>  { 
	static
	auto create(
		Ty::StDxCtx_crefPtr<DxVer::v12> pustDxCtx
		, Dx::Spec::D12::Waiter *psoWaiter
		, Dx::Spec::D12::SetterRootSignature::cref_t setterRootSignature
	) {
		return detail_::z_make_unique<DxVer::v12>( pustDxCtx, psoWaiter, setterRootSignature );
	}
};

} // namespace prj_3d::HelloWinHlsl::Dx
