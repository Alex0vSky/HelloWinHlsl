// Dx/Spec/Any/WindowResizerStub.h - dummy, just smart ptr holder
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Spec::Any {
class WindowResizerStub : public Sys::Wnd::AWndProcHolder {
public:
	using AWndProcHolder::AWndProcHolder;
	typedef uptr< WindowResizerStub > uptr_t;
	void onWindowSizeChanged(UINT, UINT) override {
	}
	void toggleFullscreenWindow() override {
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::Any
