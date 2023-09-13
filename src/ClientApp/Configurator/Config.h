// ClientApp/Configurator/Config.h - result for builder pattern
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace ClientApp::Configurator { 
struct Config {
	bool m_bShowFps;
	// Strong type
	class BaseFps { 
			unsigned int m_uLimitFps;
			
 public:
// TODO(Alex0vSky): the constructor is not yet clear, I want it to be clearly clear 
//	what I am creating with a frame limit (by setting a value) or without.
			explicit BaseFps(unsigned int uLimitFps) : m_uLimitFps( uLimitFps ) {}
			bool isLimited() const {
				return ( 0 != m_uLimitFps );
			}
			unsigned int getLimit() const {
				return m_uLimitFps;
			}
			BaseFps &operator =(unsigned int u) {
				m_uLimitFps = u;
				return *this;
			}
	} m_oLimitFps;
	enum class EnumGameLoop {
		InSeparateThread 
		, InCurrentThread
	} m_enuGameLoop;

	Config(
			bool bShowFps
			, BaseFps oLimitFps
			, EnumGameLoop enuGameLoop
		) 
		: m_bShowFps( bShowFps )
		, m_oLimitFps( oLimitFps )
		, m_enuGameLoop( enuGameLoop )
	 {}

	static auto getBuilder() {
		return std::make_unique< Builder >();
	}

	typedef uptr< const Config > uptrc_t;
	typedef const uptrc_t &cref_ptr_t;

 private: 
	Config(const Config &) = default; // only
	friend class Builder;
	friend class Predef;
};
} // namespace ClientApp::Configurator
