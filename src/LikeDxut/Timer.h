// LikeDxut/Timer.h - to measure the time that can be passed to the shader. 
//	Initialization logic is the same as "CDXUTTimer" in the DirectX examples.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::LikeDxut { 
class Timer { 
	LONGLONG m_llQPFTicksPerSec, m_llBaseTime;
    LONGLONG getPerfCounter_() const {
		LARGE_INTEGER qwTime = {{ 0 }}; // clang
		return ::QueryPerformanceCounter( &qwTime ), qwTime.QuadPart;
    }
 public:
    Timer() {
		//CDXUTTimer::CDXUTTimer()
		LARGE_INTEGER qwTicksPerSec = {{ 0 }}; // clang
		::QueryPerformanceFrequency( &qwTicksPerSec );
		m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
		//void CDXUTTimer::Reset(), void CDXUTTimer::Stop(), void CDXUTTimer::Start()
		m_llBaseTime = getPerfCounter_( );
    }
    float get() const {
		//void CDXUTTimer::GetTimeValues( double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime )
		return static_cast<float>( 
			( getPerfCounter_( ) - m_llBaseTime ) / ( double )m_llQPFTicksPerSec );
    }
};
} // namespace prj_3d::HelloWinHlsl::LikeDxut
