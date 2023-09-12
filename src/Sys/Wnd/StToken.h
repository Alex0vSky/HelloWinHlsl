// Sys/Wnd/StToken.h - ...
#pragma once
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
struct StToken {
	const HWND m_hWnd;
	const UINT m_uInitialWidth;
	const UINT m_uInitialHeight;
	const std::string m_strClientAppName;
	const UINT m_uWindowStyle;
	const RECT m_stWindowRect;

	typedef const StToken &cref_t;
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
