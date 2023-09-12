// Sys/Wnd/IResizebleWindow.h - interface for resizeble window
#pragma once
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
struct IResizebleWindow {
	virtual void onWindowSizeChanged(UINT width, UINT height) = 0;
	virtual void toggleFullscreenWindow() = 0;
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
