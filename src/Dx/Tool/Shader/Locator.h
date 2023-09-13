// Dx/Tool/Shader/Locator.h - shader file locator
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader {
struct Locator {
	static std::wstring locate(const std::wstring &strRelFileName) {
		std::filesystem::path path = std::filesystem::current_path( );
		path /= strRelFileName;
		if ( std::filesystem::exists( path ) ) {
			return path;
		}

		WCHAR wcsPath[ MAX_PATH + 1 ] = { };
		DWORD dwSize = ::GetModuleFileNameW( nullptr, wcsPath, MAX_PATH );
		if ( dwSize == 0 || dwSize == MAX_PATH )
			throw std::exception( "Method failed or wcsPath was truncated" );
		path = std::filesystem::path( wcsPath ).parent_path( ) / strRelFileName;
		if ( std::filesystem::exists( path ) ) {
			return path;
		}
		//throw std::exception( "Cant locate file" );
		return {};
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::Shader
