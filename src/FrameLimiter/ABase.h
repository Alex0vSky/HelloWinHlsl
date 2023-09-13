// FrameLimiter/ABase.h - the base class for the frame rate limiter.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::FrameLimiter {
struct ABase { 
    typedef uptr< ABase > uptr_t;
    virtual void limit() = 0;
    virtual ~ABase()
	 {}
};
} // namespace prj_3d::HelloWinHlsl::FrameLimiter
