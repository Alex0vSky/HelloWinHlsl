// FrameLimiter/Unlimit.h - no limit on the number of frames per second.
#pragma once
namespace prj_3d::HelloWinHlsl::FrameLimiter {
class Unlimit : public ABase { 
public:
    void limit() override {
        __nop( );
    }
};
} // namespace prj_3d::HelloWinHlsl
