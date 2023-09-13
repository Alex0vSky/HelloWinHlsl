// Dx/Ctx/FactoryCtxs.h - factory of creating DirectX3D context
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Ctx {

template<class T> struct CtxFactory; // Fwd decl

namespace detail_ {

template<class T> 
static 
void setWindowTile(
	Sys::Wnd::StToken::cref_t crstWndToken
	, const std::string &strAdapterDescr
) {
	Sys::Wnd::Change::title<T>( crstWndToken.m_hWnd, crstWndToken.m_strClientAppName, strAdapterDescr );
}

// From Dx::Ctx::detail_::Common Dx/Ctx/CtxTool.h
template<class T>
static 
auto getTupleBaseArgs(Ty::StDxCtx_crefPtr<T> psstDxCtx) {
	auto psoShaderLoader = std::make_shared< Dx::Tool::Shader::Loader::Composite<T> >( psstDxCtx );
	auto psoVertexBuf = std::make_shared< Dx::Tool::VertexBuf<T> >( psstDxCtx );
	return std::make_tuple( 
			psoShaderLoader
			, std::make_unique< Dx::Tool::Shader::Predef::Composite<T> >( psstDxCtx, psoShaderLoader )
			, psoVertexBuf
			, std::make_unique< Tool::Predef::Quad<T> >( psstDxCtx, psoVertexBuf )
			, std::make_unique< Tool::ConstantBuf::viaStruct::FactoryStrict<T> >( psstDxCtx )
		);
}

template< typename T, typename tup, class... Args1>
static 
auto make_uniqueToolCtx(tup&& t, Args1&& ... args1) {
    return std::apply(
        [&args1...] (auto&& ... args2) {
			return std::make_unique< const Dx::Ctx::CtxTool< T > >
                (
					CommonToolCtx< T >{ std::forward<decltype(args2)>(args2)... }
					, std::move( args1 )...
				);
        },
        std::forward<tup>(t)
    );
}

template<class T> 
static 
std::enable_if_t< ( T::m_version == 9 ), Ty::StCtxs_ptr<T> >
ctorCtx(Ty::StDxCtx_crefPtr<T> psstDxCtx) {
	auto tupBaseArgs = getTupleBaseArgs< T >( psstDxCtx );

	auto puoConstantSetterDx9 = std::make_unique< Dx::Tool::Shader::ConstantSetterDx9 >( psstDxCtx );
	auto psoIndexBuf = std::make_shared< Dx::Tool::IndexBuf<T> >( psstDxCtx );
	auto pustToolCtx = make_uniqueToolCtx< T >
		(
			std::move( tupBaseArgs )
			, std::move( puoConstantSetterDx9 )
			, std::move( psoIndexBuf )
		);

	return Dx::Ctxs< T >::z_make_unique( 
			std::move( psstDxCtx )
			, std::move( pustToolCtx )
		);
}

template<class T> 
static 
std::enable_if_t< ( T::m_version == 10 ), Ty::StCtxs_ptr<T> >
ctorCtx(Ty::StDxCtx_crefPtr<T> psstDxCtx) {
	auto tupBaseArgs = getTupleBaseArgs< T >( psstDxCtx );

	auto psoIndexBuf = std::make_shared< Dx::Tool::IndexBuf<T> >( psstDxCtx );
	auto pustToolCtx = make_uniqueToolCtx< T >
		(
			std::move( tupBaseArgs )
			, std::move( psoIndexBuf )
		);

	return Dx::Ctxs< T >::z_make_unique( 
			std::move( psstDxCtx )
			, std::move( pustToolCtx )
		);
}

template<class T> 
static 
std::enable_if_t< ( T::m_version == 11 ), Ty::StCtxs_ptr<T> >
ctorCtx(Ty::StDxCtx_crefPtr<T> psstDxCtx) {
	auto tupBaseArgs = getTupleBaseArgs< T >( psstDxCtx );

	auto pustToolCtx = make_uniqueToolCtx< T >
		(
			std::move( tupBaseArgs )
		);

	return Dx::Ctxs< T >::z_make_unique( 
			std::move( psstDxCtx )
			, std::move( pustToolCtx )
		);
}

template<class T> 
static 
std::enable_if_t< ( T::m_version == 12  ), Ty::StCtxs_ptr<T> >
ctorCtx(Ty::StDxCtx_crefPtr<T> psstDxCtx) {
	auto tupBaseArgs = getTupleBaseArgs< T >( psstDxCtx );

	auto puoRootSignatureCreator = std::make_unique
		< Dx::Tool::ConstantBuf::viaStruct::RootSignatureCreator >
		( psstDxCtx );
	auto pustToolCtx = make_uniqueToolCtx< T > (
			std::move( tupBaseArgs )
			, std::move( puoRootSignatureCreator )
		);

	return Dx::Ctxs< T >::z_make_unique( 
			std::move( psstDxCtx )
			, std::move( pustToolCtx )
		);
}

template<class T, class TAdapter, class TCtxImpl>
struct BaseCtxFactory {
	static 
	Ty::StCtxs_ptr<T>
	create(Sys::Wnd::StToken::cref_t crstWndToken) {
		TAdapter oAdapter;
		if ( !oAdapter.prepare( ) ) 
			return nullptr;
		detail_::setWindowTile<T>( crstWndToken, oAdapter.getDescrString( ) );

		Ty::StDxCtx_ptr<T> psstDxCtx = 
			TCtxImpl::create( crstWndToken, oAdapter );
		if ( !psstDxCtx )
			return nullptr;
		return detail_::ctorCtx< T >( psstDxCtx );
	}
};
} // namespace detail_

template<> struct CtxFactory<DxVer::v9>
	: public detail_::BaseCtxFactory<
		DxVer::v9
		, Dx::Adapter::Dx9Adapter
		, Dx::Ctx::Impl::Dx9CtxImpl
	>
{};
template<> struct CtxFactory<DxVer::v10>
	: public detail_::BaseCtxFactory<
		DxVer::v10
		, Dx::Adapter::Dx10Adapter
		, Dx::Ctx::Impl::Dx10CtxImpl
	>
{};
template<> struct CtxFactory<DxVer::v11>
	: public detail_::BaseCtxFactory<
		DxVer::v11
		, Dx::Adapter::Dx11Adapter
		, Dx::Ctx::Impl::Dx11CtxImpl
	>
{};
template<> struct CtxFactory<DxVer::v12>
	: public detail_::BaseCtxFactory<
		DxVer::v12
		, Dx::Adapter::Dx12Adapter
		, Dx::Ctx::Impl::Dx12CtxImpl
	>
{};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx
