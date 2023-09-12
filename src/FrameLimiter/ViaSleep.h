// FrameLimiter/ViaSleep.h - fps limiter based on thread slip.
// @insp stackoverflow.com/questions/38730273/how-to-limit-fps-in-a-loop-with-c
// They say that turning on vertical sync gives a limit of 60 frames per second -- 
// www.gamedev.net/forums/topic/705049-60fps-framelock-done-properly-without-duplicated-or-skipped-frames/5418316/
#pragma once
namespace prj_3d::HelloWinHlsl::FrameLimiter {
class ViaSleep : public ABase { 
    using ChrDurMeasure_t = std::chrono::system_clock::duration;

    std::chrono::system_clock::time_point m_chrdNextFrame;
	ChrDurMeasure_t m_chrdFrameStep;
public:
    explicit ViaSleep(unsigned int uFreq_Hz)
        : m_chrdNextFrame( std::chrono::system_clock::now( ) )
    {
		auto chrdMilliInSec = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::seconds( 1 ) );
		auto chrdMilliInFreq = std::chrono::milliseconds( chrdMilliInSec / std::chrono::milliseconds( uFreq_Hz ) );
		m_chrdFrameStep = std::chrono::duration_cast< ChrDurMeasure_t >( chrdMilliInFreq );
    }
	// TODO: remakeme, wrong when hung in start
    void limit() override {
        // TODO: calc time spend before current method call, and then reduce next frame step
        std::this_thread::sleep_until( m_chrdNextFrame );
        m_chrdNextFrame += m_chrdFrameStep;
    }
};
} // namespace prj_3d::HelloWinHlsl::FrameLimiter
