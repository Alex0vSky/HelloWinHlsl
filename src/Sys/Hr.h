// Sys/Hr.h - throws an exception when an erroneous "HRESULT" is assigned.
// @insp https://stackoverflow.com/questions/46416029/finding-the-calling-functions-address-in-visual-c-safely
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Sys {
class Hr {
    HRESULT m_hr;
	_bstr_t m_bstrDescription;
    void check(void *xAddr) {
        if ( S_OK == m_hr )
            return;

        _com_error oComError( m_hr );
        //m_bstrDescription = oComError.Description( );
        m_bstrDescription = oComError.ErrorMessage( );
	        //throw oComError;

		// TODO(Alex0vSky): convert all known values to string representation, 
		//	like 0x8876086c to "D3DERR_INVALIDCALL"
		//	@insp https://github.com/brainexcerpts/Dxerr
        ICreateErrorInfo* createErrInfo = NULL;
        IErrorInfo* errInfo = NULL;
        //HRESULT hr;
        ::CreateErrorInfo( &createErrInfo ); // hr
        createErrInfo ->SetGUID( IID_IErrorInfo ); // hr
        std::wstringstream stream;
        stream << m_bstrDescription.operator wchar_t *() << std::endl;
        stream << L"ret 0x" 
			<< std::hex << std::uppercase 
			<< reinterpret_cast<size_t>( xAddr ) << std::endl;
        _bstr_t bstrDesc( stream.str( ).c_str( ) );
        createErrInfo ->SetDescription( bstrDesc ); // hr
        createErrInfo ->QueryInterface( IID_IErrorInfo, reinterpret_cast<void**>( &errInfo ) ); // hr
        //try {
	        throw _com_error( m_hr, errInfo );
        //}
        //catch (const _com_error &oComError)
        //{
        //    auto ifcErrorInfo = oComError.ErrorInfo( );
        //    BSTR bstrDescription;
        //    hr = ifcErrorInfo ->GetDescription( &bstrDescription );
        //    if ( S_OK == hr ) 
        //        SysFreeString( bstrDescription );
        //}
    }

 public:
    Hr() 
        :m_hr ( S_OK )
    {}
	// Its converting constructor
    Hr(HRESULT hrValue) // cppcheck-suppress noExplicitConstructor; NOLINT(runtime/explicit)
        :m_hr ( hrValue ) {
        check( _ReturnAddress( ) );
    }
    // non copy-and-swap idiom
    Hr &operator= (HRESULT hrValue) {
        m_hr = hrValue;
        check( _ReturnAddress( ) );
        return *this;
    }
    operator HRESULT() {
        return m_hr;
    }
	// or -fno-elide-constructor
	Hr(const Hr&& rhs) 
		: m_hr( rhs.m_hr ) 
		, m_bstrDescription( rhs.m_bstrDescription ) 
    {}
    // NonCopyable almost -- "Hr() = default;"
    Hr(const Hr&) = delete;
    Hr & operator=(const Hr&) = delete;
};
} // namespace prj_3d::HelloWinHlsl::Sys
