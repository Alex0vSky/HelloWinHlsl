// Dx/Tool/Shader/Loader/ChainSourceCode.h - ...
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// Forward decl
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader { 
	namespace SourceCode { template<class TSPE, class T = TSPE> class FromMem; }
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader

namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader {
template<class T>
class ChainSourceCode {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const uptr< SourceCode::FromMem<T> > m_puoFromMem;
 public:
	explicit ChainSourceCode(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx )
		, m_puoFromMem( std::make_unique< SourceCode::FromMem< T > >( stDxCtx ) )
	 {}
	auto fromMem() {
		return m_puoFromMem.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader
