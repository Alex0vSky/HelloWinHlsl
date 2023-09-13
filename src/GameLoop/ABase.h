// GameLoop/ABase.h - base abstract
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::GameLoop {
namespace detail_ {

template<class T, class TWndProcHolder>
class ABase {
 protected:
	typedef std::mutex mutex_t;
	typedef std::function< void () > pfnCallback_t;

	pfnCallback_t m_pfnCallbackBeforeRender;
	void setCallbackBeforeRender(const pfnCallback_t &clb) {
		m_pfnCallbackBeforeRender = clb;
	}
	pfnCallback_t m_pfnCallbackAfterRender;
	void setCallbackAfterRender(const pfnCallback_t &clb) {
		m_pfnCallbackAfterRender = clb;
	}
	pfnCallback_t m_pfnCallbackAfterPresent;
	void setCallbackAfterPresent(const pfnCallback_t &clb) {
		m_pfnCallbackAfterPresent = clb;
	}

    Ty::StCtxs_ptr<T> m_stCtx;
    typename Ty::CurClientApp<T>::uptr_t m_puoClientApp;
    typename DrawAux::Fps::Ordinary<T>::uptr_t m_puoDrawAuxFps;
    FrameLimiter::ABase::uptr_t m_puoFrameLimiter;

	typename TWndProcHolder::uptr_t m_puoWnd;

	// Allow client to configure auxiliary DirectX features.
	typename Dx::AdjustAux<T>::uptr_t m_puoAdjustDxAux;
	// DynamicData for client, instead all other constant structures
	Ty::DynamicData_ptr<T> m_psoDynamicData;

    bool initialize() {
        if ( !m_puoClientApp ->init( 
				m_stCtx ->m_psstDxCtx
				, m_stCtx ->m_pustToolCtx
				, m_puoAdjustDxAux.get( ) 
			) 
		) {
			return false;
		}
		// The client has installed the callbacks, now you can complete the setup of the Dx stuff.
		m_puoAdjustDxAux ->adjust( );
		return true;
	}

 public:
	explicit ABase(
        Ty::StCtxs_ptr<T> pustCtx
        , typename Ty::CurClientApp<T>::uptr_t puoClientApp
        , typename DrawAux::Fps::Ordinary<T>::uptr_t puoDrawAuxFps
        , FrameLimiter::ABase::uptr_t puoFrameLimiter
        , typename TWndProcHolder::uptr_t puoWnd
	) 
        : m_stCtx( std::move( pustCtx ) )
        , m_puoClientApp( std::move( puoClientApp ) )
        , m_puoDrawAuxFps( std::move( puoDrawAuxFps ) )
        , m_puoFrameLimiter( std::move( puoFrameLimiter ) )
        , m_puoWnd( std::move( puoWnd ) )
        , m_psoDynamicData( Dx::DynamicData<T>::z_make_shared( ) )
	 {}
	virtual ~ABase()
	 {}
	// Run game loop, not pure("=0") to omit warning C4589 when virtual inher
	virtual int run() { 
		return ERROR_CALL_NOT_IMPLEMENTED;
	}
};

// primary template, "typename = void" for partial specialization
template<class T, typename = void> class ABaseWndProcHolderAware;

typedef Dx::Spec::Any::WindowResizerStub DxOther_t;
// the partial specialization other Dx
template<class T> 
class ABaseWndProcHolderAware<T> : public ABase<T, DxOther_t> {
 protected:
	using ABase<T, DxOther_t >::ABase;
 public:
	virtual ~ABaseWndProcHolderAware() 
	 {}
};

typedef Dx::Spec::D12::WindowResizer Dx12_t;
// the full specialization Dx12
template<> class ABaseWndProcHolderAware<DxVer::v12> : public ABase<DxVer::v12, Dx12_t> {
 protected:
	using ABase< DxVer::v12, Dx12_t >::ABase;
 public:
	virtual ~ABaseWndProcHolderAware() 
	 {}
};
} // namespace detail_

template<class T> using ABase_t = detail_::ABaseWndProcHolderAware<T>;
} // namespace prj_3d::HelloWinHlsl::GameLoop
