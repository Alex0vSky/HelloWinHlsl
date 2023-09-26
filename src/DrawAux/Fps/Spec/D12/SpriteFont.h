// DrawAux/Fps/Spec/D12/SpriteFont.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {

typedef SpriteEffects SpriteEffects_t;
typedef SpriteBatch SpriteBatch_t;
typedef BinaryReader BinaryReader_t;
typedef ResourceUploadBatch ResourceUploadBatch_t;


static const char spriteFontMagic[] = "DXTKfont";

class SpriteFont {
	class Impl;
	std::unique_ptr<Impl> pImpl;
    // Describes a single character glyph.
    struct Glyph
    {
        uint32_t Character;
        RECT Subrect;
        float XOffset;
        float YOffset;
        float XAdvance;
    };

	//------------------------------------------------------------------------------
	inline XMVECTOR XM_CALLCONV XMVectorNegativeMultiplySubtract
	(
		FXMVECTOR V1,
		FXMVECTOR V2,
		FXMVECTOR V3
	)
		const
	{
	#if defined(_XM_NO_INTRINSICS_)
		XMVECTORF32 Result = { { {
				V3.vector4_f32[0] - (V1.vector4_f32[0] * V2.vector4_f32[0]),
				V3.vector4_f32[1] - (V1.vector4_f32[1] * V2.vector4_f32[1]),
				V3.vector4_f32[2] - (V1.vector4_f32[2] * V2.vector4_f32[2]),
				V3.vector4_f32[3] - (V1.vector4_f32[3] * V2.vector4_f32[3])
			} } };
		return Result;
	#elif defined(_XM_ARM_NEON_INTRINSICS_)
	#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || __aarch64__
		return vfmsq_f32( V3, V1, V2 );
	#else
		return vmlsq_f32( V3, V1, V2 );
	#endif
	#elif defined(_XM_FMA3_INTRINSICS_)
		return _mm_fnmadd_ps(V1, V2, V3);
	#elif defined(_XM_SSE_INTRINSICS_)
		XMVECTOR R = _mm_mul_ps( V1, V2 );
		return _mm_sub_ps( V3, R );
	#endif
	}

	XMVECTOR XM_CALLCONV SpriteFont::MeasureString(_In_z_ wchar_t const* text, bool ignoreWhitespace = true) const;

	//------------------------------------------------------------------------------
	// Initialize a vector with four floating point values
	inline XMVECTOR XM_CALLCONV XMVectorSet
	(
		float x,
		float y,
		float z,
		float w
	)
	const
	{
	#if defined(_XM_NO_INTRINSICS_)
		XMVECTORF32 vResult = { { { x, y, z, w } } };
		return vResult.v;
	#elif defined(_XM_ARM_NEON_INTRINSICS_)
		float32x2_t V0 = vcreate_f32(
			static_cast<uint64_t>(*reinterpret_cast<const uint32_t *>(&x))
			| (static_cast<uint64_t>(*reinterpret_cast<const uint32_t *>(&y)) << 32));
		float32x2_t V1 = vcreate_f32(
			static_cast<uint64_t>(*reinterpret_cast<const uint32_t *>(&z))
			| (static_cast<uint64_t>(*reinterpret_cast<const uint32_t *>(&w)) << 32));
		return vcombine_f32(V0, V1);
	#elif defined(_XM_SSE_INTRINSICS_)
		return _mm_set_ps( w, z, y, x );
	#endif
	}

	inline XMVECTOR XM_CALLCONV XMVectorMax
	(
		FXMVECTOR V1,
		FXMVECTOR V2
	)
	const
	{
	#if defined(_XM_NO_INTRINSICS_)

		XMVECTORF32 Result = { { {
				(V1.vector4_f32[0] > V2.vector4_f32[0]) ? V1.vector4_f32[0] : V2.vector4_f32[0],
				(V1.vector4_f32[1] > V2.vector4_f32[1]) ? V1.vector4_f32[1] : V2.vector4_f32[1],
				(V1.vector4_f32[2] > V2.vector4_f32[2]) ? V1.vector4_f32[2] : V2.vector4_f32[2],
				(V1.vector4_f32[3] > V2.vector4_f32[3]) ? V1.vector4_f32[3] : V2.vector4_f32[3]
			} } };
		return Result.v;

	#elif defined(_XM_ARM_NEON_INTRINSICS_)
		return vmaxq_f32( V1, V2 );
	#elif defined(_XM_SSE_INTRINSICS_)
		return _mm_max_ps( V1, V2 );
	#endif
	}

public:
	// Construct from a binary blob created by the MakeSpriteFont utility and already loaded into memory.
	_Use_decl_annotations_
	SpriteFont::SpriteFont(
		ID3D12Device* device
		, ResourceUploadBatch_t& upload
		, uint8_t const* dataBlob
		, size_t dataSize
		, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorDest
		, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorDest
		, bool forceSRGB = false
	) {
		BinaryReader_t reader(dataBlob, dataSize);

		pImpl = std::make_unique<Impl>(device, upload, &reader, cpuDescriptorDest, gpuDescriptorDest, forceSRGB);
	}

	void XM_CALLCONV SpriteFont::DrawString(
		_In_ SpriteBatch_t* spriteBatch
		, _In_z_ wchar_t const* text
		, FXMVECTOR position
		, FXMVECTOR color
		, float rotation
		, FXMVECTOR origin
		, GXMVECTOR scale
		, SpriteEffects_t effects
		, float layerDepth) const;

};

class SpriteFont::Impl {
    size_t utfBufferSize;
    std::unique_ptr<wchar_t[]> utfBuffer;

	_Use_decl_annotations_
	void CreateShaderResourceView(
		ID3D12Device* device,
		ID3D12Resource* tex,
		D3D12_CPU_DESCRIPTOR_HANDLE srvDescriptor,
		bool isCubeMap)
	{
	#if defined(_MSC_VER) || !defined(_WIN32)
		const auto desc = tex->GetDesc();
	#else
		D3D12_RESOURCE_DESC tmpDesc;
		const auto& desc = *tex->GetDesc(&tmpDesc);
	#endif

		if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) != 0)
		{
			DebugTrace("ERROR: CreateShaderResourceView called on a resource created without support for SRV.\n");
			throw std::runtime_error("Can't have D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE");
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		const UINT mipLevels = (desc.MipLevels) ? static_cast<UINT>(desc.MipLevels) : static_cast<UINT>(-1);

		switch (desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (desc.DepthOrArraySize > 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				srvDesc.Texture1DArray.MipLevels = mipLevels;
				srvDesc.Texture1DArray.ArraySize = static_cast<UINT>(desc.DepthOrArraySize);
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				srvDesc.Texture1D.MipLevels = mipLevels;
			}
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (isCubeMap)
			{
				if (desc.DepthOrArraySize > 6)
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					srvDesc.TextureCubeArray.MipLevels = mipLevels;
					srvDesc.TextureCubeArray.NumCubes = static_cast<UINT>(desc.DepthOrArraySize / 6);
				}
				else
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MipLevels = mipLevels;
				}
			}
			else if (desc.DepthOrArraySize > 1)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MipLevels = mipLevels;
				srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(desc.DepthOrArraySize);
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = mipLevels;
			}
			break;

		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MipLevels = mipLevels;
			break;

		case D3D12_RESOURCE_DIMENSION_BUFFER:
			DebugTrace("ERROR: CreateShaderResourceView cannot be used with DIMENSION_BUFFER.\n\tUse CreateBufferShaderResourceView.\n");
			throw std::invalid_argument("buffer resources not supported");

		case D3D12_RESOURCE_DIMENSION_UNKNOWN:
		default:
			DebugTrace("ERROR: CreateShaderResourceView cannot be used with DIMENSION_UNKNOWN (%d).\n", desc.Dimension);
			throw std::invalid_argument("unknown resource dimension");
		}

		device->CreateShaderResourceView(tex, &srvDesc, srvDescriptor);
	}

	_Use_decl_annotations_
	void SpriteFont::Impl::CreateTextureResource(
		ID3D12Device* device,
		ResourceUploadBatch_t& upload,
		uint32_t width, uint32_t height,
		DXGI_FORMAT format,
		uint32_t stride, uint32_t rows,
		const uint8_t* data) noexcept(false)
	{
		const uint64_t sliceBytes = uint64_t(stride) * uint64_t(rows);
		if (sliceBytes > UINT32_MAX)
		{
			DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
			throw std::overflow_error("Invalid .spritefont file");
		}

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;

		const CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

		Sys::Hr hr;
		hr = device->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			c_initialCopyTargetState,
			nullptr,
			IID_PPV_ARGS( textureResource.ReleaseAndGetAddressOf( ) ) );

		SetDebugObjectName(textureResource.Get(), L"SpriteFont:Texture");

		D3D12_SUBRESOURCE_DATA initData = { data, static_cast<LONG_PTR>(stride), static_cast<LONG_PTR>(sliceBytes) };

		upload.Upload(
			textureResource.Get(),
			0,
			&initData,
			1);

		upload.Transition(
			textureResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	//--------------------------------------------------------------------------------------
	inline DXGI_FORMAT MakeSRGB(_In_ DXGI_FORMAT format) noexcept
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			return format;
		}
	}

	// Looks up the requested glyph, falling back to the default character if it is not in the font.
	SpriteFont::Glyph const* SpriteFont::Impl::FindGlyph(wchar_t character) const
	{
		// Rather than use std::lower_bound (which includes a slow debug path when built for _DEBUG),
		// we implement a binary search inline to ensure sufficient Debug build performance to be useful
		// for text-heavy applications.

		size_t lower = 0;
		size_t higher = glyphs.size() - 1;
		size_t index = higher / 2;
		const size_t size = glyphs.size();

		while (index < size)
		{
			const auto curChar = glyphsIndex[index];
			if (curChar == character) { return &glyphs[index]; }
			if (curChar < character)
			{
				lower = index + 1;
			}
			else
			{
				higher = index - 1;
			}
			if (higher < lower) { break; }
			else if (higher - lower <= 4)
			{
				for (index = lower; index <= higher; index++)
				{
					if (glyphsIndex[index] == character)
					{
						return &glyphs[index];
					}
				}
			}
			index = lower + ((higher - lower) / 2);
		}

		if (defaultGlyph)
		{
			return defaultGlyph;
		}

		DebugTrace("ERROR: SpriteFont encountered a character not in the font (%u, %C), and no default glyph was provided\n", character, character);
		throw std::runtime_error("Character not in font");
	}

	// Sets the missing-character fallback glyph.
	void Impl::SetDefaultCharacter(wchar_t character)
	{
		defaultGlyph = nullptr;

		if (character)
		{
			defaultGlyph = FindGlyph(character);
		}
		return;
	}

public:
    Impl(ID3D12Device* device,
        ResourceUploadBatch_t& upload,
        BinaryReader_t* reader,
        D3D12_CPU_DESCRIPTOR_HANDLE cpuDesc,
        D3D12_GPU_DESCRIPTOR_HANDLE gpuDesc,
        bool forceSRGB) noexcept(false)
		: texture{ }
		, textureSize{ }
		, defaultGlyph( nullptr )
		, lineSpacing( 0 )
		, utfBufferSize( 0 )
	{ 
		// Validate the header.
		for (char const* magic = spriteFontMagic; *magic; magic++)
		{
			if (reader->Read<uint8_t>() != *magic)
			{
				DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
				throw std::runtime_error("Not a MakeSpriteFont output binary");
			}
		}

		// Read the glyph data.
		auto glyphCount = reader->Read<uint32_t>();
		auto glyphData = reader->ReadArray<Glyph>(glyphCount);

		glyphs.assign(glyphData, glyphData + glyphCount);
		glyphsIndex.reserve(glyphs.size());

		//for (auto& glyph : glyphs)
		//{
		//	glyphsIndex.emplace_back(glyph.Character);
		//}
		//std::for_each(
		//		glyphs.begin(), glyphs.end()
		//		, [this](const auto& glyph) { 
		//			glyphsIndex.emplace_back( glyph.Character );
		//		}
		//	);
		std::transform( 
				glyphs.begin(), glyphs.end()
				, std::back_inserter( glyphsIndex ) 
				, [this](const auto& glyph) { 
					return glyph.Character;
				}
			);

		// Read font properties.
		lineSpacing = reader->Read<float>();

		SetDefaultCharacter(static_cast<wchar_t>(reader->Read<uint32_t>()));

		// Read the texture data.
		auto textureWidth = reader->Read<uint32_t>();
		auto textureHeight = reader->Read<uint32_t>();
		auto textureFormat = reader->Read<DXGI_FORMAT>();
		auto textureStride = reader->Read<uint32_t>();
		auto textureRows = reader->Read<uint32_t>();

		const uint64_t dataSize = uint64_t(textureStride) * uint64_t(textureRows);
		if (dataSize > UINT32_MAX)
		{
			DebugTrace("ERROR: SpriteFont provided with an invalid .spritefont file\n");
			throw std::overflow_error("Invalid .spritefont file");
		}

		auto textureData = reader->ReadArray<uint8_t>(static_cast<size_t>(dataSize));

		if (forceSRGB)
		{
			textureFormat = MakeSRGB(textureFormat);
		}

		// Create the D3D texture.
		CreateTextureResource(
			device,
			upload,
			textureWidth, textureHeight,
			textureFormat,
			textureStride, textureRows,
			textureData);

		// Create the shader resource view
		CreateShaderResourceView(
			device, textureResource.Get(),
			cpuDesc, false);

		// Save off the GPU descriptor pointer and size.
		texture = gpuDesc;
		textureSize = XMUINT2(textureWidth, textureHeight);
	}

	// The core glyph layout algorithm, shared between DrawString and MeasureString.
	template<typename TAction>
	void ForEachGlyph(_In_z_ wchar_t const* text, TAction action, bool ignoreWhitespace) const
	{
		float x = 0;
		float y = 0;

		for (; *text; text++)
		{
			const wchar_t character = *text;

			switch (character)
			{
			case '\r':
				// Skip carriage returns.
				continue;

			case '\n':
				// New line.
				x = 0;
				y += lineSpacing;
				break;

			default:
				// Output this character.
				auto glyph = FindGlyph(character);

				x += glyph->XOffset;

				if (x < 0)
					x = 0;

				const float advance = float(glyph->Subrect.right) - float(glyph->Subrect.left) + glyph->XAdvance;

				if (!ignoreWhitespace
					|| !iswspace(character)
					|| ((glyph->Subrect.right - glyph->Subrect.left) > 1)
					|| ((glyph->Subrect.bottom - glyph->Subrect.top) > 1))
				{
					action(glyph, x, y, advance);
				}

				x += advance;
				break;
			}
		}
	}

    // Fields.
    CPtr< ID3D12Resource > textureResource;
    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    XMUINT2 textureSize;
    std::vector< Glyph > glyphs;
    std::vector<uint32_t> glyphsIndex;
    Glyph const* defaultGlyph;
    float lineSpacing;
};

	inline 
	XMVECTOR XM_CALLCONV SpriteFont::MeasureString(_In_z_ wchar_t const* text, bool ignoreWhitespace) const
	{
		XMVECTOR result = XMVectorZero();

		pImpl->ForEachGlyph(text, [&](Glyph const* glyph, float x, float y, float advance)
			{
				UNREFERENCED_PARAMETER(advance);

				auto const w = static_cast<float>(glyph->Subrect.right - glyph->Subrect.left);
				auto h = static_cast<float>(glyph->Subrect.bottom - glyph->Subrect.top) + glyph->YOffset;

				h = iswspace(wchar_t(glyph->Character)) ?
					pImpl->lineSpacing :
					std::max(h, pImpl->lineSpacing);

				result = XMVectorMax(result, XMVectorSet(x + w, y + h, 0, 0));
			}, ignoreWhitespace);

		return result;
	}

//------------------------------------------------------------------------------
_Use_decl_annotations_
inline XMVECTOR XM_CALLCONV XMLoadInt4
(
    const uint32_t* pSource
)
{
    assert(pSource);

#if defined(_XM_NO_INTRINSICS_)
    XMVECTOR V;
    V.vector4_u32[0] = pSource[0];
    V.vector4_u32[1] = pSource[1];
    V.vector4_u32[2] = pSource[2];
    V.vector4_u32[3] = pSource[3];
    return V;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
    return vld1q_u32( pSource );
#elif defined(_XM_SSE_INTRINSICS_)
    __m128i V = _mm_loadu_si128( reinterpret_cast<const __m128i*>(pSource) );
    return _mm_castsi128_ps(V);
#endif
}

inline XMVECTOR XM_CALLCONV XMConvertVectorIntToFloat
(
    FXMVECTOR    VInt,
    uint32_t     DivExponent
)
{
    assert(DivExponent<32);
#if defined(_XM_NO_INTRINSICS_)
    float fScale = 1.0f / static_cast<float>(1U << DivExponent);
    uint32_t ElementIndex = 0;
    XMVECTOR Result;
    do {
        auto iTemp = static_cast<int32_t>(VInt.vector4_u32[ElementIndex]);
        Result.vector4_f32[ElementIndex] = static_cast<float>(iTemp) * fScale;
    } while (++ElementIndex<4);
    return Result;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
    float fScale = 1.0f / (float)(1U << DivExponent);
    float32x4_t vResult = vcvtq_f32_s32( VInt );
    return vmulq_n_f32( vResult, fScale );
#else // _XM_SSE_INTRINSICS_
    // Convert to floats
    XMVECTOR vResult = _mm_cvtepi32_ps(_mm_castps_si128(VInt));
    // Convert DivExponent into 1.0f/(1<<DivExponent)
    uint32_t uScale = 0x3F800000U - (DivExponent << 23);
    // Splat the scalar value
    __m128i vScale = _mm_set1_epi32(static_cast<int>(uScale));
    vResult = _mm_mul_ps(vResult,_mm_castsi128_ps(vScale));
    return vResult;
#endif
}

//------------------------------------------------------------------------------


#pragma region swizzle
//------------------------------------------------------------------------------

// General swizzle template
template<uint32_t SwizzleX, uint32_t SwizzleY, uint32_t SwizzleZ, uint32_t SwizzleW>
    inline XMVECTOR     XM_CALLCONV     XMVectorSwizzle(FXMVECTOR V)
{
    static_assert(SwizzleX <= 3, "SwizzleX template parameter out of range");
    static_assert(SwizzleY <= 3, "SwizzleY template parameter out of range");
    static_assert(SwizzleZ <= 3, "SwizzleZ template parameter out of range");
    static_assert(SwizzleW <= 3, "SwizzleW template parameter out of range");

#if defined(_XM_SSE_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
    return XM_PERMUTE_PS( V, _MM_SHUFFLE( SwizzleW, SwizzleZ, SwizzleY, SwizzleX ) );
#else

    return XMVectorSwizzle( V, SwizzleX, SwizzleY, SwizzleZ, SwizzleW );

#endif
}

// Specialized swizzles
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,1,2,3>(FXMVECTOR V) { return V; }

#if defined(_XM_SSE_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,1,0,1>(FXMVECTOR V) { return _mm_movelh_ps(V,V); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,3,2,3>(FXMVECTOR V) { return _mm_movehl_ps(V,V); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,1,1>(FXMVECTOR V) { return _mm_unpacklo_ps(V,V); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,2,3,3>(FXMVECTOR V) { return _mm_unpackhi_ps(V,V); }
#endif

#if defined(_XM_SSE3_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,2,2>(FXMVECTOR V) { return _mm_moveldup_ps(V); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,1,3,3>(FXMVECTOR V) { return _mm_movehdup_ps(V); }
#endif

#if defined(_XM_AVX2_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,0,0>(FXMVECTOR V) { return _mm_broadcastss_ps( V ); }
#endif

#if defined(_XM_ARM_NEON_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,0,0>(FXMVECTOR V) { return vdupq_lane_f32( vget_low_f32(V), 0); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,1,1,1>(FXMVECTOR V) { return vdupq_lane_f32( vget_low_f32(V), 1); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,2,2,2>(FXMVECTOR V) { return vdupq_lane_f32( vget_high_f32(V), 0); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<3,3,3,3>(FXMVECTOR V) { return vdupq_lane_f32( vget_high_f32(V), 1); }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,0,3,2>(FXMVECTOR V) { return vrev64q_f32(V); }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,1,0,1>(FXMVECTOR V) { float32x2_t vt = vget_low_f32(V); return vcombine_f32( vt, vt ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,3,2,3>(FXMVECTOR V) { float32x2_t vt = vget_high_f32(V); return vcombine_f32( vt, vt ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,0,1,0>(FXMVECTOR V) { float32x2_t vt = vrev64_f32( vget_low_f32(V) ); return vcombine_f32( vt, vt ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<3,2,3,2>(FXMVECTOR V) { float32x2_t vt = vrev64_f32( vget_high_f32(V) ); return vcombine_f32( vt, vt ); }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,1,3,2>(FXMVECTOR V) { return vcombine_f32( vget_low_f32(V), vrev64_f32( vget_high_f32(V) ) ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,0,2,3>(FXMVECTOR V) { return vcombine_f32( vrev64_f32( vget_low_f32(V) ), vget_high_f32(V) ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,3,1,0>(FXMVECTOR V) { return vcombine_f32( vget_high_f32(V), vrev64_f32( vget_low_f32(V) ) ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<3,2,0,1>(FXMVECTOR V) { return vcombine_f32( vrev64_f32( vget_high_f32(V) ), vget_low_f32(V) ); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<3,2,1,0>(FXMVECTOR V) { return vcombine_f32( vrev64_f32( vget_high_f32(V) ), vrev64_f32( vget_low_f32(V) ) ); }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,2,2>(FXMVECTOR V) { return vtrnq_f32(V,V).val[0]; }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,1,3,3>(FXMVECTOR V) { return vtrnq_f32(V,V).val[1]; }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,0,1,1>(FXMVECTOR V) { return vzipq_f32(V,V).val[0]; }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,2,3,3>(FXMVECTOR V) { return vzipq_f32(V,V).val[1]; }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<0,2,0,2>(FXMVECTOR V) { return vuzpq_f32(V,V).val[0]; }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,3,1,3>(FXMVECTOR V) { return vuzpq_f32(V,V).val[1]; }

template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<1,2,3,0>(FXMVECTOR V) { return vextq_f32(V, V, 1); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<2,3,0,1>(FXMVECTOR V) { return vextq_f32(V, V, 2); }
template<> inline XMVECTOR      XM_CALLCONV     XMVectorSwizzle<3,0,1,2>(FXMVECTOR V) { return vextq_f32(V, V, 3); }
#endif // _XM_ARM_NEON_INTRINSICS_ && !_XM_NO_INTRINSICS_

//------------------------------------------------------------------------------

#pragma endregion

//------------------------------------------------------------------------------

	inline 
	void XM_CALLCONV SpriteFont::DrawString(
		SpriteBatch_t* spriteBatch
		, wchar_t const* text
		, FXMVECTOR position
		, FXMVECTOR color
		, float rotation
		, FXMVECTOR origin
		, GXMVECTOR scale
		, SpriteEffects_t effects
		, float layerDepth) const
	{
		static_assert(SpriteEffects_FlipHorizontally == 1 &&
			SpriteEffects_FlipVertically == 2, "If you change these enum values, the following tables must be updated to match");
		using namespace DirectX;

	// Lookup table indicates which way to move along each axis per SpriteEffects enum value.
		static XMVECTORF32 axisDirectionTable[4] =
		{
			{ { { -1, -1, 0, 0 } } },
			{ { {  1, -1, 0, 0 } } },
			{ { { -1,  1, 0, 0 } } },
			{ { {  1,  1, 0, 0 } } },
		};

		// Lookup table indicates which axes are mirrored for each SpriteEffects enum value.
		static XMVECTORF32 axisIsMirroredTable[4] =
		{
			{ { { 0, 0, 0, 0 } } },
			{ { { 1, 0, 0, 0 } } },
			{ { { 0, 1, 0, 0 } } },
			{ { { 1, 1, 0, 0 } } },
		};

		XMVECTOR baseOffset = origin;

		// If the text is mirrored, offset the start position accordingly.
		if (effects)
		{
			baseOffset = XMVectorNegativeMultiplySubtract(
				MeasureString(text),
				axisIsMirroredTable[effects & 3],
				baseOffset);
		}

		// Draw each character in turn.
		pImpl->ForEachGlyph(text, [&](Glyph const* glyph, float x, float y, float advance)
			{
				UNREFERENCED_PARAMETER(advance);

				XMVECTOR offset = XMVectorMultiplyAdd(XMVectorSet(x, y + glyph->YOffset, 0, 0), axisDirectionTable[effects & 3], baseOffset);

				if (effects)
				{
					// For mirrored characters, specify bottom and/or right instead of top left.
					XMVECTOR glyphRect = XMConvertVectorIntToFloat(XMLoadInt4(reinterpret_cast<uint32_t const*>(&glyph->Subrect)), 0);

					// xy = glyph width/height.
					glyphRect = XMVectorSubtract( XMVectorSwizzle<2, 3, 0, 1>(glyphRect), glyphRect );

					offset = XMVectorMultiplyAdd(glyphRect, axisIsMirroredTable[effects & 3], offset);
				}

				spriteBatch->Draw(pImpl->texture, pImpl->textureSize, position, &glyph->Subrect, color, rotation, offset, scale, effects, layerDepth);
			}, true);

	}

} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
