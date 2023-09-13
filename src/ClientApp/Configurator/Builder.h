// ClientApp/Configurator/Builder.h - builder for builder pattern
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace ClientApp::Configurator { 
class Builder {
	Config m_oConfig;

 public:
	Builder() 
		: m_oConfig( *( Predef::getDefault( ) ) ) 
	 {}
    auto build() const {
        return Config::uptrc_t( new Config( m_oConfig ) );
    }
	operator Config::uptrc_t() {
        return Config::uptrc_t( new Config( m_oConfig ) );
	}

	Builder *setShowFps(bool b) {
		m_oConfig.m_bShowFps = b;
		return this;
	}
	Builder *setLimitFps(unsigned int u) {
		m_oConfig.m_oLimitFps = u;
		return this;
	}
	Builder *setGameLoop(Config::EnumGameLoop enu) {
		m_oConfig.m_enuGameLoop = enu;
		return this;
	}
};
} // namespace ClientApp::Configurator
