// Dx/Tool/ConstantBuf/viaStruct/Accessor.h - wrapper for create and map/unmap
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {

template<class T, class TConstBuf> class Accessor; // primary template

template<class TConstBuf>
class Accessor< DxVer::v10, TConstBuf > {
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
		, m_oHolderAlignedMem( crst ) { 
		D3D10_BUFFER_DESC cbDesc = { };
		cbDesc.ByteWidth = ( ( ( sizeof( crst ) / c_uDxAlign ) + 1 ) * c_uDxAlign );
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		D3D10_SUBRESOURCE_DATA InitData = { m_oHolderAlignedMem.getMem( ) };
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( 
			&cbDesc, &InitData, m_stOut.m_pcConstantBuffer.ReleaseAndGetAddressOf( ) );
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
class Accessor< DxVer::v11, TConstBuf > {
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
		, m_oHolderAlignedMem( crst ) { 
		D3D11_BUFFER_DESC cbDesc = { };
		cbDesc.ByteWidth = ( ( ( sizeof( crst ) / c_uDxAlign ) + 1 ) * c_uDxAlign );
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA InitData = { m_oHolderAlignedMem.getMem( ) };
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( 
			&cbDesc, &InitData, m_stOut.m_pcConstantBuffer.ReleaseAndGetAddressOf( ) );
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

template<class TConstBuf>
class Accessor< DxVer::v12, TConstBuf > {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	HolderMem< TConstBuf > m_oHolderAlignedMem;
	prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct::Converter m_converter;

	// this is a pointer to the memory location we get when we map our constant buffer
	UINT8* cbColorMultiplierGPUAddress[ Ty::StDxCtx_ptr<TInnerDxVer>::element_type::FrameCount ];

 public:
	typedef uptr< Accessor > uptr_t;
	Accessor(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx, const TConstBuf &crst) 
		: m_stDxCtx( stDxCtx ) 
		, m_oHolderAlignedMem( crst ) {
		Sys::Hr hr;
		for ( UINT n = 0; n < m_stDxCtx ->FrameCount; n++ ) {
			D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc = { };
			CBVDesc.BufferLocation = m_stDxCtx ->m_arpcResUploadHeap[ n ] ->GetGPUVirtualAddress( );
			// TODO: align to separate class // CB size is required to be 256-byte aligned.
			CBVDesc.SizeInBytes = ( sizeof( TConstBuf ) + 255 ) & ~255;
			m_stDxCtx ->m_pcD3dDevice12 ->CreateConstantBufferView(
					&CBVDesc
					, m_stDxCtx ->m_arpcDescriptorHeap[ n ]->GetCPUDescriptorHandleForHeapStart( )
				);
			// We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
			CD3DX12_RANGE readRange( 0, 0 );
			UINT Subresource = 0;
			hr = m_stDxCtx ->m_arpcResUploadHeap[ n ] ->Map(
					Subresource
					, &readRange
					, reinterpret_cast<void**>( &cbColorMultiplierGPUAddress[ n ] )
				);
			memcpy( 
					cbColorMultiplierGPUAddress[ n ]
					, m_oHolderAlignedMem.getMem( )
					, sizeof( TConstBuf )
				);
		}
	}

	bool passToShader(
		UINT frameIndex
		, std::function< void(TConstBuf *) > clb = nullptr
	) {
		TConstBuf *pConstData = static_cast<TConstBuf *>( m_oHolderAlignedMem.getMem( ) );
		if ( clb )
			clb( pConstData );
		memcpy( 
				cbColorMultiplierGPUAddress[ frameIndex ]
				, m_oHolderAlignedMem.getMem( )
				, sizeof( TConstBuf )
			);
		return true;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
