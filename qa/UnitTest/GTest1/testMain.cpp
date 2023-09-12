#include "stdafx.h"

namespace { class Environment0 : public ::testing::Environment {
	void SetUp() override {
		// Take all working sources to *.pdb file for OpenCppCoverage
		if ( false ) HelloWinHlsl::Main::run( nullptr );
	}
}; auto g_unusedMain_ = testing::AddGlobalTestEnvironment( new Environment0 ); }


// Nvidia and Amd cards will run executables preferring (first ordinal) their adapters.
extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; }

using namespace prj_3d::HelloWinHlsl;
using namespace prj_3d::HelloWinHlsl::ClientTy;
using CurDxVer = DxVer::v9;
template<class T> class testClientApp; // primary template
template<> class testClientApp<DxVer::v9> : public CurClientApp<DxVer::v9> {
	using T = DxVer::v9;
	bool init(DxCtx<T>::cref_ptr_t, ToolCtx<T>::cref_ptr_t, Adjust<T>*) {
		return true;
	}
	void render_frame(DxCtx<T>::cref_ptr_t, Dynamic<T>::cref_ptr_t) {
	}
};
uptr< Sys::HolderClientApp > ClientTy::entryPoint() { return std::make_unique< Sys::HolderClientApp >( new testClientApp<CurDxVer>( ) ); }
Config::uptrc_t ClientTy::configure() { return ClientApp::Configurator::Predef::getDefault( ); }
