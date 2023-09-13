// GameLoop/InCurrentThread.h - the usual game loop, 
//	at the same time guarantees the correct order of calling destructors.
// @insp https://gist.github.com/lynxluna/4242170
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::GameLoop {

// primary template
template< class T>
class InCurrentThread : public detail_ ::BaseInCurrentThread_< T > {
	using base_t = detail_::BaseInCurrentThread_<T>;
	using detail_::BaseInCurrentThread_< T >::BaseInCurrentThread_;

 public:
	int run() override {
        base_t::m_puoAdjustDxAux = Dx::FactoryAdjustAux<T>::create( base_t::m_stCtx ->m_psstDxCtx );
		if ( !base_t::initialize( ) )
			return -1;
		return base_t::wndMessageAndClientCall( );
	}
};

// the full specialization Dx12
template<>
class InCurrentThread< DxVer::v12 > 
	: public Dx::Spec::D12::GameLoopPart
	, public detail_ ::BaseInCurrentThread_< DxVer::v12 > {
	using Dx::Spec::D12::GameLoopPart::GameLoopPart;
	using detail_::BaseInCurrentThread_< DxVer::v12 >::BaseInCurrentThread_;

 public:
	int run() override {
		GameLoopPart::beforeInitialize( );
		if ( !BaseInCurrentThread_::initialize( ) )
			return -1;
		GameLoopPart::afterAdjust( );
		return BaseInCurrentThread_::wndMessageAndClientCall( );
	}
};
} // namespace prj_3d::HelloWinHlsl::GameLoop
