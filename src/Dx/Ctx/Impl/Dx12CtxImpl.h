// Dx/Ctx/Impl/Dx12CtxImpl.h - implementation for DirectX3D version 11
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl {
class Dx12CtxImpl {
public:
	static 
	Ty::StDxCtx_ptr< DxVer::v12 >
	create(Sys::Wnd::StToken::cref_t crstWndToken, const Dx::Adapter::Dx12Adapter &oAdapter) 
	{
		UINT Width = crstWndToken.m_uInitialWidth
			, Height = crstWndToken.m_uInitialHeight;
		float fWidth = static_cast< float >( Width )
			, fHeight = static_cast< float >( Height );
		LONG lWidth = static_cast< LONG >( Width )
			, lHeight = static_cast< LONG >( Height );

		Sys::Hr hr;

		CPtr< ID3D12Device > pcD3dDevice12;
		HRESULT hr_;
		// when	notebook out of electricty, used hybrid -- low consumption
		hr_ = ::D3D12CreateDevice(
				oAdapter
				, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_0
				, IID_PPV_ARGS( pcD3dDevice12.ReleaseAndGetAddressOf( ) )
			);
		if ( FAILED( hr_ ) )
			hr = ::D3D12CreateDevice(
					  oAdapter
					, D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
					, IID_PPV_ARGS( pcD3dDevice12.ReleaseAndGetAddressOf( ) )
				);
		BOOL allowTearing = FALSE;
		hr_ = oAdapter ->CheckFeatureSupport( DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof( allowTearing ) );
		bool bTearingSupport = SUCCEEDED( hr_ ) && allowTearing;

		// Describe and create the command queue.
		CPtr< ID3D12CommandQueue > commandQueue;
		D3D12_COMMAND_QUEUE_DESC queueDesc = { };
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hr = pcD3dDevice12 ->CreateCommandQueue(
				  &queueDesc
				, IID_PPV_ARGS( commandQueue.ReleaseAndGetAddressOf( ) )
			);
		// Describe and create the swap chain.
	    static const UINT FrameCount = 2;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
		swapChainDesc.Width = Width;
		swapChainDesc.Height = Height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
	    swapChainDesc.Flags = bTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		CPtr< IDXGISwapChain3 > swapChain3;
		{
			CPtr< IDXGISwapChain1 > swapChain1;
			hr = oAdapter ->CreateSwapChainForHwnd(
					// Swap chain needs the queue so that it can force a flush on it.
					  commandQueue.Get( )
					, crstWndToken.m_hWnd
					, &swapChainDesc
					, nullptr
					, nullptr
					, swapChain1.ReleaseAndGetAddressOf( )
				);
			// Allow AltEnter to fullscreen
			//hr = oAdapter ->MakeWindowAssociation( crstWndToken.m_hWnd, 0 ); // not sent even WM_MOVE
			if ( bTearingSupport )
				hr = oAdapter ->MakeWindowAssociation( crstWndToken.m_hWnd, DXGI_MWA_NO_ALT_ENTER );
			hr = swapChain1.As( std::addressof( swapChain3 ) );
		}

		// Synchronization objects.
		UINT frameIndex;
		frameIndex = swapChain3 ->GetCurrentBackBufferIndex( );

		// Pipeline objects.
		UINT rtvDescriptorSize;
		// Create descriptor heaps.
		CPtr< ID3D12DescriptorHeap > pcRtvHeap;
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = { };
			rtvHeapDesc.NumDescriptors = FrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			hr = pcD3dDevice12 ->CreateDescriptorHeap(
					  &rtvHeapDesc
					, IID_PPV_ARGS( pcRtvHeap.ReleaseAndGetAddressOf( ) )
				);

			rtvDescriptorSize = pcD3dDevice12 ->GetDescriptorHandleIncrementSize( 
					D3D12_DESCRIPTOR_HEAP_TYPE_RTV 
				);
		}
		// Pipeline objects.
		//	Adventage "std::array" its simple assign
		std::array< CPtr< ID3D12Resource >, FrameCount > renderTargets;
		// Create frame resources.
		{

			CD3DX12_CPU_DESCRIPTOR_HANDLE oRtvHandle( 
					pcRtvHeap ->GetCPUDescriptorHandleForHeapStart( ) 
				);
			// Create a RTV for each frame.
			for (UINT n = 0; n < FrameCount; n++) {
				hr = swapChain3 ->GetBuffer( 
						  n
						, IID_PPV_ARGS( renderTargets[ n ].ReleaseAndGetAddressOf( ) ) 
					);
				pcD3dDevice12 ->CreateRenderTargetView( 
						  renderTargets[ n ].Get( )
						, nullptr
						, oRtvHandle
					);
				oRtvHandle.Offset( 1, rtvDescriptorSize );
			}
		}
		// end "void D3D12HelloTriangle::LoadPipeline()"
		
		// beg "void D3D12HelloTriangle::LoadAssets()"
		// Pipeline objects.
		CPtr< ID3D12RootSignature > rootSignature;
		// Create an some root signature.
		{ 
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init( 
					0, nullptr
					, 0
					, nullptr
					, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
				);

			CPtr< ID3DBlob > signature, error;
			hr = ::D3D12SerializeRootSignature( 
					&rootSignatureDesc
					, D3D_ROOT_SIGNATURE_VERSION_1
					, signature.ReleaseAndGetAddressOf( )
					, error .ReleaseAndGetAddressOf( )
				);
			hr = pcD3dDevice12 ->CreateRootSignature( 
					0
					, signature ->GetBufferPointer( )
					, signature ->GetBufferSize( )
					, IID_PPV_ARGS( rootSignature.ReleaseAndGetAddressOf( ) )
				);
		}

		// Synchronization objects.
		CPtr< ID3D12Fence > fence;
		HANDLE fenceEvent;
		std::array< UINT64, FrameCount > aruFenceValues = { };
		std::array< CPtr< ID3D12CommandAllocator >, FrameCount > arpcCommandAllocators;
		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		{
			hr = pcD3dDevice12 ->CreateFence(
					aruFenceValues[ frameIndex ]
					, D3D12_FENCE_FLAG_NONE
					, IID_PPV_ARGS( fence.ReleaseAndGetAddressOf( ) )
				);
			
			for (UINT n = 0; n < FrameCount; n++) {
				hr = pcD3dDevice12 ->CreateCommandAllocator(
						D3D12_COMMAND_LIST_TYPE_DIRECT
						, IID_PPV_ARGS( arpcCommandAllocators[ n ].ReleaseAndGetAddressOf( ) )
					);
			}

			// Create an event handle to use for frame synchronization.
			fenceEvent = ::CreateEvent( nullptr, FALSE, FALSE, nullptr );
			if ( !fenceEvent ) {
				hr = HRESULT_FROM_WIN32( GetLastError( ) );
				return { };
			}
		}


		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { };
		// part1 Create the pipeline state, which includes compiling and loading shaders.
		// part1 Describe and create the graphics pipeline state object (PSO).
		psoDesc.pRootSignature = rootSignature.Get( );
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
		psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		// old ->IASetPrimitiveTopology(); too there is ID3D12GraphicsCommandList::IASetPrimitiveTopology( )
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[ 0 ] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		return Dx::Ctx::CtxDx< DxVer::v12 >::z_make_shared( 
				pcD3dDevice12
				, swapChain3
				, arpcCommandAllocators
				, pcRtvHeap
				, rtvDescriptorSize
				, commandQueue
				, psoDesc
				, rootSignature
				, fence
				, fenceEvent
				, aruFenceValues
				, frameIndex
				, CD3DX12_VIEWPORT( 0.0f, 0.0f, fWidth, fHeight )
				, CD3DX12_RECT( 0, 0, lWidth, lHeight )
				, renderTargets
				, bTearingSupport
			 );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx::Impl
