// GameLoop/InSeparateThread.h - render does not hang when the user moves the window (WM_ENTERSIZEMOVE). 
//	"PeekMessage" and "GetMessage" take messages only from the thread where the window was created, 
//	and "AttachThreadInput" is poorly compatible.
//@insp https://stackoverflow.com/questions/617248/can-the-hwnd-from-createwindow-createdialog-be-getmessaged-from-another-thread
#pragma once
namespace prj_3d::HelloWinHlsl::GameLoop {

// primary template
template< class T>
class InSeparateThread : public detail_ ::BaseInSeparateThread_< T > {
	using base_t = detail_::BaseInSeparateThread_<T>;
	using detail_::BaseInSeparateThread_< T >::BaseInSeparateThread_;

 public:
	virtual ~InSeparateThread() 
	{}
	int run() override {
        base_t::m_puoAdjustDxAux = Dx::FactoryAdjustAux<T>::create( base_t::m_stCtx ->m_psstDxCtx );
		if ( !base_t::initialize( ) )
			return -1;
		return base_t::wndMessageAndClientCall( );
	}
};

// the full specialization Dx12
template<>
class InSeparateThread< DxVer::v12 > 
	: public Dx::Spec::D12::GameLoopPart
	, public detail_ ::BaseInSeparateThread_< DxVer::v12 > 
{
	using Dx::Spec::D12::GameLoopPart::GameLoopPart;
	using detail_::BaseInSeparateThread_< DxVer::v12 >::BaseInSeparateThread_;

 public:
	virtual ~InSeparateThread() 
	{}
	int run() override {
		// syncronized
		GameLoopPart::m_puMutex = std::make_unique< BaseInSeparateThread_::mutex_t >( );
		BaseInSeparateThread_::setSynchronization( GameLoopPart::m_puMutex.get( ) );
		GameLoopPart::beforeInitialize( );
		if ( !BaseInSeparateThread_::initialize( ) )
			return -1;
		GameLoopPart::afterAdjust( );
		return BaseInSeparateThread_::wndMessageAndClientCall( );
    }
};
} // namespace prj_3d::HelloWinHlsl::GameLoop
