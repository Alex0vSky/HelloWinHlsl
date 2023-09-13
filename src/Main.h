// Main.h - main code class
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl { 
class Main {
	using config_t = ClientApp::Configurator::Config;
	template<class T> 
	static
	int mainTpl(HINSTANCE hInst, config_t::cref_ptr_t pustCfg, uptr< Sys::HolderClientApp > pustHolder) {
		Ty::CurClientApp<T> *poClientApp = Sys::HolderClientApp::get< T >( *pustHolder );
		if ( !poClientApp )
			Sys::ErrorHandler::showMsgAndExit( "select client class" );

		auto pustWndProcHolder = Sys::Wnd::Creator::inCurrentThread( hInst, poClientApp );
		if ( !pustWndProcHolder )
			Sys::ErrorHandler::showMsgAndExit( "create window" );

		typename Ty::CurClientApp<T>::uptr_t puoClientApp( poClientApp );

		Sys::Wnd::StToken::cref_t crstWndToken = pustWndProcHolder ->getWndToken( );
		typename Dx::Ctxs<T>::uptrc_t pustCtx = 
			Dx::Ctx::CtxFactory<T>::create( crstWndToken );

		if ( !pustCtx )
			Sys::ErrorHandler::showMsgAndExit( "init new DirectX3D context" );
		auto &pustDxCtx = pustCtx ->m_psstDxCtx;

		// Frame limiter selection.
		FrameLimiter::ABase::uptr_t puoFrameLimiter;
		if ( false ) { 
		} else if ( pustCfg ->m_oLimitFps.isLimited( ) ) {
			puoFrameLimiter = std::make_unique< FrameLimiter::ViaSleep >( pustCfg ->m_oLimitFps.getLimit( ) );
		} else { // default
			puoFrameLimiter = std::make_unique< FrameLimiter::Unlimit >();
		}

		// Drawing FPS values.
		typename DrawAux::Fps::Ordinary<T>::uptr_t puoDrawAuxFps = nullptr;
		if ( false ) {
		} else if ( pustCfg ->m_bShowFps ) {
			puoDrawAuxFps = std::make_unique< DrawAux::Fps::Ordinary<T> >( pustDxCtx );
		} else { // default
		}

		// Drawing by the client on a prepared DirectX3D context and 
		//	the library outputting the frame-limited FPS value.
		std::unique_ptr< GameLoop::ABase_t<T> > puGameLoop;
		std::tuple tuArgs = std::make_tuple( 
				std::move( pustCtx )
				, std::move( puoClientApp )
				, std::move( puoDrawAuxFps )
				, std::move( puoFrameLimiter )
				, std::move( pustWndProcHolder )
			);
		if ( false ) {
		} else if ( config_t::EnumGameLoop::InSeparateThread == pustCfg ->m_enuGameLoop ) {
			puGameLoop = Tpl::Trait::make_unique_class_from_tuple
				< GameLoop::InSeparateThread<T> >
				( std::move( tuArgs ) );
		} else if ( config_t::EnumGameLoop::InCurrentThread == pustCfg ->m_enuGameLoop ) {
			puGameLoop = Tpl::Trait::make_unique_class_from_tuple
				< GameLoop::InCurrentThread<T> > 
				( std::move( tuArgs ) );
		} else { // default
			Sys::ErrorHandler::showMsgAndExit( "select game loop" );
		}
		return puGameLoop ->run( );
	}

 public:
	static 
	int run(HINSTANCE hInst) {
#ifdef _DEBUG
		HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif // _DEBUG

		// TODO(Alex0vSky): try catch and Sys::ErrorHandler::showMsgAndExit( "exception" );
		auto pustCfg = ClientTy::configure( );
		auto pustHolder = ClientTy::entryPoint( );

		if ( Sys::HolderClientApp::get< DxVer::v9 >( *pustHolder ) ) {
			return mainTpl< DxVer::v9 >( hInst, pustCfg, std::move( pustHolder ) );
		} else if ( Sys::HolderClientApp::get< DxVer::v10 >( *pustHolder ) ) {
			return mainTpl< DxVer::v10 >( hInst, pustCfg, std::move( pustHolder ) );
		} else if ( Sys::HolderClientApp::get< DxVer::v11 >( *pustHolder ) ) {
			return mainTpl< DxVer::v11 >( hInst, pustCfg, std::move( pustHolder ) );
		} else if ( Sys::HolderClientApp::get< DxVer::v12 >( *pustHolder ) ) {
			return mainTpl< DxVer::v12 >( hInst, pustCfg, std::move( pustHolder ) );
		} else {
			Sys::ErrorHandler::showMsgAndExit( "select DirectX version" );
		}
	}
};
} // namespace prj_3d::HelloWinHlsl
