// Sys/AlignCode.h - align macro and alloc/free code
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Sys { 
static void* HlSh_alloc(size_t i) {
    return _aligned_malloc( i, c_uDxAlign );
}
static void HlSh_free(void* p) {
    _aligned_free( p );
}
} // namespace prj_3d::HelloWinHlsl::Sys
