// Dx/Adapter/Dx11Adapter.h - work with adapter
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Adapter {
class Dx11Adapter {
	std::string m_strAdapterDescr;
	CPtr< IDXGIAdapter > m_pcDxgiAdapter;
public:
	bool prepare() {
		Sys::Hr hr;
		// Get first adapter (NvOptimusEnablement, AmdPowerXpressRequestHighPerformance) or need to use IDXGIFactory6::EnumAdapterByGpuPreference
		CPtr< IDXGIFactory > pcDxgiFactory;
		hr = ::CreateDXGIFactory( IID_PPV_ARGS( pcDxgiFactory.ReleaseAndGetAddressOf( ) ) );
		hr = pcDxgiFactory ->EnumAdapters( 0, m_pcDxgiAdapter.ReleaseAndGetAddressOf( ) );
		DXGI_ADAPTER_DESC struAdapterDesc;
		hr = m_pcDxgiAdapter ->GetDesc( &struAdapterDesc );

		std::vector< char > plainMemory( ::wcslen( struAdapterDesc.Description ) + 1 );
		::wcstombs_s( nullptr, &plainMemory[ 0 ], plainMemory.size( ), struAdapterDesc.Description, _TRUNCATE );
		m_strAdapterDescr = &plainMemory[ 0 ];

		return true;
	}
	operator IDXGIAdapter *() const {
		return m_pcDxgiAdapter.Get( );
	}
	std::string getDescrString() const {
		return m_strAdapterDescr;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Adapter
