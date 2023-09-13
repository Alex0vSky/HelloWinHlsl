// Dx/Spec/D12/SetterRootSignature.h - avoid direct use of not const pointers(*) extensively in code.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Dx::Spec::D12 {
class SetterRootSignature {
	typedef CPtr< ID3D12RootSignature > type_t;
	type_t *m_ppcRootSignature;
 public:
	typedef const SetterRootSignature &cref_t;
	explicit SetterRootSignature(type_t *ppcRootSignature) 
		: m_ppcRootSignature( ppcRootSignature )
    {}
	void set(ID3D12RootSignature *ifcRootSignature) {
		(*m_ppcRootSignature) = type_t( ifcRootSignature );
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Spec::D12
