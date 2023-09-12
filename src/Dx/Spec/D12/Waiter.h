// Dx/Spec/D12/Waiter.h - wait frame ready
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 {
class Waiter {
	static const UINT FrameCount = 2;
	const CPtr< ID3D12Fence > m_pcFence;
	const HANDLE m_hFenceEvent;
	const CPtr< ID3D12CommandQueue > m_pcCommandQueue;
	const CPtr< IDXGISwapChain3 > m_pcDxgiSwapChain;
	std::array< UINT64, FrameCount > m_aruFenceValues;
	UINT m_uFrameIndex;

 public:
	Waiter( 
		CPtr< ID3D12Fence > pcFence
		, HANDLE hFenceEvent
		, CPtr< ID3D12CommandQueue > pcCommandQueue
		, CPtr< IDXGISwapChain3 > pcDxgiSwapChain
		, const std::array< UINT64, FrameCount > &aruFenceValues
		, UINT frameIndex
	)
		: m_pcFence( pcFence )
		, m_hFenceEvent( hFenceEvent )
		, m_pcCommandQueue( pcCommandQueue )
		, m_pcDxgiSwapChain( pcDxgiSwapChain )
		, m_aruFenceValues{ aruFenceValues }
		, m_uFrameIndex( frameIndex )
	{}
	void set_frameIndex(UINT frameIndex) {
		m_uFrameIndex = frameIndex;
	}
	UINT get_frameIndex() const {
		return m_uFrameIndex;
	}

	// @insp HelloFrameBuffering\D3D12HelloFrameBuffering.cpp
	// Prepare to render the next frame.
	void MoveToNextFrame() {
		Sys::Hr hr;
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_aruFenceValues[ m_uFrameIndex ];
		hr = m_pcCommandQueue ->Signal( m_pcFence.Get( ), currentFenceValue );

		// Update the frame index.
		m_uFrameIndex = m_pcDxgiSwapChain ->GetCurrentBackBufferIndex( );

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if ( m_pcFence ->GetCompletedValue( ) < m_aruFenceValues[ m_uFrameIndex ] ) {
			hr =  m_pcFence ->SetEventOnCompletion( m_aruFenceValues[ m_uFrameIndex ], m_hFenceEvent );
			const BOOL bNotAlertable = FALSE;
			::WaitForSingleObjectEx( m_hFenceEvent, INFINITE, bNotAlertable );
		}

		// Set the fence value for the next frame.
		m_aruFenceValues[ m_uFrameIndex ] = currentFenceValue + 1;
	}
	void MoveToNextFrame_signal() {
		Sys::Hr hr;
		// Schedule a Signal command in the queue.
		const UINT64 currentFenceValue = m_aruFenceValues[ m_uFrameIndex ];
		hr = m_pcCommandQueue ->Signal( m_pcFence.Get( ), currentFenceValue );
	}
	void MoveToNextFrame_wait() {
		Sys::Hr hr;
		// Update the frame index.
		m_uFrameIndex = m_pcDxgiSwapChain ->GetCurrentBackBufferIndex( );

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if ( m_pcFence ->GetCompletedValue( ) < m_aruFenceValues[ m_uFrameIndex ] ) {
			hr =  m_pcFence ->SetEventOnCompletion( m_aruFenceValues[ m_uFrameIndex ], m_hFenceEvent );
			const BOOL bNotAlertable = FALSE;
			::WaitForSingleObjectEx( m_hFenceEvent, INFINITE, bNotAlertable );
		}

		// Set the fence value for the next frame.
		++m_aruFenceValues[ m_uFrameIndex ];
	}
	// Wait for pending GPU work to complete.
	void waitForGpu() {
		Sys::Hr hr;
		// After init increment
		m_aruFenceValues[ m_uFrameIndex ]++;
		// Schedule a Signal command in the queue.
		hr = m_pcCommandQueue ->Signal( m_pcFence.Get( ), m_aruFenceValues[ m_uFrameIndex ] );

	    // Wait until the fence has been processed.
		hr = m_pcFence ->SetEventOnCompletion( m_aruFenceValues[ m_uFrameIndex ], m_hFenceEvent );
		const BOOL bNotAlertable = FALSE;
		::WaitForSingleObjectEx( m_hFenceEvent, INFINITE, bNotAlertable );

		// Increment the fence value for the current frame.
		m_aruFenceValues[ m_uFrameIndex ]++;
	}
	// For resizer
	void afterResetRenderTarget(UINT n) {
		m_aruFenceValues[ n ] = m_aruFenceValues[ m_uFrameIndex ];
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12
