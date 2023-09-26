// SdfFont/CompiledToBinary.h - font storage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)

// Font file
// @insp stackoverflow.com/questions/8707183/how-can-i-convert-a-binary-file-to-the-text-declaring-a-c-c-array-with-that-co NOLINT(whitespace/line_length)
// HowTo reproduce:
//	generate binary font file "Arial28.fnt", font family is Arial, font size is 28:
//		[microsoft/DirectX-Graphics-Samples/MiniEngine/Tools/SDFFontCreator/...xxx.../SDFFontCreator.exe C:\Windows\Fonts\arial.ttf -character_set "ASCII" -size 28 -output Arial28] // NOLINT
//	convert "Arial28.fnt" to c header via python:
//		[py -c "import sys;a=sys.argv;open(a[2],'wb').write(('const uint8_t '+a[3]+'[] = {'+','.join([hex(b) for b in open(a[1],'rb').read()])+'};').encode('utf-8'))" Arial28.fnt SdfFont_Arial28.h s_lpSdfFont_Arial28] NOLINT(whitespace/line_length)
namespace prj_3d::HelloWinHlsl::SdfFont {
struct CompiledToBinary {
	static const auto & getArial28() {
		static 
#ifndef __INTELLISENSE__
#	include "../resource/SdfFont_Arial28.h"
#endif // __INTELLISENSE__
		return s_lpSdfFont_Arial28;
	}
};
} // namespace prj_3d::HelloWinHlsl::SdfFont
