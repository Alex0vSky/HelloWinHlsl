// Dx/Tool/ConstantBuf/viaStruct/RootSignatureCreator.h - ...
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {
class RootSignatureCreator {
	using TInnerDxVer = DxVer::v12;
	const Ty::StDxCtx_ptr<TInnerDxVer> m_stDxCtx;
	static CD3DX12_ROOT_PARAMETER InitAsConstants_(UINT num32BitValues, UINT shaderRegister) {
		CD3DX12_ROOT_PARAMETER Out;
		CD3DX12_ROOT_PARAMETER::InitAsConstants( Out, num32BitValues, shaderRegister );
		return Out;
	}

 public:
	explicit RootSignatureCreator(Ty::StDxCtx_crefPtr<TInnerDxVer> stDxCtx) 
		: m_stDxCtx( stDxCtx ) 
	{}

#ifdef BOOST_PFR_ENABLED
	template<class TConstBuf>
	auto fromStruct() {
		std::vector< D3D12_ROOT_PARAMETER > vecParam;
		UINT shaderRegister = 0;
		boost::pfr::for_each_field( TConstBuf{ }, [&vecParam,&shaderRegister](const auto& field) {
			UINT num32BitValues = 0;
			CD3DX12_ROOT_PARAMETER param;
			using field_t = std::decay_t< decltype( field ) >;
			if constexpr ( false ) {}
			else if constexpr ( ( std::is_same_v< field_t, float > ) ) {
				param = InitAsConstants_( num32BitValues = 1, shaderRegister++ );
			}
			else if constexpr ( ( std::is_same_v< field_t, DirectX::XMFLOAT2 > ) ) {
				param = InitAsConstants_( num32BitValues = 2, shaderRegister++ );
			}
			else if constexpr ( ( std::is_same_v< field_t, DirectX::XMUINT2 > ) ) {
				param = InitAsConstants_( num32BitValues = 2, shaderRegister++ );
			}
			else 
				static_assert( Tpl::Trait::always_false_v< field_t >, "unsupport type" );
			vecParam.push_back( param );
		} );

		Sys::Hr hr;
		CPtr< ID3D12RootSignature > pcRootSignature;
		CD3DX12_ROOT_SIGNATURE_DESC stRootSignature;
		stRootSignature.Init( 
				(UINT)vecParam.size( ), vecParam.data( )
				, 0
				, nullptr
				, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
			);

		CPtr< ID3DBlob > signature;
		hr = ::D3D12SerializeRootSignature( 
				&stRootSignature
				, D3D_ROOT_SIGNATURE_VERSION_1
				, signature.ReleaseAndGetAddressOf( )
				, nullptr
			);
		hr = m_stDxCtx ->m_pcD3dDevice12 ->CreateRootSignature( 
				0
				, signature ->GetBufferPointer( )
				, signature ->GetBufferSize( )
				, IID_PPV_ARGS( pcRootSignature.ReleaseAndGetAddressOf( ) )
			);
		return pcRootSignature;
	}
#endif // BOOST_PFR_ENABLED

};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
