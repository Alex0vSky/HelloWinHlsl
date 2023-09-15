// Dx/Tool/IndexBuf.h - Index Buffer creator
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool {
template<class T> class IndexBuf; // primary template

template<> class IndexBuf<DxVer::v9> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit IndexBuf(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	struct indexBuf_t {
		const CPtr< IDirect3DIndexBuffer9 > m_pcBuffer;
		const D3DFORMAT m_enuFormat;
	};

	// Supported formats: D3DFORMAT::D3DFMT_INDEX16
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMem) {
		D3DFORMAT enuFormat = D3DFORMAT::D3DFMT_UNKNOWN;
		if ( 2 == sizeof( TVE ) ) {
			if ( std::is_unsigned_v< TVE > )
				enuFormat = D3DFORMAT::D3DFMT_INDEX16;
		}
		if ( D3DFORMAT::D3DFMT_UNKNOWN == enuFormat )
			return uptr< indexBuf_t >{ }; // nullptr;

		CPtr< IDirect3DIndexBuffer9 > pcIndexBuf;
		size_t si = veMem.size( ) * sizeof( TVE );
		DWORD dwUsage = 0;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateIndexBuffer( 
				si
                , dwUsage, enuFormat
                , D3DPOOL_DEFAULT, pcIndexBuf.ReleaseAndGetAddressOf( ), nullptr
			);
		if ( FAILED( hr ) )
			return uptr< indexBuf_t >{ }; // nullptr;
		void *pVertices;
		hr = pcIndexBuf ->Lock( 0, si, &pVertices, 0 );
		if ( FAILED( hr ) )
			return uptr< indexBuf_t >{ }; // nullptr;
		memcpy( pVertices, &veMem[ 0 ], si );
		pcIndexBuf ->Unlock();

		indexBuf_t stBuf{ 
				pcIndexBuf 
				, enuFormat
			};
		return std::make_unique< indexBuf_t >( stBuf );
	}
};

template<> class IndexBuf<DxVer::v10> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit IndexBuf(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	struct indexBuf_t {
		const CPtr< ID3D10Buffer > m_pcBuffer;
		const DXGI_FORMAT m_enuDxgiFormat;
	};	

	// Supported formats: DXGI_FORMAT_R16_UINT and DXGI_FORMAT_R16_SINT
	// using D3D10_BIND_INDEX_BUFFER flag
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMemNdcLH) {
		DXGI_FORMAT enuDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		if ( 2 == sizeof( TVE ) ) {
			if ( std::is_unsigned_v< TVE > )
				enuDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
			else 
				enuDxgiFormat = DXGI_FORMAT::DXGI_FORMAT_R16_SINT;
		}
		if ( DXGI_FORMAT::DXGI_FORMAT_UNKNOWN == enuDxgiFormat )
			return uptr< indexBuf_t >{ }; // nullptr;
		D3D10_SUBRESOURCE_DATA stInitData = { &veMemNdcLH[0] };
		D3D10_BUFFER_DESC stBufferDesc = { };
		stBufferDesc.ByteWidth = sizeof( TVE ) * veMemNdcLH.size( );
		stBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		stBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		CPtr< ID3D10Buffer > pcIndexBuf;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( 
			&stBufferDesc, &stInitData, pcIndexBuf.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return uptr< indexBuf_t >{ }; // nullptr;
		indexBuf_t stBuf{ 
				pcIndexBuf 
				, enuDxgiFormat
			};
		return std::make_unique< indexBuf_t >( stBuf );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool
