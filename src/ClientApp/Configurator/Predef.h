// ClientApp/Configurator/Predef.h - predefined result of the configurator.
#pragma once
namespace ClientApp::Configurator { 
class Predef { 
public:
	// Default config.
	static auto getDefault() {
		// Abuse singleton pattern for headerOnly lib
		static const Config s_struConfig(
				// m_bShowFps
				true
				// m_oLimitFps
				, Config::BaseFps( 0 )
				// m_enuGameLoop
				, Config::EnumGameLoop::InSeparateThread
			);
        return Config::uptrc_t( new Config( s_struConfig ) ); // without friend make_unique
	}
};
} // namespace ClientApp::Configurator _
