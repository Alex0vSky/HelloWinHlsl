// Sys/Wnd/Change.h - change window
#pragma once
namespace prj_3d::HelloWinHlsl::Sys::Wnd {
class Change {
public:
	// Set usable/graceful window title
	template<class T>
	static 
	void 
	title(HWND hWnd, const std::string &strClientAppName, const std::string &strAdapterDescr) {
		// Change the window name by adding the name of the adapter.
		const UINT c_uWndTextLenMax = 128;
		std::vector< char > vecMem( c_uWndTextLenMax + 1 );
		if ( ::GetWindowTextA( hWnd, &vecMem[ 0 ], c_uWndTextLenMax ) ) { 
			std::string strWndText;
			if ( strClientAppName.length( ) ) {
				strWndText = strClientAppName;
			}
			else
				strWndText = &vecMem[ 0 ] + std::string( " [noRTTI]" );
			strWndText += " (";
			strWndText += strAdapterDescr;
			strWndText += ")";

			strWndText += " (";
			if ( false ) ;
			else if ( Sys::Rtti::IsTypesEqual< T, DxVer::v9 >( ) ) {
				strWndText += "D3D9";
			}
			else if ( Sys::Rtti::IsTypesEqual< T, DxVer::v10 >( ) ) {
				strWndText += "D3D10";
			}
			else if ( Sys::Rtti::IsTypesEqual< T, DxVer::v11 >( ) ) {
				strWndText += "D3D11";
			}
			else if ( Sys::Rtti::IsTypesEqual< T, DxVer::v12 >( ) ) {
				strWndText += "D3D12";
			}
			else 
				strWndText += "Unknown DirectX version";
			strWndText += ")";

			strWndText += " (";
#ifdef _WIN64
			strWndText += "x64";
#else // _WIN64
			strWndText += "x86";
#endif // _WIN64
			strWndText += ")";

			strWndText += " (";
#ifdef _DEBUG
			strWndText += "Debug";
#else // _DEBUG
			strWndText += "Release";
#endif // _DEBUG
			strWndText += ")";

			::SetWindowTextA( hWnd, strWndText.c_str( ) );
		}
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys::Wnd
