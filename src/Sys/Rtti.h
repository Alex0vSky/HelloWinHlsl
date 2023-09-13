// Sys/Rtti.h - helper RTTI(/GR+)
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Sys { 
class Rtti {
	static const unsigned int c_uMscVer = _MSC_FULL_VER;
	// magic! Set you own version compiler of Microsoft Visual Studio
	//static const unsigned int c_uValidAbiVerForRttiCheck = 192930141; // MSVS 2019 community
	static const unsigned int c_uValidAbiVerForRttiCheck = 193431937; // MSVS 2022 community
	
 public:
	template<class T>
	static 
	std::string 
	getClassName(Ty::CurClientApp<T> *poClientApp) {
		std::string strClassName;
		// Or there will be an exception if the client disables the RTTI(/GR-) 
		//	library, and then it cannot be fixed by defining _CPPRTTI, 
		//	because this *.obj has already been compiled.		
		//	@insp C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\crt\src\vcruntime\rtti.cpp#GetCompleteObjectLocatorFromObject // NOLINT(whitespace/line_length)
		//	only for my studio version.
#ifdef _CPPRTTI
		if constexpr ( c_uMscVer == c_uValidAbiVerForRttiCheck ) {
			if ( static_cast< void*** >( reinterpret_cast<void *>( poClientApp ) )[0][-1] )
				strClassName = typeid( *poClientApp ).name( );
		}
#endif // _CPPRTTI
		// remove "class " substring in begin string
		if ( strClassName.length( ) ) // cppcheck-suppress knownConditionTrueFalse
			strClassName = strClassName.substr( _countof( "class " ) - 1 );
		return strClassName;
	}

	template<class T1, class T2>
	static 
	bool
	IsTypesEqual() {
		return typeid( T1 ).hash_code( ) == typeid( T2 ).hash_code( );
	}
};
} // namespace prj_3d::HelloWinHlsl::Sys
