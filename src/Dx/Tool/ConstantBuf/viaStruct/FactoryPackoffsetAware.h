// Dx/ConstantBuf/viaStruct/FactoryPackoffsetAware - factory for deduce type of template wrapper, with input struct type checker
// TODO: finishme
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::ConstantBuf::viaStruct {
class FactoryPackoffsetAware {
	const Dx::Ctxs m_stCtx;
public:
	FactoryPackoffsetAware(const Dx::Ctxs &stCtx) : m_stCtx( stCtx ) { }

#ifdef BOOST_PFR_ENABLED
	template<class TConstBuf>
	auto createAccessor() {
		// This will no longer be necessary, we will allocate ourselves: static_assert( (sizeof( TConstBuf ) % 16) == 0, "size not padded correctly" );
		std::vector<int> veSizeof;
		// Returns only values ​​and types. I don't know how to get names in order to calculate offsets.
		boost::pfr::for_each_field(TConstBuf{} , [&veSizeof]
			(const auto& field, auto qwe) {
				veSizeof.push_back( sizeof( field ) );
				// I think it will work if a type that is not a multiple of 4 is followed by a type that is not a multiple of 4.
				// it is also possible here it is worth checking the output of the last field for 16 bytes, although it is unlikely.
			} );
		return std::make_unique< Holder< TConstBuf > >( m_stCtx );
	}
#endif // BOOST_PFR_ENABLED

};
} // namespace prj_3d::HelloWinHlsl::Dx::ConstantBuf::viaStruct
