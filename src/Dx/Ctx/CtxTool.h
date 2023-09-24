// Dx/Ctx/CtxTool.h - structure of helper tools context
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
// Forward decl
namespace prj_3d::HelloWinHlsl::Dx {
	namespace Tool { namespace Shader { namespace Loader { template<class T> class Composite; } } }
	namespace Tool { namespace Shader { namespace Predef { template<class T> class Composite; } } }
	namespace Tool { template<class TSPE, class T = TSPE> class VertexBuf; }
	namespace Tool { template<class T> class IndexBuf; }
	namespace Tool { namespace Predef { template<class T> class Quad; } }
	namespace Tool { namespace ConstantBuf { namespace viaStruct { template<class T> class Factory; } } }
	namespace Tool { namespace ConstantBuf { namespace viaStruct { class RootSignatureCreator; } } }
	namespace Tool { namespace Shader { class ConstantSetterDx9; } }
}

namespace prj_3d::HelloWinHlsl::Dx::Ctx { 

// primary template, "typename = void" for partial specialization
// @insp stackoverflow.com/questions/55998031/multiple-conditional-template-specialization-c
template<class T, typename = void> class CtxTool;

namespace detail_ {
template<class T> 
struct CommonToolCtx {
	const sptr< Tool::Shader::Loader::Composite<T> > m_psoShaderLoader;
	uptr< Tool::Shader::Predef::Composite<T> > m_puoShaderPredef;
	const sptr< Tool::VertexBuf<T> > m_psoVertexBuf;
	uptr< Tool::Predef::Quad<T> > m_puoQuad;
	uptr< Tool::ConstantBuf::viaStruct::Factory<T> > m_puoCBufAccessorFactory;

	typedef uptr< const CtxTool<T> > uptrc_t;
	typedef const uptrc_t &cref_ptr_t;
};

template<class T> 
class CommonMethodsToolCtx : protected detail_::CommonToolCtx<T> {
 public:
	explicit CommonMethodsToolCtx(CommonToolCtx &&aggregate ) : CommonToolCtx{ std::move( aggregate ) }
	 {}
#pragma region methods
	auto shader() const { 
		class 
		ChainMe {
			Tool::Shader::Loader::Composite<T> *m_poShaderLoader;
			Tool::Shader::Predef::Composite<T> *m_poShaderPredef;
			
		 public:
			ChainMe(Tool::Shader::Loader::Composite<T> *poShaderLoader
				, Tool::Shader::Predef::Composite<T> *poShaderPredef
			) {
				//: m_poShaderLoader( nullptr ) // clang
				//, m_poShaderPredef( nullptr )
				this ->m_poShaderLoader = poShaderLoader;
				this ->m_poShaderPredef = poShaderPredef;
				return;
			}
			auto loader() const {
				return m_poShaderLoader;
			}
			auto predef() const {
				return m_poShaderPredef;
			}
		};
		return std::make_unique< ChainMe >( m_psoShaderLoader.get( ), m_puoShaderPredef.get( ) );
	}
	auto vertexBuf() const {
		return m_psoVertexBuf.get( );
	}
	auto quad() const {
		return m_puoQuad.get( );
	}
#pragma endregion // methods
};

} // namespace detail_

// the partial specialization other Dx
template<class T> 
class CtxTool<T> : public detail_::CommonMethodsToolCtx<T> {
 public:
	explicit CtxTool(
			CommonToolCtx &&aggregate
		) 
		: CommonMethodsToolCtx{ std::move( aggregate ) }
	 {}
	auto constBufAccessorFactory() const {
		return CommonMethodsToolCtx::m_puoCBufAccessorFactory.get( );
	}
};
// the full specialization Dx9
template<> class CtxTool<DxVer::v9> : public detail_::CommonMethodsToolCtx<DxVer::v9> { 
	typedef sptr< Tool::Shader::ConstantSetterDx9 > ConstantSetterDx9_t;
	typedef sptr< Tool::IndexBuf<DxVer::v9> > IndexBuf_t;
	ConstantSetterDx9_t m_puoConstantSetterDx9;
	IndexBuf_t m_psoIndexBuf;
 public:
	CtxTool(
			CommonToolCtx &&aggregate
			, const ConstantSetterDx9_t &puoConstantSetterDx9
			, const IndexBuf_t &psoIndexBuf
		) 
		: CommonMethodsToolCtx{ std::move( aggregate ) }
		, m_puoConstantSetterDx9( puoConstantSetterDx9 )
		, m_psoIndexBuf( psoIndexBuf )
	 {}
	auto shaderConstSetterDx9() const {
		return m_puoConstantSetterDx9.get( );
	}
	auto indexBuf() const {
		return m_psoIndexBuf.get( );
	}
};
// the full specialization Dx10
template<> class CtxTool<DxVer::v10> : public detail_::CommonMethodsToolCtx<DxVer::v10> { 
	typedef sptr< Tool::IndexBuf<DxVer::v10> > IndexBuf_t;
	IndexBuf_t m_psoIndexBuf;
 public:
	CtxTool(
			CommonToolCtx &&aggregate
			, const IndexBuf_t &psoIndexBuf
		) 
		: CommonMethodsToolCtx{ std::move( aggregate ) }
		, m_psoIndexBuf( psoIndexBuf )
	 {}
	auto constBufAccessorFactory() const {
		return CommonMethodsToolCtx::m_puoCBufAccessorFactory.get( );
	}
	auto indexBuf() const {
		return m_psoIndexBuf.get( );
	}
};
// the full specialization Dx12
template<> class CtxTool<DxVer::v12> : public detail_::CommonMethodsToolCtx<DxVer::v12> { 
	typedef sptr< Tool::ConstantBuf::viaStruct::RootSignatureCreator > RootSignatureCreator_t;
	RootSignatureCreator_t m_puoRootSignatureCreator;
 public:
	CtxTool(
			CommonToolCtx &&aggregate
			, const RootSignatureCreator_t &puoRootSignatureCreator
		) 
		: CommonMethodsToolCtx{ std::move( aggregate ) }
		, m_puoRootSignatureCreator( puoRootSignatureCreator )
	 {}
	auto constBufAccessorFactory() const {
		return CommonMethodsToolCtx::m_puoCBufAccessorFactory.get( );
	}
	auto constBufRootSignature() const {
		return m_puoRootSignatureCreator.get( );
	}
};

} // namespace prj_3d::HelloWinHlsl::Dx::Ctx
