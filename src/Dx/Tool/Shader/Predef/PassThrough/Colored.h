// Dx/Tool/Shader/Predef/PassThrough/Colored.h - predefined pass-through shaders await layout with color 
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef::PassThrough {
template<class TSPE,class T> class Colored; // primary template, ,class T=TSPE

template<class T> class Colored<DxVer::v9,T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	typename Tool::Shader::Loader::Composite<T>::sptr_t m_psoShaderLoader;
#pragma region shader
	const char *m_szShaderSource = R"A0S_HEREDOC(
struct VOut {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
VOut vs_shader(float4 position : POSITION, float4 color : COLOR) {
    VOut output;

    output.position = position;
    output.color = color;

    return output;
}
float4 ps_shader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
    return color;
}
)A0S_HEREDOC";
#pragma endregion
public:
	typedef uptr< Colored > uptr_t;
	Colored(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoShaderLoader( psoShaderLoader )
	{}

	struct Out_t {
		CPtr< IDirect3DVertexShader9 > m_pcVs;
		CPtr <ID3DXBuffer > m_pcBuffer;
		CPtr< IDirect3DPixelShader9 > m_pcPs;
		// Useful to omit decltype
		auto getVetrexContainer() {
			return std::vector< Vertex< T > >( );
		}
	};
	typedef uptr< Out_t > uptr_Out_t;

	CPtr< IDirect3DPixelShader9 > getPs() { 
		return m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Ps( m_szShaderSource, "ps_shader" );
	}
	uptr_Out_t get_Vs_Ps_InputLayout() {
		Out_t sOut;
		sOut.m_pcVs = m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Vs( m_szShaderSource, "vs_shader", std::addressof( sOut.m_pcBuffer ) );
		if ( !sOut.m_pcVs )
			return nullptr;
		sOut.m_pcPs = getPs( );
		if ( !sOut.m_pcPs )
			return nullptr;
		return uptr_Out_t( new Out_t( sOut ) );
	}
};

template<class T> class Colored<DxVer::v10, T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	typename Tool::Shader::Loader::Composite<T>::sptr_t m_psoShaderLoader;
#pragma region shader
	const char *m_szShaderSource = R"A0S_HEREDOC(
struct VOut {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
VOut vs_shader(float4 position : POSITION, float4 color : COLOR) {
    VOut output;

    output.position = position;
    output.color = color;

    return output;
}
float4 ps_shader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
    return color;
}
)A0S_HEREDOC";
#pragma endregion
public:
	typedef uptr< Colored > uptr_t;
	Colored(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoShaderLoader( psoShaderLoader )
	{}

	struct Out_t {
		CPtr< ID3D10VertexShader > m_pcVs;
		CPtr <ID3D10Blob > m_cpVsBlob;
		CPtr< ID3D10PixelShader > m_pcPs;
		CPtr< ID3D10InputLayout > m_cpLayout;
		// Useful to omit decltype
		auto getVetrexContainer() {
			return std::vector< Vertex< T > >( );
		}
	};
	typedef uptr< Out_t > uptr_Out_t;

	CPtr< ID3D10PixelShader > getPs() { 
		return m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Ps( m_szShaderSource, "ps_shader" );
	}
	uptr_Out_t get_Vs_Ps_InputLayout() {
		Out_t sOut;
		sOut.m_pcVs = m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Vs( m_szShaderSource, "vs_shader", std::addressof( sOut.m_cpVsBlob ) );
		if ( !sOut.m_pcVs )
			return nullptr;
		sOut.m_pcPs = getPs( );
		if ( !sOut.m_pcPs )
			return nullptr;

		// Set vertex input layout
        D3D10_INPUT_ELEMENT_DESC arrDescLayout[] = {
				{ "POSITION"
					, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 }
				, { "COLOR"
					, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 }
			};
        HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateInputLayout(
                arrDescLayout, _countof( arrDescLayout )
				, sOut.m_cpVsBlob ->GetBufferPointer( ), sOut.m_cpVsBlob ->GetBufferSize( )
				, sOut.m_cpLayout.ReleaseAndGetAddressOf( ) 
            );
		if ( FAILED( hr ) )
			return nullptr;

		return uptr_Out_t( new Out_t( sOut ) );
	}

};

template<class T> class Colored<DxVer::v11, T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	typename Tool::Shader::Loader::Composite<T>::sptr_t m_psoShaderLoader;
#pragma region shader
	const char *m_szShaderSource = R"A0S_HEREDOC(
struct VOut {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
VOut vs_shader(float4 position : POSITION, float4 color : COLOR) {
    VOut output;

    output.position = position;
    output.color = color;

    return output;
}
float4 ps_shader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
    return color;
}
)A0S_HEREDOC";
#pragma endregion
public:
	typedef uptr< Colored > uptr_t;
	Colored(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoShaderLoader( psoShaderLoader )
	{}

	struct Out_t {
		CPtr< ID3D11VertexShader > m_pcVs;
		CPtr< ID3DBlob > m_cpVsBlob;
		CPtr< ID3D11PixelShader > m_pcPs;
		CPtr< ID3D11InputLayout > m_cpLayout;
		// Useful to omit decltype
		auto getVetrexContainer() {
			return std::vector< Vertex< T > >( );
		}
	};
	typedef uptr< Out_t > uptr_Out_t;

	CPtr< ID3D11PixelShader > getPs() { 
		return m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Ps( m_szShaderSource, "ps_shader" );
	}
	uptr_Out_t get_Vs_Ps_InputLayout() {
		Out_t sOut;
		sOut.m_pcVs = m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Vs( m_szShaderSource, "vs_shader", std::addressof( sOut.m_cpVsBlob ) );
		if ( !sOut.m_pcVs )
			return nullptr;
		sOut.m_pcPs = getPs( );
		if ( !sOut.m_pcPs )
			return nullptr;

		// Set vertex input layout
        D3D11_INPUT_ELEMENT_DESC arrDescLayout[] = {
				{ "POSITION"
					, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				, { "COLOR"
					, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
        HRESULT hr = m_stDxCtx ->m_pcD3dDevice ->CreateInputLayout(
                arrDescLayout, _countof( arrDescLayout )
				, sOut.m_cpVsBlob ->GetBufferPointer( ), sOut.m_cpVsBlob ->GetBufferSize( )
				, sOut.m_cpLayout.ReleaseAndGetAddressOf( ) 
            );
		if ( FAILED( hr ) )
			return nullptr;

		return uptr_Out_t( new Out_t( sOut ) );
	}

};

template<class T> class Colored<DxVer::v12, T> {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	typename Tool::Shader::Loader::Composite<T>::sptr_t m_psoShaderLoader;
#pragma region shader
	const char *m_szShaderSource = R"A0S_HEREDOC(
struct PSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
PSInput vs_shader(float4 position : POSITION, float4 color : COLOR) {
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}
float4 ps_shader(PSInput input) : SV_TARGET {
    return input.color;
}
)A0S_HEREDOC";
#pragma endregion
public:
	typedef uptr< Colored > uptr_t;
	Colored(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_psoShaderLoader( psoShaderLoader )
	{}

	struct Out_t {
		CPtr< ID3DBlob > m_pcVs;
		CPtr< ID3DBlob > m_pcPs;
		std::vector< D3D12_INPUT_ELEMENT_DESC > m_inputElementDescs;
		// Useful to omit decltype. Until C++20 when allow pass class instance to template arg
		auto getVetrexContainer() {
			return std::vector< Vertex< T > >( );
		}
	};
	typedef uptr< Out_t > uptr_Out_t;

	CPtr< ID3DBlob > getPs() { 
		return m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Ps( m_szShaderSource, "ps_shader" );
	}
	uptr_Out_t get_Vs_Ps_InputLayout() {
		Out_t sOut;
		sOut.m_pcVs = m_psoShaderLoader ->sourceCode( ) ->fromMem( ) ->Vs( m_szShaderSource, "vs_shader" );
		if ( !sOut.m_pcVs )
			return nullptr;
		sOut.m_pcPs = getPs( );
		if ( !sOut.m_pcPs )
			return nullptr;

		// Define the vertex input layout.
		// vector<struct Xxx> its plain memory: "contiguous storage" condition from C++03
		// @insp stackoverflow.com/questions/19376820/is-stdvector-guaranteed-not-to-leave-gaps-between-elements
        sOut.m_inputElementDescs = {
				{ "POSITION", 0
					, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT
					, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
				, { "COLOR", 0
					, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT
					, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

		return uptr_Out_t( new Out_t( sOut ) );
	}

};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef::PassThrough
