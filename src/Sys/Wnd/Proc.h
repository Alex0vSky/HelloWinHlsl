// Sys/Wnd/Proc.h - window procedure like object, via SetWindowLongPtrA+GetWindowLongPtrA
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
class AWndProcHolder;
class Proc {
	HWND m_hWnd;
	const HINSTANCE m_hInstance;
	IResizebleWindow *m_ifcResizebleWindow;
	bool m_bMinimized = false;
	bool m_bInSizemove = false;

	friend class AWndProcHolder;
	void setResizebleWindow(IResizebleWindow *ifcResizebleWindow) {
		m_ifcResizebleWindow = ifcResizebleWindow;
	}

	// Message handler for about box.
	static INT_PTR CALLBACK About_(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
		UNREFERENCED_PARAMETER( lParam );
		switch ( message ) {
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if ( IDOK == LOWORD( wParam ) || IDCANCEL == LOWORD( wParam ) ) {
				EndDialog( hDlg, LOWORD( wParam ) );
				return (INT_PTR)TRUE;
			}
			break;
		}
		return (INT_PTR)FALSE;
	}

	//  PURPOSE: Processes messages for the main window.
	LRESULT messagesHandler_(UINT message, WPARAM wParam, LPARAM lParam) {
		RECT rc;
		switch (message) {
		// Fullscreen message here
		// TODO(Alex0vSky): why not real time?
//https://stackoverflow.com/questions/28406919/redraw-window-using-directx-during-move-resize
		case WM_MOVE:
			if ( m_ifcResizebleWindow ) {
				::OutputDebugStringA( "WM_MOVE\n" );
				::GetWindowRect( m_hWnd, &rc );
				m_ifcResizebleWindow ->onWindowSizeChanged( rc.right - rc.left, rc.bottom - rc.top );
			}
			break;
		case WM_SIZE:
			if ( SIZE_MINIMIZED == wParam ) {
				m_bMinimized = true;
			} else if ( m_bMinimized ) {
				m_bMinimized = false;
			} else if ( !m_bInSizemove && m_ifcResizebleWindow ) {
				// affect only when InSeparateThread render and window message handler
				::OutputDebugStringA( "WM_SIZE\n" );
				m_ifcResizebleWindow ->onWindowSizeChanged( LOWORD( lParam ), HIWORD( lParam ) );
			}
			break;
		// End of resize
		case WM_EXITSIZEMOVE:
			m_bInSizemove = false;
			if ( m_ifcResizebleWindow ) {
				::OutputDebugStringA( "WM_EXITSIZEMOVE\n" );
				::GetWindowRect( m_hWnd, &rc );
				m_ifcResizebleWindow ->onWindowSizeChanged( rc.right - rc.left, rc.bottom - rc.top );
			}
			break;
		case WM_SYSKEYDOWN:
			// Handle ALT+ENTER:
			if ((wParam == VK_RETURN) && (lParam & (1 << 29))) {
				if ( m_ifcResizebleWindow ) {
					m_ifcResizebleWindow ->toggleFullscreenWindow( );
				}
				return 0;
			}
			// Send all other WM_SYSKEYDOWN messages to the default WndProc.
			break;

		case WM_COMMAND:
			 {
				int wmId = LOWORD(wParam);
				// Parse the menu selections:
				switch (wmId) {
				case IDM_ABOUT:
					::DialogBoxParamW( 
							m_hInstance
							, MAKEINTRESOURCE( IDD_ABOUTBOX )
							, m_hWnd
							, About_
							, 0 
						);
					break;
				case IDM_EXIT:
					::DestroyWindow( m_hWnd );
					break;
				default:
					return ::DefWindowProc( m_hWnd , message, wParam, lParam);
				}
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			break;
		}
		return ::DefWindowProc( m_hWnd, message, wParam, lParam);
	}

	void setHwnd_(HWND hWnd) {
		m_hWnd = hWnd;
	}

 public:
	explicit Proc(HINSTANCE hInstance)
		: m_hWnd( nullptr )
		, m_hInstance( hInstance )
		, m_ifcResizebleWindow( nullptr )
	 {}
	typedef uptr< Proc > uptr_t;
	template<typename... Args> static auto z_make_unique(Args&&... args) {
		return std::make_unique< Proc >( std::forward<Args>(args)... );
	}
	typedef uptr_t &ref_t;

	static LRESULT systemCallback(HWND hWndMsg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Proc *self = nullptr;
		if ( WM_NCCREATE == uMsg ) { 
			// TODO(Alex0vSky): CREATESTRUCTW
			CREATESTRUCTA *pStruct = reinterpret_cast< CREATESTRUCTA * >( lParam );
			self = reinterpret_cast< Proc * >( pStruct ->lpCreateParams );

			self ->setHwnd_( hWndMsg );
			::SetLastError( 0 );
			LONG_PTR previousOrError = ::SetWindowLongPtrA( 
					hWndMsg
					, GWLP_USERDATA
					, reinterpret_cast< LONG_PTR >( self ) 
				);
			if ( !previousOrError && 0 != ::GetLastError( ) ) 
				return FALSE;
		} else {
			self = reinterpret_cast< Proc * >( ::GetWindowLongPtrA( hWndMsg, GWLP_USERDATA ) );
		}
		if ( self ) 
			return self ->messagesHandler_( uMsg, wParam, lParam );
		return ::DefWindowProcA( hWndMsg, uMsg, wParam, lParam );
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
