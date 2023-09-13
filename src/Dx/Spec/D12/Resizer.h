// Dx/Spec/D12/Resizer.h - resize window, now its only storage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 {
class Resizer {
	D3D12_VIEWPORT m_stViewport;
	D3D12_RECT m_stScissorRect;
	std::vector< CPtr< ID3D12Resource > > m_veRenderTargets;
	
 public:
	template<size_t FrameCount>
	Resizer( 
		const D3D12_VIEWPORT &viewport
		, const D3D12_RECT &scissorRect
		, const std::array< CPtr< ID3D12Resource >, FrameCount > &renderTargets
	)
		: m_stViewport( viewport )
		, m_stScissorRect( scissorRect )
		, m_veRenderTargets( renderTargets.begin( ), renderTargets.end( ) )
	 {}
	const D3D12_VIEWPORT &getViewport() const {
		return m_stViewport;
	}
	void setViewport(const D3D12_VIEWPORT &viewport) {
		m_stViewport = viewport;
	}
	const D3D12_RECT &getScissorRect() const {
		return m_stScissorRect;
	}
	void setScissorRect(const D3D12_RECT &viewport) {
		m_stScissorRect = viewport;
	}
	const auto &getRenderTargets() const { 
		return m_veRenderTargets;
	}	
	auto &getRenderTarget(size_t Position) { 
		return m_veRenderTargets[ Position ];
	}	
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12
