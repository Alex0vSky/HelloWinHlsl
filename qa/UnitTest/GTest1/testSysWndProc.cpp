#include "stdafx.h"

// @insp http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html
// @insp https://gist.github.com/dabrahams/1528856
// -TODO: trait getPrivateMember<TClass, TAddr, TTypeMember>
// -TODO: trait callPrivateMethod<TClass, TAddr, TMethodSignatire>
namespace BreakingContract {
template <class Tag> struct stowed { 
	typename Tag::TplArg_type value;
};
template <class Tag, typename Tag::TplArg_type ptm> struct stow_private {
	stowed<Tag> m_stowed;
	stow_private() {
		m_stowed.value = ptm;
	}
};

// Usage static modif
template<typename Tag>
struct result {
  typedef typename Tag::type type;
  static type ptr;
};
template<typename Tag>
typename result<Tag>::type result<Tag>::ptr;
template<typename Tag, typename Tag::type p>
struct rob : result<Tag> {
  struct filler {
    filler() { result<Tag>::ptr = p; }
  };
  static filler filler_obj;
};
template<typename Tag, typename Tag::type p>
typename rob<Tag, p>::filler rob<Tag, p>::filler_obj;

}; // namespace BreakingContract

   // realization
typedef HelloWinHlsl::Sys::Wnd::Proc clazz_t;

typedef HWND member_t;
struct getPrivateMember { typedef member_t (clazz_t::*TplArg_type); };
template<> struct BreakingContract::stow_private< getPrivateMember, &clazz_t::m_hWnd >;

typedef HelloWinHlsl::Sys::Wnd::IResizebleWindow *arg0_t;
typedef void return_t;
struct callPrivateMethod { typedef return_t(clazz_t::*type)(arg0_t); };
template struct BreakingContract::rob< callPrivateMethod, &clazz_t::setResizebleWindow>;



namespace testSysWndProc {

struct WndProcPublicMember : public HelloWinHlsl::Sys::Wnd::Proc { 
	using Proc::Proc;
public:
	HWND getHwnd() {
		HWND hWnd = 0;
		hWnd = this ->*(BreakingContract::stowed< getPrivateMember >( ).value);
		return hWnd;
	}
	// TODO: enough inher from class AWndProcHolder
	void setResizebleWindow(HelloWinHlsl::Sys::Wnd::IResizebleWindow *ifcResizebleWindow) {
		(this ->*BreakingContract::result< callPrivateMethod >::ptr)( ifcResizebleWindow );
	}
};

struct ResizebleWindowMock : public HelloWinHlsl::Sys::Wnd::IResizebleWindow {
	MOCK_METHOD(void, onWindowSizeChanged, (UINT width, UINT height), (override) );
	MOCK_METHOD(void, toggleFullscreenWindow, (), (override) );
};

struct WndHidden {
	static constexpr auto c_wcsClassName = TEXT( "testCLASS_HELLOWINHLSL" );
	static LRESULT systemCallback(HWND hWndMsg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return ::DefWindowProc( hWndMsg, uMsg, wParam, lParam );
	}
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	WndHidden(HINSTANCE hInstance) 
		: m_hInstance( hInstance )
	{
		UINT uWidth = 800, uHeight = 600;
		WNDCLASSEXW wcex = { };
		wcex.cbSize = sizeof( wcex );
		wcex.lpfnWndProc = (WNDPROC)&systemCallback;
		wcex.hInstance = m_hInstance;
		wcex.lpszClassName = c_wcsClassName;
		if ( !::RegisterClassExW( &wcex ) )
			throw;
		m_hWnd = ::CreateWindowExW(
				0, wcex.lpszClassName, TEXT( "testHelloWinHlsl" )
				, WS_OVERLAPPED
				, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
				, nullptr, nullptr, m_hInstance
				, nullptr
			);
	}
	~WndHidden() {
		BOOL b;
		b = ::DestroyWindow( m_hWnd );
		b = ::UnregisterClassW( c_wcsClassName, m_hInstance );
	}
};
struct Context : public ::testing::Test {
	HINSTANCE m_hInstance;
	WndProcPublicMember m_oWndProcMock;
	WndHidden m_oWndHidden;
	CREATESTRUCTW m_lpCreateStruct{ };
	ResizebleWindowMock m_oResizebleWindowMock;
	Context()
		: m_hInstance( GetModuleHandle( nullptr ) )
		, m_oWndProcMock( m_hInstance )
		, m_oWndHidden( m_hInstance )
	{}
};

typedef Context SysWndProc;
TEST_F(SysWndProc, init) {
	m_lpCreateStruct.lpCreateParams = &m_oWndProcMock;
	HWND m_hWnd = m_oWndHidden.m_hWnd;
	EXPECT_EQ( 1, WndProcPublicMember::systemCallback( m_hWnd, WM_NCCREATE, 0, LPARAM( &m_lpCreateStruct ) ) );
	EXPECT_EQ( m_hWnd, m_oWndProcMock.getHwnd( ) );
}

struct SysWndProcCreated : public Context {
	HWND m_hWnd;
	RECT m_rc;
	SysWndProcCreated()
	{
		m_lpCreateStruct.lpCreateParams = &m_oWndProcMock;
		m_hWnd = m_oWndHidden.m_hWnd;
		WndProcPublicMember::systemCallback( m_hWnd, WM_NCCREATE, 0, LPARAM( &m_lpCreateStruct ) );
		m_oWndProcMock.setResizebleWindow( &m_oResizebleWindowMock );
		::GetWindowRect( m_hWnd, &m_rc );
	}
};

TEST_F(SysWndProcCreated, resize_WM_MOVE) {
	EXPECT_CALL( m_oResizebleWindowMock, onWindowSizeChanged( m_rc.right - m_rc.left, m_rc.bottom - m_rc.top ) );
	EXPECT_EQ( 0, WndProcPublicMember::systemCallback( m_hWnd, WM_MOVE, 0, 0 ) );
}

TEST_F(SysWndProcCreated, resize_WM_EXITSIZEMOVE) {
	EXPECT_CALL( m_oResizebleWindowMock, onWindowSizeChanged( m_rc.right - m_rc.left, m_rc.bottom - m_rc.top ) );
	EXPECT_EQ( 0, WndProcPublicMember::systemCallback( m_hWnd, WM_EXITSIZEMOVE, 0, 0 ) );
}

TEST_F(SysWndProcCreated, resize_WM_SIZE) {
	EXPECT_CALL( m_oResizebleWindowMock, onWindowSizeChanged( 1, 2 ) );
	EXPECT_EQ( 0, WndProcPublicMember::systemCallback( m_hWnd, WM_SIZE, 0, MAKELPARAM( 1, 2 ) ) );
}

TEST_F(SysWndProcCreated, toggleFullscreenWindow) {
	EXPECT_CALL( m_oResizebleWindowMock, toggleFullscreenWindow( ) );
	EXPECT_EQ( 0, WndProcPublicMember::systemCallback( m_hWnd, WM_SYSKEYDOWN, VK_RETURN, ( (1 << 29) ) ) );
}

} // namespace testSysWndProc _
