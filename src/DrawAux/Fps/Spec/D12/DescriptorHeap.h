// DrawAux/Fps/Spec/D12/DescriptorHeap.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {
class DescriptorHeap {
	_Use_decl_annotations_
	void DescriptorHeap::Create(
		ID3D12Device* pDevice,
		const D3D12_DESCRIPTOR_HEAP_DESC* pDesc)
	{
		assert(pDesc != nullptr);

		m_desc = *pDesc;
		m_increment = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);

		if (pDesc->NumDescriptors == 0) {
			m_pHeap.Reset();
			m_hCPU.ptr = 0;
			m_hGPU.ptr = 0;
		} else {
			Sys::Hr hr;
			hr = pDevice->CreateDescriptorHeap(
				pDesc,
				IID_PPV_ARGS( m_pHeap.ReleaseAndGetAddressOf( ) ) );

			SetDebugObjectName(m_pHeap.Get(), L"DescriptorHeap");

#if defined(_MSC_VER) || !defined(_WIN32)
			m_hCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart();
			if (pDesc->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
				m_hGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart();
			}
#else
			std::ignore = m_pHeap->GetCPUDescriptorHandleForHeapStart(&m_hCPU);
			if (pDesc->Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
				std::ignore = m_pHeap->GetGPUDescriptorHandleForHeapStart(&m_hGPU);
			}
#endif
		}
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>    m_pHeap;
	D3D12_DESCRIPTOR_HEAP_DESC                      m_desc;
	D3D12_CPU_DESCRIPTOR_HANDLE                     m_hCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE                     m_hGPU;
	uint32_t                                        m_increment;

 public:
	DescriptorHeap::DescriptorHeap(
		ID3D12Device* device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags,
		size_t count) noexcept(false) 
		: m_desc{ }
		, m_hCPU{ }
		, m_hGPU{ }
		, m_increment( 0 ) {
		if ( count > UINT32_MAX )
			throw std::invalid_argument("Too many descriptors");
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = flags;
		desc.NumDescriptors = static_cast<UINT>(count);
		desc.Type = type;
		Create( device, &desc );
	}

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(_In_ size_t index) const {
        assert(m_pHeap != nullptr);
        if ( index >= m_desc.NumDescriptors ) {
            throw std::out_of_range("D3DX12_GPU_DESCRIPTOR_HANDLE");
        }
        assert(m_desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

        D3D12_GPU_DESCRIPTOR_HANDLE handle;
        handle.ptr = m_hGPU.ptr + UINT64(index) * UINT64(m_increment);
        return handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(_In_ size_t index) const {
        assert(m_pHeap != nullptr);
        if (index >= m_desc.NumDescriptors) {
            throw std::out_of_range("D3DX12_CPU_DESCRIPTOR_HANDLE");
        }

        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        handle.ptr = static_cast<SIZE_T>(m_hCPU.ptr + UINT64(index) * UINT64(m_increment));
        return handle;
    }

	ID3D12DescriptorHeap* Heap() const noexcept { return m_pHeap.Get(); }

};
} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
