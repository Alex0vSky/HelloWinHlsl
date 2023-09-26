// DrawAux/Fps/Spec/D12/mix.h - from DirectXTK
#pragma once // NOLINT copyright
// TODO(alex): wtf, error LNK2001 unresolved external symbol IID_ID3D12Device
const GUID IID_ID3D12Device = { 0x189819f1, 0x1db6, 0x4b57, { 0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7 } };
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {

using XMVECTOR = DirectX::XMVECTOR;
using FXMVECTOR = DirectX::FXMVECTOR;
using XMVECTOR = DirectX::XMVECTOR;
using FXMVECTOR = DirectX::FXMVECTOR;
using GXMVECTOR = DirectX::GXMVECTOR;
using XMUINT2 = DirectX::XMUINT2;
using XMVECTORF32 = DirectX::XMVECTORF32;
using XMMATRIX = DirectX::XMMATRIX;
using FXMMATRIX = DirectX::FXMMATRIX;
using XMFLOAT2 = DirectX::XMFLOAT2;
using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;
using XMFLOAT4A = DirectX::XMFLOAT4A;
using XMUINT2 = DirectX::XMUINT2;
using GXMVECTOR = DirectX::GXMVECTOR;
using CXMVECTOR = DirectX::CXMVECTOR;

template<UINT TNameLength>
inline void SetDebugObjectName(_In_ ID3D12DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength]) noexcept
{
    resource->SetName(name);
}

// Helper for output debug tracing
inline void DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...) noexcept
{
//#ifdef _DEBUG
    va_list args;
    va_start(args, format);

    char buff[1024] = {};
    vsprintf_s(buff, format, args);
    OutputDebugStringA(buff);
    va_end(args);
//#else
//    UNREFERENCED_PARAMETER(format);
//#endif
}

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
    constexpr D3D12_RESOURCE_STATES c_initialCopyTargetState = D3D12_RESOURCE_STATE_COPY_DEST;
#else
    constexpr D3D12_RESOURCE_STATES c_initialCopyTargetState = D3D12_RESOURCE_STATE_COMMON;
#endif

// See https://walbourn.github.io/modern-c++-bitmask-types/
#ifndef ENUM_FLAGS_CONSTEXPR
#if defined(NTDDI_WIN10_RS1) && !defined(__MINGW32__)
#define ENUM_FLAGS_CONSTEXPR constexpr
#else
#define ENUM_FLAGS_CONSTEXPR const
#endif
#endif

enum SpriteEffects : uint32_t
{
    SpriteEffects_None = 0,
    SpriteEffects_FlipHorizontally = 1,
    SpriteEffects_FlipVertically = 2,
    SpriteEffects_FlipBoth = SpriteEffects_FlipHorizontally | SpriteEffects_FlipVertically,
};

// Helper for resource barrier.
inline void TransitionResource(
    _In_ ID3D12GraphicsCommandList* commandList,
    _In_ ID3D12Resource* resource,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter) noexcept
{
    assert(commandList != nullptr);
    assert(resource != nullptr);

    if (stateBefore == stateAfter)
        return;

    D3D12_RESOURCE_BARRIER desc = {};
    desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    desc.Transition.pResource = resource;
    desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    desc.Transition.StateBefore = stateBefore;
    desc.Transition.StateAfter = stateAfter;

    commandList->ResourceBarrier(1, &desc);
}

// Shorthand for creating a root signature
inline HRESULT CreateRootSignature(
    _In_ ID3D12Device* device,
    _In_ const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
    _Out_ ID3D12RootSignature** rootSignature) noexcept
{
    Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
    Microsoft::WRL::ComPtr<ID3DBlob> pError;
    HRESULT hr = D3D12SerializeRootSignature(rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        hr = device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
            IID_PPV_ARGS(rootSignature)
        );
    }
    return hr;
}

// Helper class for COM exceptions
class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) noexcept : result(hr) {}

    const char* what() const noexcept override
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
        return s_str;
    }

    HRESULT get_result() const noexcept { return result; }

private:
    HRESULT result;
};
// Helper utility converts D3D API failures into exceptions.
inline void ThrowIfFailed(HRESULT hr) noexcept(false)
{
    if (FAILED(hr))
    {
        throw com_exception(hr);
    }
}
#ifndef IID_GRAPHICS_PPV_ARGS
#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
#endif
struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };
using ScopedHandle = std::unique_ptr<void, handle_closer>;

template<typename T>
inline T AlignUp(T size, size_t alignment) noexcept
{
    if (alignment > 0)
    {
        assert(((alignment - 1) & alignment) == 0);
        auto mask = static_cast<T>(alignment - 1);
        return (size + mask) & ~mask;
    }
    return size;
}

//------------------------------------------------------------------------------
// Return a vector with all elements equaling zero
inline XMVECTOR XM_CALLCONV XMVectorZero() 
{
#if defined(_XM_NO_INTRINSICS_)
	XMVECTORF32 vResult = { { { 0.0f, 0.0f, 0.0f, 0.0f } } };
	return vResult.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
	return vdupq_n_f32(0);
#elif defined(_XM_SSE_INTRINSICS_)
	return _mm_setzero_ps();
#endif
}

inline XMVECTOR XM_CALLCONV XMVectorEqual
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{
#if defined(_XM_NO_INTRINSICS_)

    XMVECTORU32 Control = { { {
            (V1.vector4_f32[0] == V2.vector4_f32[0]) ? 0xFFFFFFFF : 0,
            (V1.vector4_f32[1] == V2.vector4_f32[1]) ? 0xFFFFFFFF : 0,
            (V1.vector4_f32[2] == V2.vector4_f32[2]) ? 0xFFFFFFFF : 0,
            (V1.vector4_f32[3] == V2.vector4_f32[3]) ? 0xFFFFFFFF : 0,
        } } };
    return Control.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vceqq_f32( V1, V2 );
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_cmpeq_ps( V1, V2 );
#endif
}

//------------------------------------------------------------------------------

inline XMVECTOR XM_CALLCONV XMVectorDivide
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{
#if defined(_XM_NO_INTRINSICS_)
    XMVECTORF32 Result = { { {
            V1.vector4_f32[0] / V2.vector4_f32[0],
            V1.vector4_f32[1] / V2.vector4_f32[1],
            V1.vector4_f32[2] / V2.vector4_f32[2],
            V1.vector4_f32[3] / V2.vector4_f32[3]
        } } };
    return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || __aarch64__
    return vdivq_f32( V1, V2 );
#else
    // 2 iterations of Newton-Raphson refinement of reciprocal
    float32x4_t Reciprocal = vrecpeq_f32(V2);
    float32x4_t S = vrecpsq_f32( Reciprocal, V2 );
    Reciprocal = vmulq_f32( S, Reciprocal );
    S = vrecpsq_f32( Reciprocal, V2 );
    Reciprocal = vmulq_f32( S, Reciprocal );
    return vmulq_f32( V1, Reciprocal );
#endif
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_div_ps( V1, V2 );
#endif
}

/*
inline XMVECTOR XM_CALLCONV XMVectorMultiply
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{
#if defined(_XM_NO_INTRINSICS_)
    XMVECTORF32 Result = { { {
            V1.vector4_f32[0] * V2.vector4_f32[0],
            V1.vector4_f32[1] * V2.vector4_f32[1],
            V1.vector4_f32[2] * V2.vector4_f32[2],
            V1.vector4_f32[3] * V2.vector4_f32[3]
        } } };
    return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vmulq_f32( V1, V2 );
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_mul_ps( V1, V2 );
#endif
}

//*/

//------------------------------------------------------------------------------

_Use_decl_annotations_
inline void XMScalarSinCos
(
    float* pSin,
    float* pCos,
    float  Value
)
{
	using namespace DirectX;
    assert(pSin);
    assert(pCos);

    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = XM_1DIV2PI*Value;
    if (Value >= 0.0f)
    {
        quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
    }
    else
    {
        quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
    }
    float y = Value - XM_2PI*quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > XM_PIDIV2)
    {
        y = XM_PI - y;
        sign = -1.0f;
    }
    else if (y < -XM_PIDIV2)
    {
        y = -XM_PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *pSin = ( ( ( ( (-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f ) * y2 + 0.0083333310f ) * y2 - 0.16666667f ) * y2 + 1.0f ) * y;

    // 10-degree minimax approximation
    float p = ( ( ( ( -2.6051615e-07f * y2 + 2.4760495e-05f ) * y2 - 0.0013888378f ) * y2 + 0.041666638f ) * y2 - 0.5f ) * y2 + 1.0f;
    *pCos = sign*p;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
_Use_decl_annotations_
inline XMVECTOR XM_CALLCONV XMLoadFloat(const float* pSource)
{
    assert(pSource);
#if defined(_XM_NO_INTRINSICS_)
    XMVECTOR V;
    V.vector4_f32[0] = *pSource;
    V.vector4_f32[1] = 0.f;
    V.vector4_f32[2] = 0.f;
    V.vector4_f32[3] = 0.f;
    return V;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
    float32x4_t zero = vdupq_n_f32(0);
    return vld1q_lane_f32( pSource, zero, 0 );
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_load_ss( pSource );
#endif
}

inline XMVECTOR XM_CALLCONV XMVectorMergeXY
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{
#if defined(_XM_NO_INTRINSICS_)

    XMVECTORU32 Result = { { {
            V1.vector4_u32[0],
            V2.vector4_u32[0],
            V1.vector4_u32[1],
            V2.vector4_u32[1],
        } } };
    return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vzipq_f32( V1, V2 ).val[0];
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_unpacklo_ps( V1, V2 );
#endif
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

inline XMVECTOR XM_CALLCONV XMVectorNegate
(
    FXMVECTOR V
)
{
#if defined(_XM_NO_INTRINSICS_)

    XMVECTORF32 Result = { { {
            -V.vector4_f32[0],
            -V.vector4_f32[1],
            -V.vector4_f32[2],
            -V.vector4_f32[3]
        } } };
    return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vnegq_f32(V);
#elif defined(_XM_SSE_INTRINSICS_)
    XMVECTOR Z;

    Z = _mm_setzero_ps();

    return _mm_sub_ps( Z, V );
#endif
}

/*
inline XMVECTOR XM_CALLCONV XMVectorSubtract
(
    FXMVECTOR V1,
    FXMVECTOR V2
)
{
#if defined(_XM_NO_INTRINSICS_)

    XMVECTORF32 Result = { { {
            V1.vector4_f32[0] - V2.vector4_f32[0],
            V1.vector4_f32[1] - V2.vector4_f32[1],
            V1.vector4_f32[2] - V2.vector4_f32[2],
            V1.vector4_f32[3] - V2.vector4_f32[3]
        } } };
    return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vsubq_f32( V1, V2 );
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_sub_ps( V1, V2 );
#endif
}
//*/

//------------------------------------------------------------------------------

} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
