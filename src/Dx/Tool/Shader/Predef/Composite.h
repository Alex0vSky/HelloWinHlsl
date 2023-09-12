// Dx/Tool/Shader/Predef/Composite.h - composite predefined shaders
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef {
template<class T>
class Composite {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const typename ChainPassThrough< T >::uptr_t m_puoChainPassThrough;
public:
	Composite(
		Ty::StDxCtx_crefPtr<T> stDxCtx
		, typename Tool::Shader::Loader::Composite<T>::cref_ptr_t psoShaderLoader
	) 
		: m_stDxCtx( stDxCtx )
		, m_puoChainPassThrough( std::make_unique< ChainPassThrough< T > >( stDxCtx, psoShaderLoader ) )
	{}
	auto passThrough() {
		return m_puoChainPassThrough.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Predef
