// Dx/Tool/ConstantBuf/viaStruct/FactoryStrict.h - factory for deduce type of template wrapper, with input struct type checker, without using HLSL "packoffset"
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {
template<class T>
class FactoryStrict {
	const Ty::StDxCtx_ptr<T> m_stDxCtx;
public:
	explicit FactoryStrict(Ty::StDxCtx_crefPtr<T> stDxCtx) 
		: m_stDxCtx( stDxCtx ) 
	{}

#ifdef BOOST_PFR_ENABLED
	template<class TConstBuf>
	auto createAccessor(const TConstBuf &crst) {
		boost::pfr::for_each_field(TConstBuf{} , [](const auto& field, size_t ) {
				static_assert( ( sizeof( field ) >= 4 ), "size of field idx not padded correctly" );
			} );
		return std::make_unique< Accessor< T, TConstBuf > >( m_stDxCtx, crst );
	}
#endif // BOOST_PFR_ENABLED

};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
