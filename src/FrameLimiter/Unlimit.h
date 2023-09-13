// FrameLimiter/Unlimit.h - no limit on the number of frames per second.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::FrameLimiter {
class Unlimit : public ABase { 
 public:
    void limit() override {
        __nop( );
    }
};
} // namespace prj_3d::HelloWinHlsl::FrameLimiter
