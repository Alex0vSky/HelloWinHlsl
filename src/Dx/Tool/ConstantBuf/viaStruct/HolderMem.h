// Dx/Tool/ConstantBuf/viaStruct/HolderMem.h - self release aligned memory
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {
template<class TConstBuf>
class HolderMem {
	void* m_lpAlignedMem;
public:
	explicit HolderMem(const TConstBuf &crst) { 
		m_lpAlignedMem = Sys::HlSh_alloc( sizeof( crst ) );
		if ( !m_lpAlignedMem ) 
			throw std::bad_alloc( );
		memcpy_s( m_lpAlignedMem, sizeof( crst ), &crst, sizeof( crst ) );
	}
	~HolderMem() { 
		Sys::HlSh_free( m_lpAlignedMem );
	}

	void *getMem() const {
		return m_lpAlignedMem;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
