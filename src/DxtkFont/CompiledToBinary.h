// DxtkFont/CompiledToBinary.h - font storage

// Font file
// @insp stackoverflow.com/questions/8707183/how-can-i-convert-a-binary-file-to-the-text-declaring-a-c-c-array-with-that-co
// HowTo reproduce:
//	generate binary font file "myfile.spritefont", font family is Arial, font size is 28:
//		[DirectXTK12\MakeSpriteFont.exe "Arial" myfile.spritefont /FontSize:28]
//	convert "myfile.spritefont" to c header via python:
//		[py -c "import sys;a=sys.argv;open(a[2],'wb').write(('const uint8_t '+a[3]+'[] = {'+','.join([hex(b) for b in open(a[1],'rb').read()])+'};').encode('utf-8'))" myfile.spritefont DXTKfont_Arial28.h s_lpDXTKfont_Arial28]
namespace prj_3d::HelloWinHlsl::DxtkFont {
struct CompiledToBinary {
	static const auto & getArial28() {
		static 
#ifndef __INTELLISENSE__
#	include "../resource/DXTKfont_Arial28.h"
#endif // __INTELLISENSE__
		return s_lpDXTKfont_Arial28;
	}
};
} // namespace prj_3d::HelloWinHlsl::DxtkFont
