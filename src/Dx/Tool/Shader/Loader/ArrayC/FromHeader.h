// Dx/Tool/Shader/Loader/ArrayC/FromHeader.h - loader shader compiled bytecode from header file
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::ArrayC {
// common reader
namespace detail_ { 
static std::vector<char> read(const std::wstring &strRelFileName) {
	const std::wstring strFullFileName = Shader::Locator::locate( strRelFileName );
	std::ifstream file( strFullFileName, std::ios::binary );
	return std::vector<char>( std::istreambuf_iterator<char>( file ), {} );
}
// TODO(Alex0vSky): simplifyme
template<typename T> 
struct check {
	typedef std::remove_cv_t< std::remove_reference_t< T > > type;
	static_assert( std::is_array_v< type >, "expect only array" );
	static_assert( std::rank_v< type > == 1, "expect only one dimension array" );
	static_assert( std::extent_v< type > > 0, "expect only bounded array" );
	static void array() {
	}
};
} // namespace detail_

template<class TSPE, class T> class FromHeader; // primary template

template<class T> class FromHeader<DxVer::v9, T> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromHeader(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}
	// Load the vertex shader.
	template <typename TCALLABLE> 
	CPtr< IDirect3DVertexShader9 > Vs(
		TCALLABLE fn
		, std::vector<BYTE>* pveShaderByte = nullptr
	) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr<IDirect3DVertexShader9> pcVS;
		// If you compile the shader in fxc.exe with "vs_2_0" (as in the examples for Dx9), 
		//	then it is not accepted in CreateVertexShader().
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
				reinterpret_cast<DWORD *>( veMem.data( ) )
				, pcVS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		if ( pveShaderByte )
			*pveShaderByte = veMem;
		return pcVS;
	}

	// Load the pixel shader
	template <typename TCALLABLE> 
	CPtr<IDirect3DPixelShader9> Ps(TCALLABLE fn) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr<IDirect3DPixelShader9> pcPS;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
				reinterpret_cast<DWORD *>( veMem.data( ) )
				, pcPS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}
};

template<class T> class FromHeader<DxVer::v10, T> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromHeader(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load the vertex shader.
	template <typename TCALLABLE> 
	CPtr< ID3D10VertexShader > Vs(
		TCALLABLE fn
		, std::vector<BYTE>* pveShaderByte = nullptr
	) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr< ID3D10VertexShader > pcVS;
		// If you compile the shader in fxc.exe with "vs_2_0" (as in the examples for Dx9), 
		//	then it is not accepted in CreateVertexShader().
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
			veMem.data( ), veMem.size( ), pcVS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		if ( pveShaderByte )
			*pveShaderByte = veMem;
		return pcVS;
	}

	// Load the pixel shader.
	template <typename TCALLABLE> 
	CPtr< ID3D10PixelShader > Ps(TCALLABLE fn) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr<ID3D10PixelShader> pcPS;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
			veMem.data( ), veMem.size( ), pcPS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}
};

template<class T> class FromHeader<DxVer::v11, T> {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromHeader(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load the vertex shader.
	template <typename TCALLABLE> 
	CPtr< ID3D11VertexShader > Vs(
		TCALLABLE fn
		, std::vector<BYTE>* pveShaderByte = nullptr
	) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr< ID3D11VertexShader > pcVS;
		// If you compile the shader in fxc.exe with "vs_2_0" (as in the examples for Dx9), 
		//	then it is not accepted in CreateVertexShader().
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
			veMem.data( ), veMem.size( ), nullptr, pcVS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		if ( pveShaderByte )
			*pveShaderByte = veMem;
		return pcVS;
	}

	// Load the pixel shader.
	template <typename TCALLABLE> 
	CPtr< ID3D11PixelShader > Ps(TCALLABLE fn) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );
		std::vector<BYTE> veMem( arrayC, arrayC + sizeof( arrayC ) );
		if ( !veMem.size( ) )
			return { };
		CPtr< ID3D11PixelShader > pcPS;
		HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
			veMem.data( ), veMem.size( ), nullptr, pcPS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}
};

template<class T> class FromHeader<DxVer::v12, T> {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromHeader(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load the vertex shader.
	template <typename TCALLABLE> 
	CPtr< ID3DBlob > Vs(TCALLABLE fn) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );

		CPtr< ID3DBlob > pcVS;
		HRESULT hr = ::D3DCreateBlob( 
				sizeof( arrayC )
				, pcVS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		memcpy_s( pcVS ->GetBufferPointer( ), pcVS ->GetBufferSize( ), arrayC, sizeof( arrayC ) );
		return pcVS;
	}

	// Load the pixel shader.
	template <typename TCALLABLE> 
	CPtr< ID3DBlob > Ps(TCALLABLE fn) {
		const auto &arrayC = fn( );
		detail_::check< decltype( arrayC ) >::array( );

		CPtr< ID3DBlob > pcPS;
		HRESULT hr = ::D3DCreateBlob( 
				sizeof( arrayC )
				, pcPS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		memcpy_s( pcPS ->GetBufferPointer( ), pcPS ->GetBufferSize( ), arrayC, sizeof( arrayC ) );
		return pcPS;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::ArrayC
