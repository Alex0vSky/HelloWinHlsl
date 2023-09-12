// Sys/ErrorHandler.h - error handler
#pragma once
namespace prj_3d::HelloWinHlsl::Sys {
//_Noreturn 
class ErrorHandler {
public:
	// show error message and exit
	__declspec(noreturn) 
	static 
	void showMsgAndExit(const std::string &strMsg, UINT uCode = E_UNEXPECTED) { 
		::MessageBoxA( nullptr, ( "Fail: " + strMsg ).c_str( ), "Error", MB_ICONERROR );
		::ExitProcess( uCode );
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys
