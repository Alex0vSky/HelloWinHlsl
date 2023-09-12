// stdafx.h - pch

// disable min and max macros
#define NOMINMAX
// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
#	ifndef D3D_DEBUG_INFO
#		define D3D_DEBUG_INFO
#	endif
#endif

#pragma warning( push )
#ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmicrosoft-include"
#endif // __clang__

#include "sysw.h"
#include "dx.h"

#ifdef __clang__
#	pragma clang diagnostic pop
#endif // __clang__
#pragma warning( pop )

#include "HelloWinHlsl.h"
