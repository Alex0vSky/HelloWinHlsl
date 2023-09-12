// _pch\sysw.h
#pragma once

#include "framework.h"

#include <sstream>
#include <memory>
// WRL=WindowsRuntime_c++_template_Library
#include <wrl/client.h>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <thread>
#include <chrono>
#include <fstream>
#include <comdef.h>

#include <array>
#include <mutex>
#include <filesystem>
#include <map>
#include <deque>
#include <variant>
#include <queue>

#include <intrin.h>

// Boost pfr
#ifndef __INTELLISENSE__
#	ifndef A0S_cppcheck__
#		include "boost\pfr.hpp"
#	endif // A0S_cppcheck__
#endif // __INTELLISENSE__
//	for IntelliSence methods list
#ifndef BOOST_PFR_ENABLED
#	define BOOST_PFR_ENABLED
#endif // BOOST_PFR_ENABLED