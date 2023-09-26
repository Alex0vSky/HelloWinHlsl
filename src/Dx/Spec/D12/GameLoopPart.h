// Dx/Spec/D12/GameLoopPart.h - common behavior in run for DirectX 12 game loop
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 {
class GameLoopPart : virtual public GameLoop::detail_::ABaseWndProcHolderAware<DxVer::v12> {
	using ABaseWndProcHolderAware<DxVer::v12>::ABaseWndProcHolderAware;

 protected:
	uptr< DrawAux::Fps::Spec::D12::GraphicsMemory > m_graphicsMemory;
	sptr< Dx::Spec::D12::Waiter > m_psoWaiter;
	sptr< Dx::Spec::D12::Resizer > m_psoResizer;
	CPtr< ID3D12RootSignature > m_pcRootSignature;
	uptr< Dx::Spec::D12::SetterRootSignature > m_puSetterRootSignature;
	CPtr< ID3D12PipelineState > m_pcPipelineState;
	CPtr< ID3D12GraphicsCommandList > m_pc_CommandList;

	uptr< ABaseWndProcHolderAware::mutex_t > m_puMutex;

	void beforeInitialize() {
		m_pcRootSignature = m_stCtx ->m_psstDxCtx ->m_initialRootSignature;
		m_psoWaiter = std::make_shared< Dx::Spec::D12::Waiter >( 
				m_stCtx ->m_psstDxCtx ->detail_.m_pcFence
				, m_stCtx ->m_psstDxCtx ->detail_.m_hFenceEvent
				, m_stCtx ->m_psstDxCtx ->m_pcCommandQueue
				, m_stCtx ->m_psstDxCtx ->m_pcDxgiSwapChain
				, m_stCtx ->m_psstDxCtx ->detail_.m_aruFenceValues
				, m_stCtx ->m_psstDxCtx ->detail_.m_uFrameIndex
			);
		m_psoResizer = std::make_shared< Dx::Spec::D12::Resizer >( 
				m_stCtx ->m_psstDxCtx ->detail_.m_viewport
				, m_stCtx ->m_psstDxCtx ->detail_.m_scissorRect
				, m_stCtx ->m_psstDxCtx ->detail_.m_renderTargets
			);
		for ( const auto &var : m_stCtx ->m_psstDxCtx ->detail_.m_renderTargets )
			var.~CPtr( );

		m_puoWnd ->setDxCtx( m_stCtx ->m_psstDxCtx, m_puMutex.get( ), m_psoWaiter.get( ), m_psoResizer.get( ) );

		m_puSetterRootSignature = std::make_unique< Dx::Spec::D12::SetterRootSignature >( 
			std::addressof( m_pcRootSignature ) );
        m_puoAdjustDxAux = Dx::FactoryAdjustAux<DxVer::v12>::create( 
				m_stCtx ->m_psstDxCtx
				, m_psoWaiter.get( )
				, *m_puSetterRootSignature
			);
	}

	virtual ~GameLoopPart() {
		m_psoWaiter ->waitForGpu( );
		if ( !m_stCtx ->m_psstDxCtx ->detail_.m_bTearingSupport ) {
			// Fullscreen state should always be false before exiting the app.
			Sys::Hr hr = m_stCtx ->m_psstDxCtx ->m_pcDxgiSwapChain ->SetFullscreenState( FALSE, nullptr );
		}
	}

	void afterAdjust() {
		m_pcPipelineState = m_puoAdjustDxAux ->getPipelineState( );
		m_pc_CommandList = m_puoAdjustDxAux ->getCommandList( );
		m_psoDynamicData ->setCommandList( m_pc_CommandList );
		if ( m_puoDrawAuxFps ) {
			m_puoDrawAuxFps ->setDx12Viewport( m_psoResizer ->getViewport( ) );
			m_puoDrawAuxFps ->setDx12CommandList( m_pc_CommandList );
		}
		m_graphicsMemory = std::make_unique< DrawAux::Fps::Spec::D12::GraphicsMemory >( 
			m_stCtx ->m_psstDxCtx ->m_pcD3dDevice12.Get( ) );

		this ->setCallbackAfterPresent(
				[this]() {
					m_psoWaiter ->MoveToNextFrame( );

					// Not mandatory for fps draw
					m_graphicsMemory ->Commit( m_stCtx ->m_psstDxCtx ->m_pcCommandQueue.Get( ) );
				}
			);
		this ->setCallbackBeforeRender(
				[this]() {
					Sys::Hr hr;
					// Render the scene. //part void D3D12HelloTriangle::OnRender() ...
					//beg void D3D12HelloTriangle::PopulateCommandList() {
					// Command list allocators can only be reset when the associated 
					// command lists have finished execution on the GPU; apps should use 
					// fences to determine GPU execution progress.
					UINT frameIndex = m_psoWaiter ->get_frameIndex( );
					hr = m_stCtx ->m_psstDxCtx ->m_arpcCommandAllocators[ frameIndex ] ->Reset( );

					// However, when ExecuteCommandList() is called on a particular command 
					// list, that command list can then be reset at any time and must be before 
					// re-recording.
					// FullGPU
					hr = m_pc_CommandList ->Reset(
							m_stCtx ->m_psstDxCtx ->m_arpcCommandAllocators[ frameIndex ].Get( )
							, m_pcPipelineState.Get( )
						);

					// Set necessary state.
					m_pc_CommandList ->SetGraphicsRootSignature( m_pcRootSignature.Get( ) );

					// set constant buffer descriptor heap
					ID3D12DescriptorHeap* descriptorHeaps[] = { m_stCtx ->m_psstDxCtx ->m_arpcDescriptorHeap[ frameIndex ].Get( ) };
					m_pc_CommandList ->SetDescriptorHeaps( _countof( descriptorHeaps ), descriptorHeaps );
					// set the root descriptor table 0 to the constant buffer descriptor heap
					UINT RootParameterIndex = 0;
					m_pc_CommandList ->SetGraphicsRootDescriptorTable(
							RootParameterIndex
							, m_stCtx ->m_psstDxCtx ->m_arpcDescriptorHeap[ frameIndex ] ->GetGPUDescriptorHandleForHeapStart( )
						);

					m_pc_CommandList ->RSSetViewports( 1, &m_psoResizer ->getViewport( ) );
					m_pc_CommandList ->RSSetScissorRects( 1, &m_psoResizer ->getScissorRect( ) );
					// Indicate that the back buffer will be used as a render target.
					CD3DX12_RESOURCE_BARRIER rb1 = CD3DX12_RESOURCE_BARRIER::Transition(
							m_psoResizer ->getRenderTargets( )[ m_psoWaiter ->get_frameIndex( ) ].Get( )
							, D3D12_RESOURCE_STATE_PRESENT
							, D3D12_RESOURCE_STATE_RENDER_TARGET
						);
					m_pc_CommandList ->ResourceBarrier( 1, &rb1 );

					CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( 
							m_stCtx ->m_psstDxCtx ->m_rtvHeap ->GetCPUDescriptorHandleForHeapStart( )
							, m_psoWaiter ->get_frameIndex( )
							, m_stCtx ->m_psstDxCtx ->m_rtvDescriptorSize 
						);
					m_pc_CommandList ->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );
					m_psoDynamicData ->setRtvHandle( rtvHandle );
					m_psoDynamicData ->setWindowed( m_puoWnd ->isWindowed( ) );
					m_psoDynamicData ->setWindowHandle( m_puoWnd ->getWndToken( ).m_hWnd );
					m_psoDynamicData ->setFrameIndex( m_psoWaiter ->get_frameIndex( ) );
				}
			);
		this ->setCallbackAfterRender(
				[this]() {
					Sys::Hr hr;
					// Indicate that the back buffer will now be used to present.
					CD3DX12_RESOURCE_BARRIER rb2 = CD3DX12_RESOURCE_BARRIER::Transition(
							m_psoResizer ->getRenderTargets( )[ m_psoWaiter ->get_frameIndex( ) ].Get( )
							, D3D12_RESOURCE_STATE_RENDER_TARGET
							, D3D12_RESOURCE_STATE_PRESENT
						);
					m_pc_CommandList ->ResourceBarrier( 1, &rb2 );
					hr = m_pc_CommandList ->Close( );
					//} end void D3D12HelloTriangle::PopulateCommandList()

					// Execute the command list.
					ID3D12CommandList* ppCommandLists[] = { m_pc_CommandList.Get( ) };
					m_stCtx ->m_psstDxCtx ->m_pcCommandQueue ->ExecuteCommandLists( 
						_countof( ppCommandLists ), ppCommandLists );
				}
			);		
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12
