// Dx/Tool/Shader/Loader/ChainArrayC.h - ...
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// Forward decl
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader { 
	namespace ArrayC { template<class TSPE, class T = TSPE> class FromHeader; }
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader

namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader {
template<class T>
class ChainArrayC {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const uptr< ArrayC::FromHeader<T> > m_puoFromFile;
 public:
	explicit ChainArrayC(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx )
		, m_puoFromFile( std::make_unique< ArrayC::FromHeader< T > >( stDxCtx ) )
	 {}
	auto fromHeader() {
		return m_puoFromFile.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader
