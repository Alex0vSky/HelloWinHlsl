// Dx/Adapter/Dx12Adapter.h - work with adapter
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Adapter {
class Dx12Adapter {
	std::string m_strAdapterDescr;
	CPtr< IDXGIAdapter1 > m_pcDxgiAdapter;
	CPtr< IDXGIFactory6 > m_pcDxgiFactory6;
public:
	bool prepare() {
		Sys::Hr hr;
	    UINT dxgiFactoryFlags = 0;
		//* 
#ifdef _DEBUG
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			CPtr< ID3D12Debug > D3D12Debug;
			HRESULT hr_;
			hr_ = ::D3D12GetDebugInterface( IID_PPV_ARGS( D3D12Debug.ReleaseAndGetAddressOf( ) ) );
			if ( SUCCEEDED( hr_ ) ) {
				D3D12Debug ->EnableDebugLayer( );
				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}

		/*
		CPtr< IDXGIInfoQueue > dxgiInfoQueue;
		if (DXGIGetDebugInterface1 != nullptr && SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			hr = dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
			hr = dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			//dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				// IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides.
				//80 
				// D3D Error 887e0003: (3224@711816656) at 00007FFDDE460ED5
				// D3D12 SDKLayers dll does not match the D3D12SDKVersion of D3D12 Core dll.
				//0x887e0003
				3224
			};
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			//hr = dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
			D3D12_MESSAGE_ID_DRAW_EMPTY_SCISSOR_RECTANGLE;
			hr = dxgiInfoQueue ->AddStorageFilterEntries( DXGI_DEBUG_ALL, &filter );
		}//*/

#endif // _DEBUG
		//*/
		{
			CPtr< IDXGIFactory4 > pcDxgiFactory4;
			hr = ::CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS( pcDxgiFactory4.ReleaseAndGetAddressOf( ) ) );
			hr = pcDxgiFactory4 ->QueryInterface( IID_PPV_ARGS( m_pcDxgiFactory6.ReleaseAndGetAddressOf( ) ) );
		}
		// No more need to export symbols NvOptimusEnablement and AmdPowerXpressRequestHighPerformance
		DXGI_ADAPTER_DESC1 struAdapterDesc;
		HRESULT hr_ = m_pcDxgiFactory6 ->EnumAdapterByGpuPreference(
				  0
#ifdef _DEBUG
				// For my AMD Radeon most informative. Full compartible PIX, but disallow AltEnter
				, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_MINIMUM_POWER
#else // _DEBUG
				, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
#endif // _DEBUG
				, IID_PPV_ARGS( m_pcDxgiAdapter.ReleaseAndGetAddressOf( ) )
			);
		// if "Capture Adapter" when "Start Graphics Debugging"
		if ( FAILED( hr_ ) )
			hr = m_pcDxgiFactory6 ->EnumAdapters1( 0, m_pcDxgiAdapter.ReleaseAndGetAddressOf( ) );
		hr = m_pcDxgiAdapter ->GetDesc1( &struAdapterDesc );

		std::vector< char > plainMemory( ::wcslen( struAdapterDesc.Description ) + 1 );
		::wcstombs_s( nullptr, &plainMemory[ 0 ], plainMemory.size( ), struAdapterDesc.Description, _TRUNCATE );
		m_strAdapterDescr = &plainMemory[ 0 ];

		return true;
	}
	operator IDXGIAdapter1 *() const {
		return m_pcDxgiAdapter.Get( );
	}
	IDXGIFactory6 *operator ->() const {
		return m_pcDxgiFactory6.Get( );
	}
	std::string getDescrString() const {
		return m_strAdapterDescr;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Adapter
