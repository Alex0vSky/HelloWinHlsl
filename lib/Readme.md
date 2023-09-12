Libraries DirectXTK12.lib got from Msvc NuGet tool, version directxtk12_desktop_2019.2023.4.28.1
Dont take DirectXTK12 .pdb-s, because dont want to store in repo DirectXTK12 source code
Dont take DirectXTK12 ARM64-s, because dont known something about ARM64 for Dx9 and Dx10

Libraries d3dx9.lib and d3dx10.lib got from https://github.com/NovaRain/DXSDK_Collection/tree/master/DXSDK_Jun2010/Lib
Dont take d3dx9d.lib, its Dx9 debug version of library, because dont want to store in repo rary d3dx9d_*.dll

For lib\Dx\x64\Release\d3dx9.lib 
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x64/d3dx9.lib
For lib\Dx\x64\Debug\d3dx9d.lib 
	skip https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x64/d3dx9d.lib
	take Release https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x64/d3dx9.lib

For lib\Dx\x64\Release\d3dx10.lib 
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x64/d3dx10.lib
For lib\Dx\x64\Debug\d3dx10d.lib
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x64/d3dx10d.lib

For lib\Dx\x86\Release\d3dx9.lib 
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x86/d3dx9.lib
For lib\Dx\x86\Debug\d3dx9d.lib 
	skip https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x86/d3dx9d.lib
	take Release https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x86/d3dx9.lib

For lib\Dx\x86\Release\d3dx10.lib 
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x86/d3dx10.lib
For lib\Dx\x86\Debug\d3dx10d.lib 
	https://github.com/NovaRain/DXSDK_Collection/blob/master/DXSDK_Jun2010/Lib/x86/d3dx10d.lib

1