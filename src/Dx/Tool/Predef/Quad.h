// Dx/Tool/Predef/Quad.h - helper for creating a square of vertices for shaders to work there.
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::Predef {

template<class T> class Quad; // primary template

template<> class Quad<DxVer::v9> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	const sptr< Tool::VertexBuf<TInnerDxVer> > m_psoVertexBuf;
public:
	Quad(
		Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
		, const sptr< Tool::VertexBuf<TInnerDxVer> > &psoVertexBuf
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoVertexBuf( psoVertexBuf )
	{}

	struct vertexBufPrimitive_t {
		VertexBuf<TInnerDxVer>::OutVertexBuf m_stVertexBuf;
		static const Tool::Vertex<TInnerDxVer> typeForLayout;
		const D3DPRIMITIVETYPE m_enuPrimitiveType;
		const UINT m_uStartVertex;
		const UINT m_uPrimitiveCount;
		const UINT m_uStride;
	};
	auto createVertexBuf() {
		static const D3DCOLOR colorCyan = D3DCOLOR_XRGB( 0, 255, 255 ); // LightSaturatedCyan
		static const D3DCOLOR colorRed = D3DCOLOR_XRGB( 255, 0, 0 );
		static const D3DCOLOR colorGreen = D3DCOLOR_XRGB( 0, 255, 0 );
		static const D3DCOLOR colorYellow = D3DCOLOR_XRGB( 255, 255, 0 );
		float z = 0;
		float rhw = 1.0f;
		// Fill vertex buffer
		std::vector< Tool::Vertex<TInnerDxVer> > veMem = {
				{ D3DXVECTOR3( -1, -1, z ),	rhw,	colorCyan	},	// bottom left corner
				{ D3DXVECTOR3( -1, +1, z ),	rhw,	colorRed	},	// top left corner
				{ D3DXVECTOR3( +1, -1, z ),	rhw,	colorGreen	},	// bottom right corner
				{ D3DXVECTOR3( +1, +1, z ),	rhw,	colorYellow	},	// top right corner
			};
		auto pust = m_psoVertexBuf ->createInitialized( veMem );
		if ( !pust )
			return uptr< vertexBufPrimitive_t >{ }; // nullptr;
		
		vertexBufPrimitive_t stBuf{
				VertexBuf<TInnerDxVer>::OutVertexBuf{
					pust ->m_pcBuffer
					, pust ->m_dwFVF
				}
				, D3DPT_TRIANGLESTRIP
				, 0
				, (UINT)veMem.size( ) - 1
				, (UINT)sizeof( Vertex<TInnerDxVer> )
			};
		return std::make_unique< vertexBufPrimitive_t >( stBuf );
	}
};

template<> class Quad<DxVer::v10> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	const sptr< Tool::VertexBuf<TInnerDxVer> > m_psoVertexBuf;
public:
	Quad(
		Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
		, const sptr< Tool::VertexBuf<TInnerDxVer> > &psoVertexBuf
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoVertexBuf( psoVertexBuf )
	{}

	CPtr< ID3D10InputLayout > createLayout(const std::vector<char> &veShaderByte) {
		CPtr< ID3D10InputLayout > cpLayout;
		D3D10_INPUT_ELEMENT_DESC stLayout[] = { 
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
			};
		m_stDxCtx ->m_pcD3dDevice ->CreateInputLayout( stLayout, _countof( stLayout ), &veShaderByte[ 0 ], veShaderByte.size( ), cpLayout.ReleaseAndGetAddressOf( ) );
		return cpLayout;
	}
	struct vertexBufTopology_t {
		VertexBuf<TInnerDxVer>::OutVertexBuf m_stVertexBuf;
		D3D_PRIMITIVE_TOPOLOGY m_enuTopology;
	};	
	auto createVertexBuf() {
		// TODO: common data to base class, now its common sure
		// TODO: take type from Tool::VertexBuf<TInnerDxVer>:: ...
		// Fill vertex buffer. Enough "D3DXVECTOR3", expects "D3DXVECTOR4", definition goes by "stride" in "IASetVertexBuffers".
		std::vector< D3DXVECTOR3 > veMem = {
				  D3DXVECTOR3( -1, -1, 0 ) // bottom left corner
				, D3DXVECTOR3( -1, +1, 0 ) // top left corner
				, D3DXVECTOR3( +1, -1, 0 ) // bottom right corner
				, D3DXVECTOR3( +1, +1, 0 ) // top right corner
			};
		auto pust = m_psoVertexBuf ->createInitialized( veMem );
		if ( !pust )
			return uptr< vertexBufTopology_t >{ }; // nullptr;
		vertexBufTopology_t stBuf{
				VertexBuf<TInnerDxVer>::OutVertexBuf{
					pust ->m_pcBuffer
					, pust ->m_veBuffers
					, pust ->m_veStride
					, pust ->m_veOffset
				}
				, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			};
		return std::make_unique< vertexBufTopology_t >( stBuf );
	}
};

template<> class Quad<DxVer::v11> {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	const sptr< Tool::VertexBuf<TInnerDxVer> > m_psoVertexBuf;
public:
	Quad(
		Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
		, const sptr< Tool::VertexBuf<TInnerDxVer> > &psoVertexBuf
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoVertexBuf( psoVertexBuf )
	{}

	CPtr< ID3D11InputLayout > createLayout(const std::vector<char> &veShaderByte) {
		CPtr< ID3D11InputLayout > cpLayout;
		D3D11_INPUT_ELEMENT_DESC stLayout[] = { 
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
		m_stDxCtx ->m_pcD3dDevice ->CreateInputLayout( stLayout, _countof( stLayout ), &veShaderByte[ 0 ], veShaderByte.size( ), cpLayout.ReleaseAndGetAddressOf( ) );
		return cpLayout;
	}
	struct vertexBufTopology_t {
		VertexBuf<TInnerDxVer>::OutVertexBuf m_stVertexBuf;
		D3D_PRIMITIVE_TOPOLOGY m_enuTopology;
	};	
	auto createVertexBuf() {
		// Fill vertex buffer. Enough "D3DXVECTOR3", expects "D3DXVECTOR4", definition goes by "stride" in "IASetVertexBuffers".
		std::vector< D3DXVECTOR3 > veMem = {
				  D3DXVECTOR3( -1, -1, 0 ) // bottom left corner
				, D3DXVECTOR3( -1, +1, 0 ) // top left corner
				, D3DXVECTOR3( +1, -1, 0 ) // bottom right corner
				, D3DXVECTOR3( +1, +1, 0 ) // top right corner
			};
		auto pust = m_psoVertexBuf ->createInitialized( veMem );
		if ( !pust )
			return uptr< vertexBufTopology_t >{ }; // nullptr;
		vertexBufTopology_t stBuf{
				VertexBuf<TInnerDxVer>::OutVertexBuf{
					pust ->m_pcBuffer
					, pust ->m_veBuffers
					, pust ->m_veStride
					, pust ->m_veOffset
				}
				, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			};
		return std::make_unique< vertexBufTopology_t >( stBuf );
	}
};

template<> class Quad<DxVer::v12> {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	const sptr< Tool::VertexBuf<TInnerDxVer> > m_psoVertexBuf;
public:
	Quad(
		Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
		, const sptr< Tool::VertexBuf<TInnerDxVer> > &psoVertexBuf
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoVertexBuf( psoVertexBuf )
	{}

	std::vector< D3D12_INPUT_ELEMENT_DESC > createLayout() {
		std::vector< D3D12_INPUT_ELEMENT_DESC > vecLayout = { 
				{ "POSITION", 0
					, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT
					, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
		return vecLayout;
	}
	struct vertexBufTopology_t {
		VertexBuf<TInnerDxVer>::OutVertexBuf m_stVertexBuf;
		D3D_PRIMITIVE_TOPOLOGY m_enuTopology;
	};	
	auto createVertexBuf() {
		// Fill vertex buffer
		std::vector< D3DXVECTOR3 > veMem = {
				  D3DXVECTOR3( -1, -1, 0 ) // bottom left corner
				, D3DXVECTOR3( -1, +1, 0 ) // top left corner
				, D3DXVECTOR3( +1, -1, 0 ) // bottom right corner
				, D3DXVECTOR3( +1, +1, 0 ) // top right corner
			};
		auto pust = m_psoVertexBuf ->createInitialized( veMem );
		if ( !pust )
			return uptr< vertexBufTopology_t >{ }; // nullptr;
		vertexBufTopology_t stBuf{
				VertexBuf<TInnerDxVer>::OutVertexBuf{
					pust ->m_pcBuffer
					, pust ->m_vertexBufferView
				}
				, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			};
		return std::make_unique< vertexBufTopology_t >( stBuf );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Predef
