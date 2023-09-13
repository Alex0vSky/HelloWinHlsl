// Dx/DynamicData.h - dynamic data from Dx to client render
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx { 

template<class T> struct ClientDynamicData;

namespace detail_ {
template<class T> 
struct CommonClientDynamicData {
	typedef uptr< ClientDynamicData< T > > uptrc_t;
	typedef const uptrc_t &cref_ptr_t;
};
} // namespace detail_

template<class T> 
struct ClientDynamicData : public detail_::CommonClientDynamicData<T> {
};
template<> 
struct ClientDynamicData<DxVer::v12> : public detail_::CommonClientDynamicData<DxVer::v12> {
	const CPtr< ID3D12GraphicsCommandList > m_pcCommandList;
	const D3D12_CPU_DESCRIPTOR_HANDLE m_stRtvHandle;
	const bool m_isWindowed;
	const HWND m_hWnd;
};

// primary template, "typename = void" for partial specialization
template<class T, typename = void> struct DynamicData;

namespace detail_ {
template<class T> 
struct CommonDynamicData {
	typedef sptr< DynamicData< T > > sptr_t;
	typedef const sptr_t &cref_ptr_t;
	// make shared_ptr, work with single argument too
	template<typename... Args> static auto z_make_shared(Args&&... args) {
		return std::make_shared< DynamicData< T > > ( 
				DynamicData< T >{ 
					// parent struct aggregate initialization
					{ }
					// real arguments
					, std::forward<Args>(args)... 
				}
			);
	}
};
} // namespace detail_

// dummy/empty, the partial specialization other Dx
template<class T> struct DynamicData<T> : public detail_::CommonDynamicData<T> {
	auto forClient() const {
		return std::make_unique< ClientDynamicData<T> >( );
	}
};

// the full specialization Dx12
template<> struct DynamicData<DxVer::v12> : public detail_::CommonDynamicData<DxVer::v12> { 
	typedef sptr< const DynamicData< DxVer::v12 > > sptrc_t;
	typedef const sptrc_t &cref_ptr_t;

	CPtr< ID3D12GraphicsCommandList > m_pcCommandList;
	D3D12_CPU_DESCRIPTOR_HANDLE m_stRtvHandle;
	bool m_isWindowed;
	HWND m_hWnd;
	auto setCommandList(const CPtr< ID3D12GraphicsCommandList > pcCommandList) {
		m_pcCommandList = pcCommandList;
	}

	void setRtvHandle(const D3D12_CPU_DESCRIPTOR_HANDLE &rtvHandle) {
		m_stRtvHandle = rtvHandle;
	}

	void setWindowed(bool isWindowed) {
		m_isWindowed = isWindowed;
	}

	void setWindowHandle(HWND hWnd) {
		m_hWnd = hWnd;
	}

	auto forClient() const {
		return std::make_unique< Tpl::Trait::aggregate_adapter_empty_parent< ClientDynamicData< DxVer::v12 > > >(
				m_pcCommandList
				, m_stRtvHandle
				, m_isWindowed
				, m_hWnd
			);
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx
