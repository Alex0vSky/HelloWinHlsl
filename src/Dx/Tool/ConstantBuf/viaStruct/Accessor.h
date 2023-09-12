// Dx/Tool/ConstantBuf/viaStruct/Accessor.h - wrapper for create and map/unmap
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {

template<class T, class TConstBuf> class Accessor; // primary template

template<class TConstBuf>
class Accessor< DxVer::v10,TConstBuf > {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	struct Out_t {
		CPtr< ID3D10Buffer > m_pcConstantBuffer;
		const UINT c_uNumBuffers = 1;
		std::vector<ID3D10Buffer *> m_veConstantBuffers;
	} m_stOut;
	HolderMem< TConstBuf > m_oHolderAlignedMem;

 public:
	typedef uptr< Accessor > uptr_t;
	Accessor(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx, const TConstBuf &crst) 
		: m_stDxCtx( stDxCtx ) 
		, m_oHolderAlignedMem( crst ) 
	{ 
		D3D10_BUFFER_DESC cbDesc = { };
		cbDesc.ByteWidth = ( ( ( sizeof( crst ) / c_uDxAlign ) + 1 ) * c_uDxAlign );
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		D3D10_SUBRESOURCE_DATA InitData = { m_oHolderAlignedMem.getMem( ) };
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( &cbDesc, &InitData, m_stOut.m_pcConstantBuffer.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) ) 
			throw _com_error( E_OUTOFMEMORY );
		m_stOut.m_veConstantBuffers = { m_stOut.m_pcConstantBuffer.Get( ) };
	}

	auto getConstantBuf() {
		return std::make_unique< Out_t >( m_stOut );
	}

	bool passToShader(std::function< void(TConstBuf *) > clb) const {
		TConstBuf *pConstData;
		HRESULT hr = m_stOut.m_pcConstantBuffer ->Map( 
				D3D10_MAP_WRITE_NO_OVERWRITE
				, NULL
				, reinterpret_cast<void**>(&pConstData) 
			);
		if ( FAILED( hr ) ) 
			return false;
		clb( pConstData );
		m_stOut.m_pcConstantBuffer ->Unmap();
		return true;
	}
};

template<class TConstBuf>
class Accessor< DxVer::v11,TConstBuf > {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	struct Out_t {
		CPtr< ID3D11Buffer > m_pcConstantBuffer;
		const UINT c_uNumBuffers = 1;
		std::vector< ID3D11Buffer *> m_veConstantBuffers;
	} m_stOut;
	HolderMem< TConstBuf > m_oHolderAlignedMem;

 public:
	typedef uptr< Accessor > uptr_t;
	Accessor(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx, const TConstBuf &crst) 
		: m_stDxCtx( stDxCtx ) 
		, m_oHolderAlignedMem( crst ) 
	{ 
		D3D11_BUFFER_DESC cbDesc = { };
		cbDesc.ByteWidth = ( ( ( sizeof( crst ) / c_uDxAlign ) + 1 ) * c_uDxAlign );
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA InitData = { m_oHolderAlignedMem.getMem( ) };
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( &cbDesc, &InitData, m_stOut.m_pcConstantBuffer.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) ) 
			throw _com_error( E_OUTOFMEMORY );
		m_stOut.m_veConstantBuffers = { m_stOut.m_pcConstantBuffer.Get( ) };
	}

	auto getConstantBuf() {
		return std::make_unique< Out_t >( m_stOut );
	}

	bool passToShader(std::function< void(TConstBuf *) > clb) const {
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = m_stDxCtx ->m_pcDeviceContext ->Map( 
				m_stOut.m_pcConstantBuffer.Get( )
				, 0
				, D3D11_MAP_WRITE_NO_OVERWRITE
				, 0
				, &mappedSubresource
			);
		if ( FAILED( hr ) ) 
			return false;
		TConstBuf *pConstData = static_cast<TConstBuf *>( mappedSubresource.pData );
		clb( pConstData );
		m_stDxCtx ->m_pcDeviceContext ->Unmap( m_stOut.m_pcConstantBuffer.Get( ), 0 );
		return true;
	}
};

// Supported types: float(Cpp) ->float(Hlsl), DirectX::XMFLOAT2(Cpp) ->float2(Hlsl), DirectX::XMUINT2(Cpp) ->uint2(Hlsl)
template<class TConstBuf>
class Accessor< DxVer::v12,TConstBuf > {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	HolderMem< TConstBuf > m_oHolderAlignedMem;
	prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct::Converter m_converter;

 public:
	typedef uptr< Accessor > uptr_t;
	Accessor(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx, const TConstBuf &crst) 
		: m_stDxCtx( stDxCtx ) 
		, m_oHolderAlignedMem( crst ) 
	{}

#ifdef BOOST_PFR_ENABLED
	bool passToShader(const CPtr< ID3D12GraphicsCommandList > &pcCommandList, std::function< void(TConstBuf *) > clb = nullptr) {
		TConstBuf *pConstData = static_cast<TConstBuf *>( m_oHolderAlignedMem.getMem( ) );
		if ( clb )
			clb( pConstData );
		boost::pfr::for_each_field( *pConstData, [this](const auto& field) {
				using field_t = std::decay_t< decltype( field ) >;
				if constexpr ( false ) {}
				else if constexpr ( ( std::is_same_v< field_t, float > ) ) {
					m_converter.wonnaPassHlslType.likeFloat_( ) ->float_( field );
				}
				else if constexpr ( ( std::is_same_v< field_t, DirectX::XMFLOAT2 > ) ) {
					m_converter.wonnaPassHlslType.likeFloat_( ) ->float2_( { field.x, field.y } );
				}
				else if constexpr ( ( std::is_same_v< field_t, DirectX::XMUINT2 > ) ) {
					m_converter.wonnaPassHlslType.likeUint_( ) ->uint2_( { field.x, field.y } );
				}
				else 
					static_assert( Tpl::Trait::always_false_v< field_t >, "unsupport type" );
			} );
		return m_converter.send( pcCommandList );
	}
#endif // BOOST_PFR_ENABLED
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
