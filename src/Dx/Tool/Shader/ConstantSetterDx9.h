// Dx/Tool/Shader/ConstantSetterDx9.h - setter Dx9 shaders constants
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader {
class ConstantSetterDx9 {
	using TInnerDxVer = DxVer::v9;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	
 public:
	explicit ConstantSetterDx9(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx ) 
	 {}

	class Setter {
		const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
		const CPtr< ID3DXConstantTable > m_cpConstantTable;
		
	public:
		typedef uptr< Setter > uptrc_t;
		Setter(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx
				, const CPtr<ID3DXConstantTable> &cpConstantTable
			)
			: m_stDxCtx( stDxCtx )
			, m_cpConstantTable( cpConstantTable )
		 {}
		bool setFloat4(const std::string &strNameConst, const D3DXVECTOR4 &value) const {
			// variable can optimized/removed by fxc like "dead code" and get D3DERR_INVALIDCALL
			if ( D3DXHANDLE h = m_cpConstantTable ->GetConstantByName( 0, strNameConst.c_str( ) ) )
				if ( SUCCEEDED( m_cpConstantTable ->SetVector( m_stDxCtx ->m_pcD3dDevice.Get( ), h, &value ) ) ) 
					return true;
			return false;
		}
		bool setMatrix(const std::string &strNameConst, const D3DXMATRIX &value) const {
			if ( D3DXHANDLE h = m_cpConstantTable ->GetConstantByName( 0, strNameConst.c_str( ) ) )
				if ( SUCCEEDED( m_cpConstantTable ->SetMatrix( m_stDxCtx ->m_pcD3dDevice.Get( ), h, &value ) ) ) 
					return true;
			return false;
		}
		bool setFloat(const std::string &strNameConst, float value) const {
			if ( D3DXHANDLE h = m_cpConstantTable ->GetConstantByName( 0, strNameConst.c_str( ) ) )
				if ( SUCCEEDED( m_cpConstantTable ->SetFloat( m_stDxCtx ->m_pcD3dDevice.Get( ), h, value ) ) ) 
					return true;
			return false;
		}
	};

	template<class U>
	typename std::enable_if_t< false
			|| std::is_same_v< U, IDirect3DVertexShader9 >
			|| std::is_same_v< U, IDirect3DPixelShader9 >
		, Setter::uptrc_t >
	create(const CPtr<U> &cp) const {
		UINT uSizeData = 0;
		if ( FAILED( cp ->GetFunction( nullptr, &uSizeData  ) ) ) 
			return nullptr;
		std::vector< char > veMemFunc( static_cast<size_t>( uSizeData ) );
		if ( FAILED( cp ->GetFunction( &veMemFunc[ 0 ], &uSizeData ) ) ) 
			return nullptr;
		CPtr< ID3DXConstantTable > cpConstantTable;
		if ( FAILED( ::D3DXGetShaderConstantTable( 
				reinterpret_cast<DWORD *>( &veMemFunc[ 0 ] )
				, cpConstantTable.ReleaseAndGetAddressOf( ) ) ) 
			) 
			return nullptr;
		return std::make_unique< Setter >( m_stDxCtx, cpConstantTable );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader
