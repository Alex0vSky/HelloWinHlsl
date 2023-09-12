// GameLoop/BaseInSeparateThread.h - base logic for game loop in new/separeate thread
#pragma once
namespace prj_3d::HelloWinHlsl::GameLoop {
namespace detail_ {

// Use access to parent members via "this ->" or "ABase_t::"
//	to avoid write "using ABase<T>::blah" for each member
//@insp www.modernescpp.com/index.php/surprise-included-inheritance-and-member-functions-of-class-templates/
//@insp isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
template<class T>
class BaseInSeparateThread_ : virtual public ABaseWndProcHolderAware<T> {
	using base_t = ABaseWndProcHolderAware<T>;

 protected:
	using mutex_t = typename ABaseWndProcHolderAware<T>::mutex_t;
	using base_t::ABaseWndProcHolderAware;
	std::atomic< mutex_t *> m_pMutex = nullptr;
	typedef std::lock_guard< mutex_t > lock_t;
	void setSynchronization(mutex_t *pMutex) {
		m_pMutex = pMutex;
	}

	int wndMessageAndClientCall() {
		std::atomic_bool bStopThread = { false };
		std::thread oMsgHandleThread(
				[&]() {
#ifdef _DEBUG
					::SetThreadDescription( ::GetCurrentThread(), L"GameLoop" );
#endif // _DEBUG
					do {
						uptr< lock_t > scoped;
						if ( m_pMutex )
							scoped = std::make_unique< lock_t >( *m_pMutex );

						if ( base_t::m_pfnCallbackBeforeRender )
							base_t::m_pfnCallbackBeforeRender( );
						base_t::m_puoClientApp ->render_frame( base_t::m_stCtx ->m_psstDxCtx, base_t::m_psoDynamicData ->forClient( ) );

						// show fps
						if ( base_t::m_puoDrawAuxFps )
							base_t::m_puoDrawAuxFps ->renderOnTarget( );

						if ( base_t::m_pfnCallbackAfterRender )
							base_t::m_pfnCallbackAfterRender( );
						// limit fps
						base_t::m_puoFrameLimiter ->limit( );

						base_t::m_puoClientApp ->present_to_screen( base_t::m_stCtx ->m_psstDxCtx, base_t::m_psoDynamicData ->forClient( ) );
						if ( base_t::m_pfnCallbackAfterPresent ) 
							base_t::m_pfnCallbackAfterPresent( );
					} while ( !bStopThread );
				} 
			);

		MSG msg = {};
		while ( GetMessage( &msg, NULL, 0, 0 ) ) {
			//::OutputDebugStringA( "message\n" );
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		bStopThread = true;
		oMsgHandleThread.join( );
        return (int) msg.wParam;
	}

public:
	virtual ~BaseInSeparateThread_() 
	{}
};
} // namespace detail_;
} // namespace prj_3d::HelloWinHlsl::GameLoop
