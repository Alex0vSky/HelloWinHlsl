// Dx/Ctxs.h - ...
#pragma once
namespace prj_3d::HelloWinHlsl::Dx { 

template<class T>
struct Ctxs {
	typedef uptr< const Ctxs > uptrc_t;
	typedef const uptrc_t &cref_ptr_t;
	template<typename... Args> static auto z_make_unique(Args&&... args) {
		return std::make_unique< Tpl::Trait::aggregate_adapter< Ctxs< T > > >( 
			std::forward<Args>(args)... );
	}

	const typename Ctx::CtxDx<T>::sptrc_t m_psstDxCtx;
	const typename Ctx::CtxTool<T>::uptrc_t m_pustToolCtx;
};
} // namespace prj_3d::HelloWinHlsl::Dx
