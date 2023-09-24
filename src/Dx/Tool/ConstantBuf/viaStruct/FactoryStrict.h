// Dx/Tool/ConstantBuf/viaStruct/FactoryStrict.h - factory
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {
template<class T>
class FactoryStrict {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;

 public:
	explicit FactoryStrict(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx ) 
	 {}

	template<class TConstBuf>
	auto createAccessor(const TConstBuf &crst) {
		return std::make_unique< Accessor< T, TConstBuf > >( m_stDxCtx, crst );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
