// _pch\dx.h
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

#pragma warning( disable: 26812 )
#pragma warning( disable: 26495 )
#ifdef __clang__
#	pragma clang diagnostic ignored "-Wnew-returns-null"
#	pragma clang diagnostic ignored "-Wclass-conversion"
#	pragma clang diagnostic ignored "-Wmissing-braces"
#endif // __clang__

#ifndef A0S_cppcheck__
// toDrawFps
#	include <d2d1.h>
#	include <dwrite.h>
// DirectX 9
#	include <d3d9.h>
#	include "d3dx9shader.h"
// DirectX 10
#	include <d3d10_1.h>
//	for D3DX10CreateEffectFromXXX, d3dx10_43.dll!_D3DX10CreateEffectFromMemory@56
#	include "d3dx10async.h"

// DirectX 11
#	include <d3d11.h>
#	include <d3dcompiler.h>

// DirectX 12
#	include <d3d12.h>
#	include <D3Dcompiler.h>
#	include <dxgi1_6.h>
//	MIT https://github.com/microsoft/DirectX-Headers/blob/main/include/directx/d3dx12.h
#	define D3DX12_NO_STATE_OBJECT_HELPERS
#	define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
//	because old d3dcommon.h, and dont have D3D_FORMAT_LAYOUT declatation
#	define __D3D12_PROPERTY_LAYOUT_FORMAT_TABLE_H__
#	include "d3dx12.h"
#	include <DirectXMath.h>

#	define HAS_DIRECTXTK12
#	include "DirectXTK12\SpriteFont.h"
#	include "DirectXTK12\DescriptorHeap.h"
#	include "DirectXTK12\ResourceUploadBatch.h"
#	include "DirectXTK12\SimpleMath.h"
//d3dx12_core.h
#	ifdef _DEBUG
#		include <dxgidebug.h>
#	endif
#endif // A0S_cppcheck__

