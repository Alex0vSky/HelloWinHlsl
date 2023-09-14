// Dx/Tool/Shader/Loader/Composite.h - composite shader loader
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader {
template<class T>
class Composite {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
	const uptr< ChainSourceCode< T > > m_puoChainSourceCode;
	const uptr< ChainByteCode< T > > m_puoChainByteCode;
	const uptr< ChainArrayC< T > > m_puoChainArrayC;
 public:
	typedef sptr< Composite > sptr_t;
	typedef const sptr_t &cref_ptr_t;
	explicit Composite(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx )
		, m_puoChainSourceCode( std::make_unique< ChainSourceCode< T > >( stDxCtx ) )
		, m_puoChainByteCode( std::make_unique< ChainByteCode< T > >( stDxCtx ) )
		, m_puoChainArrayC( std::make_unique< ChainArrayC< T > >( stDxCtx ) )
	 {}
	auto sourceCode() {
		return m_puoChainSourceCode.get( );
	}
	auto byteCode() {
		return m_puoChainByteCode.get( );
	}
	// TODO(Alex): makeme
	auto arrayC() {
		return m_puoChainArrayC.get( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader::Loader
