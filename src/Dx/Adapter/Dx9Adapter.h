// Dx/Adapter/Dx9Adapter.h - work with adapter
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Adapter {
class Dx9Adapter {
	D3DADAPTER_IDENTIFIER9 m_struIdentifier;
	UINT m_uAdapterIdx;
	
public:
	Dx9Adapter()
		: m_uAdapterIdx( 0 )
	{}
	bool prepare() {
		CPtr< IDirect3D9 > pcD3D;
		if( !( *pcD3D.ReleaseAndGetAddressOf( ) = ::Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return false;
		D3DCAPS9 Caps;
		Sys::Hr hr;
		UINT uCnt = pcD3D ->GetAdapterCount( );
		for ( m_uAdapterIdx = 0; m_uAdapterIdx < uCnt; ++m_uAdapterIdx ) { 
			if ( SUCCEEDED( pcD3D ->GetDeviceCaps( m_uAdapterIdx, D3DDEVTYPE::D3DDEVTYPE_HAL, &Caps ) ) ) { // any hardware
				ZeroMemory( &m_struIdentifier, sizeof( m_struIdentifier ) );
				hr = pcD3D ->GetAdapterIdentifier( m_uAdapterIdx, 0, &m_struIdentifier );
				break;
			}
		}
		return ( m_uAdapterIdx < uCnt );
	}
	UINT getIdx() const {
		return m_uAdapterIdx;
	}
	std::string getDescrString() const {
		return m_struIdentifier.Description;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Adapter
