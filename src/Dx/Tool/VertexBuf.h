// Dx/Tool/VertexBuf.h - Vertex Buffer creator
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool {

template<class TSPE,class T> class VertexBuf; // primary template

template<class T2> class VertexBuf<DxVer::v9,T2> {
	using TInnerDxVer = DxVer::v9; // IntelliSense
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
public:
	typedef uptr< VertexBuf > uptr_t;
	explicit VertexBuf(
		Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
	) 
		: m_stDxCtx( stDxCtx )
	{}

	struct OutVertexBuf {
		const CPtr< IDirect3DVertexBuffer9 > m_pcBuffer;
		const DWORD m_dwFVF;
		const UINT m_uStride;
	};

	// Coordinate system handednesses is LH(left-handed)
	// Position/coordinate values in Ndc(Normalized device coordinate)
	// With D3DPOOL::D3DPOOL_DEFAULT and FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMemNdcLH, bool bNdc = true) {
		// TODO: std::enable_if_v< is_same_t< TVE, Tool::Vertex<TInnerDxVer> > >

		// From Ndc LH to Orth LH. @insp https://stackoverflow.com/questions/19226156/directx-11-vertices-coordinates @insp https://stackoverflow.com/questions/42751427/transformations-from-pixels-to-ndc
		D3DVIEWPORT9 stViewport = { };
		m_stDxCtx ->m_pcD3dDevice ->GetViewport( &stViewport );
		std::vector< TVE > veMemOrthLH = veMemNdcLH;
		if ( bNdc ) 
			for ( TVE &el : veMemOrthLH )
				el.xyz.x = ( ( stViewport.Width * ( 1 + el.xyz.x ) / 2.0f ) ), el.xyz.y = ( ( stViewport.Height * ( 1 - el.xyz.y ) / 2.0f ) );

		CPtr< IDirect3DVertexBuffer9 > cpVertexBuf;
		size_t si = veMemOrthLH.size( ) * sizeof( TVE );
		DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
		DWORD dwUsage = 0;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexBuffer( 
				(UINT)si
                , dwUsage, dwFVF
                , D3DPOOL_DEFAULT, cpVertexBuf.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;
		void *pVertices;
		hr = cpVertexBuf ->Lock( 0, (UINT)si, &pVertices, 0 );
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;
		memcpy( pVertices, &veMemOrthLH[ 0 ], si );
		cpVertexBuf ->Unlock();

		OutVertexBuf stBuf{ 
				cpVertexBuf 
				, dwFVF
				, sizeof( TVE )
			};
		return std::make_unique< OutVertexBuf >( stBuf );
	}
};

template<class T> class VertexBuf<DxVer::v10,T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
public:
	typedef uptr< VertexBuf > uptr_t;
	explicit VertexBuf(
		Ty::StDxCtx_crefPtr<T> stDxCtx
	) 
		: m_stDxCtx( stDxCtx )
	{}

	struct OutVertexBuf {
		static const UINT c_uNumBuffers = 1;
		const CPtr< ID3D10Buffer > m_pcBuffer;
		const std::vector< ID3D10Buffer *> m_veBuffers;
		const std::vector< UINT > m_veStride;
		const std::vector< UINT > m_veOffset;
	};	

	// Coordinate system handednesses is LH(left-handed)
	// Position/coordinate values in Ndc(Normalized device coordinate)
	// using D3D10_BIND_VERTEX_BUFFER flag
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMemNdcLH) {
		D3D10_SUBRESOURCE_DATA stInitData = { &veMemNdcLH[0] };
		D3D10_BUFFER_DESC stBufferDesc = { };
		stBufferDesc.ByteWidth = (UINT)( sizeof( TVE ) * veMemNdcLH.size( ) );
		stBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		stBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		CPtr< ID3D10Buffer > cpVertexBuf;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( &stBufferDesc, &stInitData, cpVertexBuf.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;
		OutVertexBuf stBuf{ 
				cpVertexBuf 
				, { cpVertexBuf.Get( ) }
				, { sizeof( TVE ) }
				, { 0 }
			};
		return std::make_unique< OutVertexBuf >( stBuf );
	}
};

template<class T> class VertexBuf<DxVer::v11,T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
public:
	typedef uptr< VertexBuf > uptr_t;
	explicit VertexBuf(
		Ty::StDxCtx_crefPtr<T> stDxCtx
	) 
		: m_stDxCtx( stDxCtx )
	{}

	struct OutVertexBuf {
		static const UINT c_uNumBuffers = 1;
		const CPtr< ID3D11Buffer > m_pcBuffer;
		const std::vector< ID3D11Buffer *> m_veBuffers;
		const std::vector< UINT > m_veStride;
		const std::vector< UINT > m_veOffset;
	};	

	// Coordinate system handednesses is LH(left-handed)
	// Position/coordinate values in Ndc(Normalized device coordinate)
	// using D3D10_BIND_VERTEX_BUFFER flag
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMemNdcLH) {
		D3D11_SUBRESOURCE_DATA stInitData = { &veMemNdcLH[0] };
		D3D11_BUFFER_DESC stBufferDesc = { };
		stBufferDesc.ByteWidth = (UINT)( sizeof( TVE ) * veMemNdcLH.size( ) );
		stBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		stBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		CPtr< ID3D11Buffer > cpVertexBuf;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateBuffer( &stBufferDesc, &stInitData, cpVertexBuf.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;
		OutVertexBuf stBuf{ 
				cpVertexBuf 
				, { cpVertexBuf.Get( ) }
				, { sizeof( TVE ) }
				, { 0 }
			};
		return std::make_unique< OutVertexBuf >( stBuf );
	}
};

template<class T> class VertexBuf<DxVer::v12,T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
public:
	typedef uptr< VertexBuf > uptr_t;
	explicit VertexBuf(
		Ty::StDxCtx_crefPtr<T> stDxCtx
	) 
		: m_stDxCtx( stDxCtx )
	{}

	struct OutVertexBuf {
		static const UINT c_uNumBuffers = 1;
		const CPtr< ID3D12Resource > m_pcBuffer;
		const D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};	

	// Coordinate system handednesses is LH(left-handed)
	// Position/coordinate values in Ndc(Normalized device coordinate)
	template<class TVE>
	auto createInitialized(const std::vector< TVE > &veMemNdcLH) {

		HRESULT hr;
		CPtr< ID3D12Resource > vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		const UINT vertexBufferSize = (UINT)Tpl::Trait::vectorsizeof( veMemNdcLH );
		CD3DX12_HEAP_PROPERTIES stHeapProp( D3D12_HEAP_TYPE_UPLOAD );
		CD3DX12_RESOURCE_DESC stResDesc = CD3DX12_RESOURCE_DESC::Buffer( vertexBufferSize, D3D12_RESOURCE_FLAG_NONE );
		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		hr = m_stDxCtx ->m_pcD3dDevice12 ->CreateCommittedResource(
				&stHeapProp
				, D3D12_HEAP_FLAG_NONE // ? NVIDIA man D3D12_HEAP_FLAG_CREATE_NOT_ZEROED
				, &stResDesc
				, D3D12_RESOURCE_STATE_GENERIC_READ
				, nullptr
				, IID_PPV_ARGS( vertexBuffer.ReleaseAndGetAddressOf( ) )
			);
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange( 0, 0 );        // We do not intend to read from this resource on the CPU.
		hr = vertexBuffer ->Map( 0, &readRange, reinterpret_cast<void**>( &pVertexDataBegin ) );
		if ( FAILED( hr ) )
			return uptr< OutVertexBuf >{ }; // nullptr;
		memcpy( pVertexDataBegin, veMemNdcLH.data( ), Tpl::Trait::vectorsizeof( veMemNdcLH ) );
		vertexBuffer ->Unmap( 0, nullptr );

		// Initialize the vertex buffer view.
		vertexBufferView.BufferLocation = vertexBuffer ->GetGPUVirtualAddress( );
		vertexBufferView.StrideInBytes = sizeof( TVE );
		vertexBufferView.SizeInBytes = vertexBufferSize;

		OutVertexBuf stBuf{ 
				vertexBuffer
				, vertexBufferView
			};
		return std::make_unique< OutVertexBuf >( stBuf );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool
