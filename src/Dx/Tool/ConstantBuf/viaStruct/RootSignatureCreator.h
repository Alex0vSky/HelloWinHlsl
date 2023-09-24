// Dx/Tool/ConstantBuf/viaStruct/RootSignatureCreator.h - ...
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
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

	// @insp https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-constant-buffers-root-descriptor-tables // NOLINT
	template<class TConstBuf>
	auto fromStruct() {
		// create a descriptor range (descriptor table) and fill it out
		// this is a range of descriptors inside a descriptor heap

		// only one range right now
		D3D12_DESCRIPTOR_RANGE descriptorTableRanges[ 1 ];
		// this is a range of constant buffer views (descriptors)
		descriptorTableRanges[ 0 ].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		// we only have one constant buffer, so the range is only 1
		descriptorTableRanges[ 0 ].NumDescriptors = 1;
		// start index of the shader registers in the range
		descriptorTableRanges[ 0 ].BaseShaderRegister = 0;
		// space 0. can usually be zero
		descriptorTableRanges[ 0 ].RegisterSpace = 0;
		// this appends the range to the end of the root signature descriptor tables
		descriptorTableRanges[ 0 ].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    
		// create a descriptor table
		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
		// we only have one range
		descriptorTable.NumDescriptorRanges = _countof( descriptorTableRanges );
		// the pointer to the beginning of our ranges array
		descriptorTable.pDescriptorRanges = &descriptorTableRanges[ 0 ];

		// create a root parameter and fill it out

		// only one parameter right now
		D3D12_ROOT_PARAMETER rootParameters[ 1 ];
		// this is a descriptor table
		rootParameters[ 0 ].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		// this is our descriptor table for this root parameter
		rootParameters[ 0 ].DescriptorTable = descriptorTable;
		// our pixel shader will be the only shader accessing this parameter for now
		rootParameters[ 0 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		Sys::Hr hr;
		CPtr< ID3D12RootSignature > pcRootSignature;
		CD3DX12_ROOT_SIGNATURE_DESC stRootSignature;
		UINT numStaticSamplers = 0;
		stRootSignature.Init( 
				// we have 1 root parameter
				_countof( rootParameters )
				// a pointer to the beginning of our root parameters array
				, rootParameters
				, numStaticSamplers
				, nullptr
				// we can deny shader stages here for better performance
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
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
