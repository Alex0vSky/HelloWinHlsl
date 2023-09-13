// HelloWinHlsl.cpp - code designed to get acquainted with the difference 
//	in working with shaders for all versions of Dx.
// Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include <stdafx.h>
#include "HelloWinHlsl.h"

// Nvidia and Amd cards will run executables preferring (first ordinal) their adapters.
extern "C" { 
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int) {
	return prj_3d::HelloWinHlsl::Main::run( hInst );
}
