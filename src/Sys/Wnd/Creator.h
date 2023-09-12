// Sys/Wnd/Creator.h - create window
#pragma once
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
namespace detail_ {
template<class TResizer>
struct BaseCreator {
	// Create window in current thread
	template<class T>
	static 
	typename TResizer::uptr_t
	inCurrentThread(HINSTANCE hInstance, Ty::CurClientApp<T> *poClientApp) {
		auto upWndProc = Sys::Wnd::Proc::z_make_unique( hInstance );

		UINT uWidth = 800, uHeight = 600;
		WNDCLASSEXW wcex = { }; // C++20 { .cbSize = sizeof( WNDCLASSEX ) };
		wcex.cbSize = sizeof( wcex );
		wcex.lpfnWndProc = (WNDPROC)&Sys::Wnd::Proc::systemCallback;
		wcex.hInstance = hInstance;
		wcex.hIcon = ::LoadIconW( hInstance, MAKEINTRESOURCE( IDI_HELLOWINHLSL ) );
		wcex.hCursor = ::LoadCursorW( nullptr, IDC_ARROW );
		wcex.hbrBackground = HBRUSH( COLOR_WINDOW + 1 );
		wcex.lpszMenuName = MAKEINTRESOURCE( IDC_HELLOWINHLSL );
		wcex.lpszClassName = TEXT( "CLASS_HELLOWINHLSL" );
		wcex.hIconSm = ::LoadIconW( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );
		if ( !::RegisterClassExW( &wcex ) )
			return { };
		RECT stRectDesk = { };
		if ( !::GetWindowRect( ::GetDesktopWindow( ), &stRectDesk ) )
			return { };
		UINT windowStyle = WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_SIZEBOX | WS_MAXIMIZEBOX;
		RECT stWindowRect = {
				( stRectDesk.right / 2 - (LONG)uWidth / 2 )
				, ( stRectDesk.bottom / 2 - (LONG)uHeight / 2 )
			};
		stWindowRect.right = stWindowRect.left + uWidth;
		stWindowRect.bottom = stWindowRect.top + uHeight;
		HWND hWnd = ::CreateWindowExW(
				0, wcex.lpszClassName, TEXT( "HelloWinHlsl" )
				, windowStyle
				, stWindowRect.left, stWindowRect.top, uWidth, uHeight
				, nullptr, nullptr, hInstance
				, upWndProc.get( )
			);
		if ( !hWnd )
			return { };
		Sys::Wnd::StToken stToken{
			hWnd
			, uWidth
			, uHeight
			, HelloWinHlsl::Sys::Rtti::getClassName<T>( poClientApp )
			, windowStyle
			, stWindowRect
		};
		return std::make_unique< TResizer > ( 
				stToken
				, upWndProc
			);
	}
};
} // namespace detail_ _
class Creator {
	typedef Dx::Spec::D12::WindowResizer ResizerDx12_t;
	typedef Dx::Spec::Any::WindowResizerStub ResizerDxOther_t;
 public:
	// Create window in current thread, for Dx12 spec windows resizing
	template<class T>
	static
	std::enable_if_t< ( T::m_version < 12 ), ResizerDxOther_t::uptr_t >
	inCurrentThread(HINSTANCE hInstance, Ty::CurClientApp<T> *poClientApp) {
		return detail_::BaseCreator<ResizerDxOther_t>::inCurrentThread( hInstance, poClientApp );
	}
	// Create window in current thread
	template<class T>
	static
	std::enable_if_t< ( T::m_version == 12 ), ResizerDx12_t::uptr_t >
	inCurrentThread(HINSTANCE hInstance, Ty::CurClientApp<T> *poClientApp) {
		return detail_::BaseCreator<ResizerDx12_t>::inCurrentThread( hInstance, poClientApp );
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
