// Dx/Tool/Shader/Loader/ChainByteCode.h - ...
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// Forward decl
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader { 
	namespace ByteCode { template<class TSPE, class T = TSPE> class FromFile; }
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader

namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader {
template<class T>
class ChainByteCode {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const uptr< ByteCode::FromFile<T> > m_puoFromFile;
 public:
	explicit ChainByteCode(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx )
		, m_puoFromFile( std::make_unique< ByteCode::FromFile< T > >( stDxCtx ) )
	 {}
	auto fromFile() {
		return m_puoFromFile.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader
