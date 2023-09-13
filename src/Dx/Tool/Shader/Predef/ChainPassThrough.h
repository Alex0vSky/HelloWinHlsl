// Dx/Tool/Shader/Predef/ChainPassThrough.h - predefined shaders for 
//	pass-through vertice and color without changes
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// Forward decl
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef {
	namespace PassThrough { template<class TSPE, class T = TSPE> class Colored; }
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef

namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef {
template<class T>
class ChainPassThrough {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const typename PassThrough::Colored<T>::uptr_t m_puoColored;
 public:
	typedef uptr< ChainPassThrough > uptr_t;
	ChainPassThrough(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_puoColored( std::make_unique< PassThrough::Colored< T > >( stDxCtx, psoShaderLoader ) )
	 {}
	// mean float4 var passed in vertice, and layout with color
	auto colored() {
		return m_puoColored.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef
