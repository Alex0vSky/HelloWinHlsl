// Sys/Wnd/AWndProcHolder.h - for first ist holder smart ptr with WndProc functional
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
class AWndProcHolder : public Sys::Wnd::IResizebleWindow {
	StToken m_stWndToken;
	Sys::Wnd::Proc::uptr_t m_upWndProc;
 public:
	AWndProcHolder(
		const Sys::Wnd::StToken &stWndToken
		, Sys::Wnd::Proc::ref_t refWndProc
	)
		: m_stWndToken( stWndToken )
		, m_upWndProc( std::move( refWndProc ) ) {
		m_upWndProc ->setResizebleWindow( this );
	}
	const Sys::Wnd::StToken &getWndToken() const {
		return m_stWndToken;
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
