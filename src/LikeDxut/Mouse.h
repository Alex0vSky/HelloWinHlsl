// LikeDxut/Mouse.h - something about mouse
#pragma once
namespace prj_3d::HelloWinHlsl::LikeDxut { 
struct Mouse { 
    static bool getPos(HWND hWnd, POINT *pstPoint) {
		//::OutputDebugStringA( "getPos\n");
		*pstPoint = {};
		if ( !::GetCursorPos( pstPoint ) ) 
			return false;
		if ( !::ScreenToClient( hWnd, pstPoint ) )
			return false;
		RECT stRect;
		if ( !::GetClientRect( hWnd, &stRect ) )
			return false;
		if ( !::PtInRect( &stRect, *pstPoint ) )
			return false;
		// HLSL->GLSL
		//pstPoint ->y = stRect.bottom - pstPoint ->y;
		return true;
    }
};
} // namespace prj_3d::HelloWinHlsl::LikeDxut
