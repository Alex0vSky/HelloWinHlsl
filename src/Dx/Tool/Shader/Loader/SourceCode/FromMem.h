// Dx/Tool/Shader/Loader/SourceCode/FromMem.h - loader shader from shader source code placed in memory
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::SourceCode {
template<class TSPE,class T> class FromMem; // primary template

template<class T> class FromMem<DxVer::v9,T> {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
public:
	explicit FromMem(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	{}

	// preference vertex shader model version is 3_0
	CPtr< IDirect3DVertexShader9 > 
	Vs(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr<ID3DXBuffer> *ppcVBlob = nullptr
	) {
		return Vs30( strShadeSourceCode, strEntryPointName, ppcVBlob );
	}
	// preference pixel shader model version is 3_0
	CPtr< IDirect3DPixelShader9 > 
	Ps(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
	) {
		return Ps30( strShadeSourceCode, strEntryPointName );
	}

	CPtr< IDirect3DVertexShader9 > 
	Vs30(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr<ID3DXBuffer> *ppcVBlob = nullptr
	) {
		HRESULT hr;
		CPtr<ID3DXBuffer> pcBlob; 
		CPtr< IDirect3DVertexShader9 > pcVS;
		hr = ::D3DXCompileShader( 
				strShadeSourceCode.c_str( ), (UINT)strShadeSourceCode.length( ), NULL, NULL
				, strEntryPointName.c_str( ), "vs_3_0", 0
				, pcBlob.ReleaseAndGetAddressOf( ), nullptr, nullptr 
			);
		if ( FAILED( hr ) )
			return { };
		hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
				static_cast<DWORD *>( pcBlob ->GetBufferPointer( ) )
				, pcVS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		if ( ppcVBlob )
			*ppcVBlob = pcBlob;
		return pcVS;
	}

	CPtr< IDirect3DPixelShader9 > 
	Ps30(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
	) {
		HRESULT hr;
		CPtr<ID3DXBuffer> pcBlob; 
		CPtr< IDirect3DPixelShader9 > pcPS;
		CPtr< ID3DXBuffer > pcErrorMsgs;
		hr = ::D3DXCompileShader( 
				strShadeSourceCode.c_str( )
				, (UINT)strShadeSourceCode.length( ), NULL, NULL
				, strEntryPointName.c_str( ), "ps_3_0", 0
				, pcBlob.ReleaseAndGetAddressOf( ), pcErrorMsgs.ReleaseAndGetAddressOf( ), nullptr 
			);
		if ( FAILED( hr ) ) {
			char *szErrorMsgs = static_cast<char *>( pcErrorMsgs ->GetBufferPointer( ) );
			UNREFERENCED_PARAMETER( szErrorMsgs );
			return { };
		}
		hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
				static_cast<DWORD *>( pcBlob ->GetBufferPointer( ) )
				, pcPS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}
};

template<class T> class FromMem<DxVer::v10,T> {
	using TInnerDxVer = DxVer::v10;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
public:
	explicit FromMem(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	{}

	// preference vertex shader model version is 4_0
	CPtr< ID3D10VertexShader > 
	Vs(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr<ID3D10Blob> *ppcVBlob = nullptr
	) {
		return Vs40( strShadeSourceCode, strEntryPointName, ppcVBlob );
	}
	// preference pixel shader model version is 4_0
	CPtr< ID3D10PixelShader > 
	Ps(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
	) {
		return Ps40( strShadeSourceCode, strEntryPointName );
	}

	// Load the vertex shader of the "vs_4_0" model, from string.
	CPtr<ID3D10VertexShader> 
	Vs40(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr<ID3D10Blob> *ppcVBlob = nullptr
	) {
		HRESULT hr;
		CPtr<ID3D10Blob> pcBlob;
		hr = ::D3D10CompileShader(
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, NULL, NULL, NULL, strEntryPointName.c_str( ), "vs_4_0", 0
				, pcBlob.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return { };
		CPtr<ID3D10VertexShader> pcVS;
		hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
				pcBlob ->GetBufferPointer( )
				, pcBlob ->GetBufferSize( )
				, pcVS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		if ( ppcVBlob )
			*ppcVBlob = pcBlob;
		return pcVS;
	}

	// Load the pixel shader of the vs_4_0 model, from string.
	CPtr<ID3D10PixelShader> 
	Ps40(
			const std::string &strShadeSourceCode
			, const std::string &strEntryPointName = "main"
		)
	{
		HRESULT hr;
		CPtr<ID3D10Blob> pcBlob;
		hr = ::D3D10CompileShader(
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, NULL, NULL, NULL, strEntryPointName.c_str( ), "ps_4_0", 0, pcBlob.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return { };
		CPtr<ID3D10PixelShader> pcPS;
		hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
				pcBlob ->GetBufferPointer( )
				, pcBlob ->GetBufferSize( )
				, pcPS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}

};

template<class T> class FromMem<DxVer::v11,T> {
	using TInnerDxVer = DxVer::v11;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
public:
	explicit FromMem(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	{}

	// preference vertex shader model version is 4_0
	CPtr< ID3D11VertexShader > 
	Vs(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr< ID3DBlob > *ppcVBlob = nullptr
	) {
		return Vs40( strShadeSourceCode, strEntryPointName, ppcVBlob );
	}
	// preference pixel shader model version is 4_0
	CPtr< ID3D11PixelShader > 
	Ps(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
	) {
		return Ps40( strShadeSourceCode, strEntryPointName );
	}

	// Load the vertex shader of the "vs_4_0" model, from string.
	CPtr<ID3D11VertexShader> 
	Vs40(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr< ID3DBlob > *ppcVBlob = nullptr
	) {
		HRESULT hr;
		CPtr< ID3DBlob > pcBlob;
		hr = ::D3DCompile( 
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, nullptr, nullptr, nullptr, strEntryPointName.c_str( ), "vs_4_0", 0, 0
				, pcBlob.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return { };
		CPtr<ID3D11VertexShader> pcVS;
		hr = m_stDxCtx ->m_pcD3dDevice ->CreateVertexShader( 
				pcBlob ->GetBufferPointer( )
				, pcBlob ->GetBufferSize( )
				, nullptr
				, pcVS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		if ( ppcVBlob )
			*ppcVBlob = pcBlob;
		return pcVS;
	}

	// Load the pixel shader of the vs_4_0 model, from string.
	CPtr< ID3D11PixelShader > 
	Ps40(
			const std::string &strShadeSourceCode
			, const std::string &strEntryPointName = "main"
		)
	{
		HRESULT hr;
		CPtr< ID3DBlob > pcBlob;
		hr = ::D3DCompile( 
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, nullptr, nullptr, nullptr, strEntryPointName.c_str( ), "ps_4_0", 0, 0
				, pcBlob.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return { };
		CPtr< ID3D11PixelShader > pcPS;
		hr = m_stDxCtx ->m_pcD3dDevice ->CreatePixelShader( 
				pcBlob ->GetBufferPointer( )
				, pcBlob ->GetBufferSize( )
				, nullptr
				, pcPS.ReleaseAndGetAddressOf( ) 
			);
		if ( FAILED( hr ) )
			return { };
		return pcPS;
	}

};

template<class T> class FromMem<DxVer::v12,T> {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	UINT m_compileFlags;
public:
	explicit FromMem(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx )
	{
		m_compileFlags = 0;
		// Enable better shader debugging with the graphics debugging tools.
#ifdef _DEBUG
		m_compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	}

	// preference vertex shader model version is 5_0
	CPtr< ID3DBlob > 
	Vs(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr< ID3DBlob > *ppcVBlob = nullptr
	) {
		return Vs50( strShadeSourceCode, strEntryPointName, ppcVBlob );
	}
	// preference pixel shader model version is 5_0
	CPtr< ID3DBlob > 
	Ps(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
	) {
		return Ps50( strShadeSourceCode, strEntryPointName );
	}

	// Load the vertex shader of the "vs_5_0" model, from string.
	CPtr<ID3DBlob> 
	Vs50(
		const std::string &strShadeSourceCode
		, const std::string &strEntryPointName = "main"
		, CPtr< ID3DBlob > *ppcVBlob = nullptr
	) {
		HRESULT hr;
		CPtr< ID3DBlob > pcBlob;
		hr = ::D3DCompile( 
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, nullptr, nullptr, nullptr, strEntryPointName.c_str( ), "vs_5_0", m_compileFlags, 0
				, pcBlob.ReleaseAndGetAddressOf( ), nullptr
			);
		if ( FAILED( hr ) )
			return { };
		if ( ppcVBlob )
			*ppcVBlob = pcBlob;
		return pcBlob;
	}

	// Load the pixel shader of the vs_5_0 model, from string.
	CPtr< ID3DBlob > 
	Ps50(
			const std::string &strShadeSourceCode
			, const std::string &strEntryPointName = "main"
		)
	{
		HRESULT hr;
		CPtr< ID3DBlob > pcBlob;
		hr = ::D3DCompile( 
				strShadeSourceCode.c_str( ), strShadeSourceCode.length( )
				, nullptr, nullptr, nullptr, strEntryPointName.c_str( ), "ps_5_0", m_compileFlags, 0
				, pcBlob.ReleaseAndGetAddressOf( ), NULL
			);
		if ( FAILED( hr ) )
			return { };
		return pcBlob;
	}

};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader::SourceCode
