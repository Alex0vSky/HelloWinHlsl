// GameLoop/BaseInCurrentThread.h - base logic for game loop in current thread
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::GameLoop {
namespace detail_ {

template<class T>
class BaseInCurrentThread_ : virtual public ABaseWndProcHolderAware<T> {
	using base_t = ABaseWndProcHolderAware<T>;

 protected:
	using base_t::ABaseWndProcHolderAware;
	int wndMessageAndClientCall() {
        MSG msg = { };
        do {
			// Hangs inside DispatchMessage when I move the window, only WndProc works.
            if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage( &msg);
                DispatchMessage( &msg);
            }

			if ( base_t::m_pfnCallbackBeforeRender )
				base_t::m_pfnCallbackBeforeRender( );
			base_t::m_puoClientApp ->render_frame( 
					base_t::m_stCtx ->m_psstDxCtx
					, base_t::m_psoDynamicData ->forClient( ) 
				);

			// show fps
			if ( base_t::m_puoDrawAuxFps )
				base_t::m_puoDrawAuxFps ->renderOnTarget( );

			if ( base_t::m_pfnCallbackAfterRender )
				base_t::m_pfnCallbackAfterRender( );
            // limit fps
            base_t::m_puoFrameLimiter ->limit( );

			base_t::m_puoClientApp ->present_to_screen( 
					base_t::m_stCtx ->m_psstDxCtx
					, base_t::m_psoDynamicData ->forClient( ) 
				);
			if ( base_t::m_pfnCallbackAfterPresent ) {
				base_t::m_pfnCallbackAfterPresent( );
			}
        } while ( WM_QUIT != msg.message );
        return static_cast<int>( msg.wParam );
	}
};
} // namespace detail_
} // namespace prj_3d::HelloWinHlsl::GameLoop

