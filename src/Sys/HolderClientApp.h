// Sys/HolderClientApp.h - helper for store value of template type
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace Sys { 
class HolderClientApp {
	union {
		const Ty::CurClientApp< DxVer::v9 > *m_pDx9;
		const Ty::CurClientApp< DxVer::v10 > *m_pDx10; 
		const Ty::CurClientApp< DxVer::v11 > *m_pDx11;
		const Ty::CurClientApp< DxVer::v12 > *m_pDx12;
		const void *m_pInitAll_;
	};
	const int m_iTypeId;
	
 public:
	template<class T, typename = typename std::enable_if_t< std::is_base_of_v< DxVerBase, T > > >
	explicit HolderClientApp(Ty::CurClientApp< T > *p) 
		: m_pInitAll_( nullptr ), m_iTypeId( T::m_version ) {
		if ( false ) {}
		else if ( DxVer::v9::m_version == m_iTypeId )
		   m_pDx9 = (Ty::CurClientApp< DxVer::v9 > *)p;
		else if ( DxVer::v10::m_version == m_iTypeId )
		   m_pDx10 = (Ty::CurClientApp< DxVer::v10 > *)p;
		else if ( DxVer::v11::m_version == m_iTypeId )
		   m_pDx11 = (Ty::CurClientApp< DxVer::v11 > *)p;
		else if ( DxVer::v12::m_version == m_iTypeId )
		   m_pDx12 = (Ty::CurClientApp< DxVer::v12 > *)p;
		else 
			Sys::ErrorHandler::showMsgAndExit( "select client app version" );
	}
	template<class T>
	typename std::enable_if_t< std::is_base_of_v< DxVerBase, T >, Ty::CurClientApp< T > * >
	static 
	get(const HolderClientApp &holder) {
		if ( T::m_version == holder.m_iTypeId )
			return (Ty::CurClientApp< T > *)holder.m_pDx9;
		return nullptr;
	}
};
} // namespace Sys
