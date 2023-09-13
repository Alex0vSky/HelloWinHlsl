// Dx/Ctx/CtxDx.h - structure of DirectX3D context
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Ctx {

template<class T> struct CtxDx; // primary template

namespace detail_ {

template<class T> 
struct CommonDxCtx {
	// const all of them
	typedef sptr< const CtxDx< T > > sptrc_t;
	typedef const sptrc_t &cref_ptr_t;
	// make shared_ptr, work with single argument too
	// @insp stackoverflow.com/questions/35298989/using-c-aggregate-initialization-in-stdmake-shared
	template<typename... Args> static auto z_make_shared(Args&&... args) {
		return std::make_shared< CtxDx< T > > ( 
				CtxDx< T >{ 
					// parent struct aggregate initialization
					{ }
					// real arguments
					, std::forward<Args>(args)... 
				}
			);
	}
};
} // namespace detail_

template<> struct CtxDx<DxVer::v9> : public detail_::CommonDxCtx<DxVer::v9> { 
    const CPtr< IDirect3DDevice9 > m_pcD3dDevice;
};
template<> struct CtxDx<DxVer::v10> : public detail_::CommonDxCtx<DxVer::v10> { 
	const CPtr< IDXGISwapChain > m_pcDxgiSwapChain;
    const CPtr< ID3D10RenderTargetView > m_pcD3dRenderTargetView;
    const CPtr< ID3D10Device > m_pcD3dDevice;
};
template<> struct CtxDx<DxVer::v11> : public detail_::CommonDxCtx<DxVer::v11> { 
	const CPtr< IDXGISwapChain > m_pcDxgiSwapChain;
    const CPtr< ID3D11RenderTargetView > m_pcD3dRenderTargetView;
    const CPtr< ID3D11Device > m_pcD3dDevice;
    const CPtr< ID3D11DeviceContext > m_pcDeviceContext;
};
template<> struct CtxDx<DxVer::v12> : public detail_::CommonDxCtx<DxVer::v12> {
    static const UINT FrameCount = 2;
    const CPtr< ID3D12Device > m_pcD3dDevice12;
	const CPtr< IDXGISwapChain3 > m_pcDxgiSwapChain;
	const std::array< CPtr< ID3D12CommandAllocator >, FrameCount > m_arpcCommandAllocators;
	const CPtr< ID3D12DescriptorHeap > m_rtvHeap;
	const UINT m_rtvDescriptorSize;
	const CPtr< ID3D12CommandQueue > m_pcCommandQueue;
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
	const CPtr< ID3D12RootSignature > m_initialRootSignature;
	struct detail_ {
		const CPtr< ID3D12Fence > m_pcFence;
		const HANDLE m_hFenceEvent;
		const std::array< UINT64, FrameCount > m_aruFenceValues;
		const UINT m_uFrameIndex;
		const CD3DX12_VIEWPORT m_viewport;
		const CD3DX12_RECT m_scissorRect;
		// TODO(Alex0vSky): how to safe release under const? Need for SwapChain::ResizeBuffers()
		const std::array< CPtr< ID3D12Resource >, FrameCount > m_renderTargets;
		bool m_bTearingSupport;
	} detail_;
};
} // namespace prj_3d::HelloWinHlsl::Dx::Ctx
