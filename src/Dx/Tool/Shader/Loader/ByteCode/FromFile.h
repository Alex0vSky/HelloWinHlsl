// Dx/Tool/Shader/Loader/ByteCode/FromFile.h - loader shader compiled bytecode from file
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::ByteCode {
// common reader
namespace detail_ { 
static std::vector<BYTE> read(const std::wstring &strRelFileName) {
	const std::wstring strFullFileName = Shader::Locator::locate( strRelFileName );
	std::ifstream file( strFullFileName, std::ios::binary );
	return std::vector<BYTE>( std::istreambuf_iterator<char>( file ), {} );
}
} // namespace detail_

template<class TSPE, class T> class FromFile; // primary template

template<class T> class FromFile<DxVer::v9, T> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromFile(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}
	// Load the vertex shader bytecode.
	CPtr<IDirect3DVertexShader9> Vs(
		const std::wstring& strRelFileName
		, std::vector<BYTE>* pveShaderByte = nullptr
	) {
		auto veMem = detail_::read( strRelFileName );
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
	// Load the pixel shader bytecode.
	CPtr<IDirect3DPixelShader9> Ps(const std::wstring& strRelFileName) {
		auto veMem = detail_::read( strRelFileName );
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

template<class T> class FromFile<DxVer::v10, T> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromFile(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load effect bytecode.
	CPtr<ID3D10Effect> Fx(const std::wstring& strRelFileName) {
		auto veMem = detail_::read( strRelFileName );
		if ( !veMem.size( ) )
			return { };
		// Read the D3DX effect file
		DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3D10_SHADER_DEBUG;
		// Otherwise, there will be a warning in VsGa: "This draw call uses 
		//	system-value semantics that interfere with pixel history computation"
		dwShaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif // _DEBUG

		CPtr<ID3D10Blob> pcD3dBlobErrorMsg = nullptr;
		CPtr<ID3D10Effect> pcEffect10 = nullptr;
		::D3DX10CreateEffectFromMemory( veMem.data( ), veMem.size( ), NULL,  NULL, NULL
				, "fx_4_0", dwShaderFlags, 0, m_stDxCtx ->m_pcD3dDevice.Get( ), NULL
				, NULL, pcEffect10.ReleaseAndGetAddressOf( ), pcD3dBlobErrorMsg.ReleaseAndGetAddressOf( ), NULL 
			);
		if ( pcD3dBlobErrorMsg ) {
			// TODO(Alex0vSky): Use a trace class that can change the functionality.
			::OutputDebugStringA( static_cast<char *>( pcD3dBlobErrorMsg ->GetBufferPointer( ) ) );
		}
		return pcEffect10;
	}

	// Load the vertex shader bytecode.
	CPtr<ID3D10VertexShader> Vs(const std::wstring& strRelFileName, std::vector<BYTE>* pveShaderByte = nullptr) {
		auto veMem = detail_::read( strRelFileName );
		if ( !veMem.size( ) )
			return { };
		CPtr<ID3D10VertexShader> pcVS;
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

	// Load the pixel shader bytecode.
	CPtr<ID3D10PixelShader> Ps(const std::wstring& strRelFileName) {
		auto veMem = detail_::read( strRelFileName );
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

template<class T> class FromFile<DxVer::v11, T> {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromFile(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load the vertex shader bytecode.
	CPtr< ID3D11VertexShader > Vs(
		const std::wstring& strRelFileName
		, std::vector<BYTE>* pveShaderByte = nullptr
	) {
		auto veMem = detail_::read( strRelFileName );
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

	// Load the pixel shader bytecode.
	CPtr< ID3D11PixelShader > Ps(const std::wstring& strRelFileName) {
		auto veMem = detail_::read( strRelFileName );
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

template<class T> class FromFile<DxVer::v12, T> {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit FromFile(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	 {}

	// Load the vertex shader bytecode.
	CPtr< ID3DBlob > Vs(const std::wstring& strRelFileName) {
		CPtr< ID3DBlob > pcVS;
		const std::wstring strFullFileName = Shader::Locator::locate( strRelFileName );
		HRESULT hr = ::D3DReadFileToBlob( 
			strFullFileName.c_str( ), pcVS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		return pcVS;
	}

	// Load the pixel shader bytecode.
	CPtr< ID3DBlob > Ps(const std::wstring& strRelFileName) {
		CPtr< ID3DBlob > pcPS;
		const std::wstring strFullFileName = Shader::Locator::locate( strRelFileName );
		HRESULT hr = ::D3DReadFileToBlob( 
			strFullFileName.c_str( ), pcPS.ReleaseAndGetAddressOf( ) );
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::ByteCode
