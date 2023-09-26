// DrawAux/Fps/Spec/D12/SpriteBatch.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {
//typedef DirectX::ResourceUploadBatch ResourceUploadBatch_t;
typedef ResourceUploadBatch ResourceUploadBatch_t;
//typedef DirectX::GraphicsResource GraphicsResource_t;
typedef GraphicsResource GraphicsResource_t;
//typedef DirectX::GraphicsMemory GraphicsMemory_t;
typedef GraphicsMemory GraphicsMemory_t;

namespace
{
    inline bool operator != (D3D12_GPU_DESCRIPTOR_HANDLE a, D3D12_GPU_DESCRIPTOR_HANDLE b) noexcept
    {
        return a.ptr != b.ptr;
    }
    inline bool operator < (D3D12_GPU_DESCRIPTOR_HANDLE a, D3D12_GPU_DESCRIPTOR_HANDLE b) noexcept
    {
        return a.ptr < b.ptr;
    }
} // namespace

enum SpriteSortMode
{
    SpriteSortMode_Deferred,
    SpriteSortMode_Immediate,
    SpriteSortMode_Texture,
    SpriteSortMode_BackToFront,
    SpriteSortMode_FrontToBack,
};

// Encapsulates all render target state when creating pipeline state objects
class RenderTargetState
{
public:
    RenderTargetState() noexcept
        : sampleMask(~0U)
        , numRenderTargets(0)
        , rtvFormats{}
        , dsvFormat(DXGI_FORMAT_UNKNOWN)
        , sampleDesc{}
        , nodeMask(0)
    {
    }

    RenderTargetState(const RenderTargetState&) = default;
    RenderTargetState& operator=(const RenderTargetState&) = default;

    RenderTargetState(RenderTargetState&&) = default;
    RenderTargetState& operator=(RenderTargetState&&) = default;

    // Single render target convenience constructor
    RenderTargetState(
        _In_ DXGI_FORMAT rtFormat,
        _In_ DXGI_FORMAT dsFormat) noexcept
        : sampleMask(UINT_MAX)
        , numRenderTargets(1)
        , rtvFormats{}
        , dsvFormat(dsFormat)
        , sampleDesc{}
        , nodeMask(0)
    {
        sampleDesc.Count = 1;
        rtvFormats[0] = rtFormat;
    }

    // Convenience constructors converting from DXGI_SWAPCHAIN_DESC
#if defined(__dxgi_h__) || defined(__d3d11_x_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
    RenderTargetState(
        _In_ const DXGI_SWAP_CHAIN_DESC* desc,
        _In_ DXGI_FORMAT dsFormat) noexcept
        : sampleMask(UINT_MAX)
        , numRenderTargets(1)
        , rtvFormats{}
        , dsvFormat(dsFormat)
        , sampleDesc{}
        , nodeMask(0)
    {
        rtvFormats[0] = desc->BufferDesc.Format;
        sampleDesc = desc->SampleDesc;
    }
#endif

#if defined(__dxgi1_2_h__) || defined(__d3d11_x_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
    RenderTargetState(
        _In_ const DXGI_SWAP_CHAIN_DESC1* desc,
        _In_ DXGI_FORMAT dsFormat) noexcept
        : sampleMask(UINT_MAX)
        , numRenderTargets(1)
        , rtvFormats{}
        , dsvFormat(dsFormat)
        , sampleDesc{}
        , nodeMask(0)
    {
        rtvFormats[0] = desc->Format;
        sampleDesc = desc->SampleDesc;
    }
#endif

    uint32_t            sampleMask;
    uint32_t            numRenderTargets;
    DXGI_FORMAT         rtvFormats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
    DXGI_FORMAT         dsvFormat;
    DXGI_SAMPLE_DESC    sampleDesc;
    uint32_t            nodeMask;
};

class SpriteBatchPipelineStateDescription
{
public:
    explicit SpriteBatchPipelineStateDescription(
        const RenderTargetState& renderTarget,
        _In_opt_ const D3D12_BLEND_DESC* blend = nullptr,
        _In_opt_ const D3D12_DEPTH_STENCIL_DESC* depthStencil = nullptr,
        _In_opt_ const D3D12_RASTERIZER_DESC* rasterizer = nullptr,
        _In_opt_ const D3D12_GPU_DESCRIPTOR_HANDLE* isamplerDescriptor = nullptr) noexcept
        :
        blendDesc(blend ? *blend : s_DefaultBlendDesc),
        depthStencilDesc(depthStencil ? *depthStencil : s_DefaultDepthStencilDesc),
        rasterizerDesc(rasterizer ? *rasterizer : s_DefaultRasterizerDesc),
        renderTargetState(renderTarget),
        samplerDescriptor{},
        customRootSignature(nullptr),
        customVertexShader{},
        customPixelShader{}
    {
        if (isamplerDescriptor)
            this->samplerDescriptor = *isamplerDescriptor;
    }

    D3D12_BLEND_DESC            blendDesc;
    D3D12_DEPTH_STENCIL_DESC    depthStencilDesc;
    D3D12_RASTERIZER_DESC       rasterizerDesc;
    RenderTargetState           renderTargetState;
    D3D12_GPU_DESCRIPTOR_HANDLE samplerDescriptor;
    ID3D12RootSignature*        customRootSignature;
    D3D12_SHADER_BYTECODE       customVertexShader;
    D3D12_SHADER_BYTECODE       customPixelShader;

private:
    static const D3D12_BLEND_DESC           s_DefaultBlendDesc;
    static const D3D12_RASTERIZER_DESC      s_DefaultRasterizerDesc;
    static const D3D12_DEPTH_STENCIL_DESC   s_DefaultDepthStencilDesc;
};

// Matches CommonStates::AlphaBlend
inline const D3D12_BLEND_DESC SpriteBatchPipelineStateDescription::s_DefaultBlendDesc =
{
	FALSE, // AlphaToCoverageEnable
	FALSE, // IndependentBlendEnable
	{ {
		TRUE, // BlendEnable
		FALSE, // LogicOpEnable
		D3D12_BLEND_ONE, // SrcBlend
		D3D12_BLEND_INV_SRC_ALPHA, // DestBlend
		D3D12_BLEND_OP_ADD, // BlendOp
		D3D12_BLEND_ONE, // SrcBlendAlpha
		D3D12_BLEND_INV_SRC_ALPHA, // DestBlendAlpha
		D3D12_BLEND_OP_ADD, // BlendOpAlpha
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	} }
};
// Same to CommonStates::CullCounterClockwise
inline const D3D12_RASTERIZER_DESC SpriteBatchPipelineStateDescription::s_DefaultRasterizerDesc =
{
    D3D12_FILL_MODE_SOLID,
    D3D12_CULL_MODE_BACK,
    FALSE, // FrontCounterClockwise
    D3D12_DEFAULT_DEPTH_BIAS,
    D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
    D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    TRUE, // DepthClipEnable
    TRUE, // MultisampleEnable
    FALSE, // AntialiasedLineEnable
    0, // ForcedSampleCount
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
};

// Same as CommonStates::DepthNone
inline const D3D12_DEPTH_STENCIL_DESC SpriteBatchPipelineStateDescription::s_DefaultDepthStencilDesc =
{
    FALSE, // DepthEnable
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_COMPARISON_FUNC_LESS_EQUAL, // DepthFunc
    FALSE, // StencilEnable
    D3D12_DEFAULT_STENCIL_READ_MASK,
    D3D12_DEFAULT_STENCIL_WRITE_MASK,
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    }, // FrontFace
    {
        D3D12_STENCIL_OP_KEEP, // StencilFailOp
        D3D12_STENCIL_OP_KEEP, // StencilDepthFailOp
        D3D12_STENCIL_OP_KEEP, // StencilPassOp
        D3D12_COMPARISON_FUNC_ALWAYS // StencilFunc
    } // BackFace
};

class SpriteBatch {
    // Private implementation.
    struct Impl;

    std::unique_ptr<Impl> pImpl;

    static const XMMATRIX MatrixIdentity;
    static const XMFLOAT2 Float2Zero;

 public:
	// Public constructor.
	SpriteBatch(_In_ ID3D12Device* device, ResourceUploadBatch_t& upload,
		const SpriteBatchPipelineStateDescription& psoDesc,
		_In_opt_ const D3D12_VIEWPORT* viewport = nullptr);

	virtual ~SpriteBatch() = default;

	SpriteBatch::SpriteBatch(SpriteBatch&&) noexcept = default;
	SpriteBatch& SpriteBatch::operator= (SpriteBatch&&) noexcept = default;

	SpriteBatch(SpriteBatch const&) = delete;
	SpriteBatch& operator= (SpriteBatch const&) = delete;

    // Begin/End a batch of sprite drawing operations.
    void XM_CALLCONV Begin(
        ID3D12GraphicsCommandList* commandList,
        SpriteSortMode sortMode = SpriteSortMode_Deferred,
        FXMMATRIX transformMatrix = MatrixIdentity);
    void __cdecl End();

    // Set viewport for sprite transformation
    void __cdecl SetViewport(const D3D12_VIEWPORT& viewPort);

	void XM_CALLCONV Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
		XMUINT2 const& textureSize,
		FXMVECTOR position,
		RECT const* sourceRectangle,
		FXMVECTOR color,
		float rotation,
		FXMVECTOR origin,
		GXMVECTOR scale,
		SpriteEffects effects,
		float layerDepth);
};

#if (DIRECTX_MATH_VERSION < 315)
#define XM_ALIGNED_STRUCT(x) __declspec(align(x)) struct
#endif


// Derive from this to customize operator new and delete for
// types that have special heap alignment requirements.
//
// Example usage:
//
//      XM_ALIGNED_STRUCT(16) MyAlignedType : public AlignedNew<MyAlignedType>

template<typename TDerived>
struct AlignedNew
{
    // Allocate aligned memory.
    static void* operator new (size_t size)
    {
        const size_t alignment = alignof(TDerived);

        static_assert(alignment > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a __declspec(align) on TDerived?");
        static_assert(((alignment - 1) & alignment) == 0, "AlignedNew only works with power of two alignment");

    #ifdef _WIN32
        void* ptr = _aligned_malloc(size, alignment);
    #else
                // This C++17 Standard Library function is currently NOT
                // implemented for the Microsoft Standard C++ Library.
        void* ptr = aligned_alloc(alignment, size);
    #endif
        if (!ptr)
            throw std::bad_alloc();

        return ptr;
    }


    // Free aligned memory.
    static void operator delete (void* ptr)
    {
    #ifdef _WIN32
        _aligned_free(ptr);
    #else
        free(ptr);
    #endif
    }


    // Array overloads.
    static void* operator new[](size_t size)
    {
        static_assert((sizeof(TDerived) % alignof(TDerived) == 0), "AlignedNew expects type to be padded to the alignment");

        return operator new(size);
    }


        static void operator delete[](void* ptr)
    {
        operator delete(ptr);
    }
};

// Pool manager ensures that only a single TData instance is created for each unique TKey.
// This is used to avoid duplicate resource creation, so that for instance a caller can
// create any number of SpriteBatch instances, but these can internally share shaders and
// vertex buffer if more than one SpriteBatch uses the same underlying D3D device.
template<typename TKey, typename TData, typename... TConstructorArgs>
class SharedResourcePool
{
public:
    SharedResourcePool() noexcept(false)
        : mResourceMap(std::make_shared<ResourceMap>())
    {
    }

    SharedResourcePool(SharedResourcePool const&) = delete;
    SharedResourcePool& operator= (SharedResourcePool const&) = delete;

    // Allocates or looks up the shared TData instance for the specified key.
    std::shared_ptr<TData> DemandCreate(TKey key, TConstructorArgs... args)
    {
        std::lock_guard<std::mutex> lock(mResourceMap->mutex);

        // Return an existing instance?
        auto pos = mResourceMap->find(key);

        if (pos != mResourceMap->end())
        {
            auto existingValue = pos->second.lock();

            if (existingValue)
                return existingValue;
            else
                mResourceMap->erase(pos);
        }

        // Allocate a new instance.
        auto newValue = std::make_shared<WrappedData>(key, mResourceMap, args...);

        auto entry = std::make_pair(key, newValue);
        mResourceMap->insert(entry);

        return newValue;
    }


private:
    // Keep track of all allocated TData instances.
    struct ResourceMap : public std::map<TKey, std::weak_ptr<TData>>
    {
        std::mutex mutex;
    };

    std::shared_ptr<ResourceMap> mResourceMap;


    // Wrap TData with our own subclass, so we can hook the destructor
    // to remove instances from our pool before they are freed.
    struct WrappedData : public TData
    {
        WrappedData(TKey key, std::shared_ptr<ResourceMap> const& resourceMap, TConstructorArgs... args)
            : TData(key, args...),
            mKey(key),
            mResourceMap(resourceMap)
        {
        }

        WrappedData(WrappedData&&) = default;
        WrappedData& operator= (WrappedData&&) = default;

        WrappedData(WrappedData const&) = delete;
        WrappedData& operator= (WrappedData const&) = delete;

        ~WrappedData()
        {
            const std::lock_guard<std::mutex> lock(mResourceMap->mutex);

            auto const pos = mResourceMap->find(mKey);

            // Check for weak reference expiry before erasing, in case DemandCreate runs on
            // a different thread at the same time as a previous instance is being destroyed.
            // We mustn't erase replacement objects that have just been added!
            if (pos != mResourceMap->end() && pos->second.expired())
            {
                mResourceMap->erase(pos);
            }
        }

        TKey mKey;
        std::shared_ptr<ResourceMap> mResourceMap;
    };
};

// Info about a single sprite that is waiting to be drawn.
XM_ALIGNED_STRUCT(16) SpriteInfo : public AlignedNew<SpriteInfo> {
    XMFLOAT4A source;
    XMFLOAT4A destination;
    XMFLOAT4A color;
    XMFLOAT4A originRotationDepth;
    D3D12_GPU_DESCRIPTOR_HANDLE texture;
    XMVECTOR textureSize;
    unsigned int flags;

    // Combine values from the public SpriteEffects enum with these internal-only flags.
    static constexpr unsigned int SourceInTexels = 4;
    static constexpr unsigned int DestSizeInPixels = 8;

    static_assert((SpriteEffects_FlipBoth & (SourceInTexels | DestSizeInPixels)) == 0, "Flag bits must not overlap");
};

//------------------------------------------------------------------------------

inline XMVECTOR XM_CALLCONV XMVectorReciprocal
(
    FXMVECTOR V
)
{
#if defined(_XM_NO_INTRINSICS_)
    XMVECTORF32 Result = { { {
            1.f / V.vector4_f32[0],
            1.f / V.vector4_f32[1],
            1.f / V.vector4_f32[2],
            1.f / V.vector4_f32[3]
        } } };
    return Result.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || __aarch64__
    float32x4_t one = vdupq_n_f32(1.0f);
    return vdivq_f32(one,V);
#else
    // 2 iterations of Newton-Raphson refinement
    float32x4_t Reciprocal = vrecpeq_f32(V);
    float32x4_t S = vrecpsq_f32( Reciprocal, V );
    Reciprocal = vmulq_f32( S, Reciprocal );
    S = vrecpsq_f32( Reciprocal, V );
    return vmulq_f32( S, Reciprocal );
#endif
#elif defined(_XM_SSE_INTRINSICS_)
    return _mm_div_ps(DirectX::g_XMOne,V);
#endif
}

// Internal SpriteBatch implementation class.
XM_ALIGNED_STRUCT(16) 
SpriteBatch::Impl : public AlignedNew<SpriteBatch::Impl> {
	// Sets up D3D device state ready for drawing sprites.
	void PrepareForRendering()
	{
		auto commandList = mCommandList.Get();

		// Set root signature
		commandList->SetGraphicsRootSignature(mRootSignature.Get());

		// Set render state
		commandList->SetPipelineState(mPSO.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set the index buffer.
		commandList->IASetIndexBuffer(&mDeviceResources->indexBufferView);

		// Set the transform matrix.
		const XMMATRIX transformMatrix = (mRotation == DXGI_MODE_ROTATION_UNSPECIFIED)
			? mTransformMatrix
			: (mTransformMatrix * GetViewportTransform(mRotation));

		mConstantBuffer = GraphicsMemory_t::Get(mDeviceResources->mDevice).AllocateConstant(transformMatrix);
		commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ConstantBuffer, mConstantBuffer.GpuAddress());
	}

	// Populates the mSortedSprites vector with pointers to individual elements of the mSpriteQueue array.
	void GrowSortedSprites()
	{
		const size_t previousSize = mSortedSprites.size();

		mSortedSprites.resize(mSpriteQueueCount);

		for (size_t i = previousSize; i < mSpriteQueueCount; i++)
		{
			mSortedSprites[i] = &mSpriteQueue[i];
		}
	}

	// Sorts the array of queued sprites.
	void SortSprites()
	{
		// Fill the mSortedSprites vector.
		if (mSortedSprites.size() < mSpriteQueueCount)
		{
			GrowSortedSprites();
		}

		switch (mSortMode)
		{
		case SpriteSortMode_Texture:
			// Sort by texture.
			std::sort(mSortedSprites.begin(),
				mSortedSprites.begin() + static_cast<int>(mSpriteQueueCount),
				[](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
				{
					return x->texture < y->texture;
				});
			break;

		case SpriteSortMode_BackToFront:
			// Sort back to front.
			std::sort(mSortedSprites.begin(),
				mSortedSprites.begin() + static_cast<int>(mSpriteQueueCount),
				[](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
				{
					return x->originRotationDepth.w > y->originRotationDepth.w;
				});
			break;

		case SpriteSortMode_FrontToBack:
			// Sort front to back.
			std::sort(mSortedSprites.begin(),
				mSortedSprites.begin() + static_cast<int>(mSpriteQueueCount),
				[](SpriteInfo const* x, SpriteInfo const* y) noexcept -> bool
				{
					return x->originRotationDepth.w < y->originRotationDepth.w;
				});
			break;

		default:
			break;
		}
	}

	// Generates vertex data for drawing a single sprite.
	static
	void XM_CALLCONV RenderSprite(SpriteInfo const* sprite, VertexPositionColorTexture* vertices, FXMVECTOR textureSize, FXMVECTOR inverseTextureSize) noexcept
	{
		using namespace DirectX;
		// Load sprite parameters into SIMD registers.
		XMVECTOR source = XMLoadFloat4A(&sprite->source);
		const XMVECTOR destination = XMLoadFloat4A(&sprite->destination);
		const XMVECTOR color = XMLoadFloat4A(&sprite->color);
		const XMVECTOR originRotationDepth = XMLoadFloat4A(&sprite->originRotationDepth);

		const float rotation = sprite->originRotationDepth.z;
		const unsigned int flags = sprite->flags;

		// Extract the source and destination sizes into separate vectors.
		XMVECTOR sourceSize = XMVectorSwizzle<2, 3, 2, 3>(source);
		XMVECTOR destinationSize = XMVectorSwizzle<2, 3, 2, 3>(destination);

		// Scale the origin offset by source size, taking care to avoid overflow if the source region is zero.
		const XMVECTOR isZeroMask = XMVectorEqual(sourceSize, XMVectorZero());
		const XMVECTOR nonZeroSourceSize = XMVectorSelect(sourceSize, DirectX::g_XMEpsilon, isZeroMask);

		XMVECTOR origin = XMVectorDivide(originRotationDepth, nonZeroSourceSize);

		// Convert the source region from texels to mod-1 texture coordinate format.
		if (flags & SpriteInfo::SourceInTexels)
		{
			source = XMVectorMultiply(source, inverseTextureSize);
			sourceSize = XMVectorMultiply(sourceSize, inverseTextureSize);
		}
		else
		{
			origin = XMVectorMultiply(origin, inverseTextureSize);
		}

		// If the destination size is relative to the source region, convert it to pixels.
		if (!(flags & SpriteInfo::DestSizeInPixels))
		{
			destinationSize = XMVectorMultiply(destinationSize, textureSize);
		}

		// Compute a 2x2 rotation matrix.
		XMVECTOR rotationMatrix1;
		XMVECTOR rotationMatrix2;

		if (rotation != 0)
		{
			float sin, cos;

			XMScalarSinCos(&sin, &cos, rotation);

			const XMVECTOR sinV = XMLoadFloat(&sin);
			const XMVECTOR cosV = XMLoadFloat(&cos);

			rotationMatrix1 = XMVectorMergeXY(cosV, sinV);
			rotationMatrix2 = XMVectorMergeXY(XMVectorNegate(sinV), cosV);
		}
		else
		{
			using namespace DirectX;
			rotationMatrix1 = g_XMIdentityR0;
			rotationMatrix2 = g_XMIdentityR1;
		}

		// The four corner vertices are computed by transforming these unit-square positions.
		static XMVECTORF32 cornerOffsets[VerticesPerSprite] =
		{
			{ { { 0, 0, 0, 0 } } },
			{ { { 1, 0, 0, 0 } } },
			{ { { 0, 1, 0, 0 } } },
			{ { { 1, 1, 0, 0 } } },
		};

		// Tricksy alert! Texture coordinates are computed from the same cornerOffsets
		// table as vertex positions, but if the sprite is mirrored, this table
		// must be indexed in a different order. This is done as follows:
		//
		//    position = cornerOffsets[i]
		//    texcoord = cornerOffsets[i ^ SpriteEffects]

		static_assert(SpriteEffects_FlipHorizontally == 1 &&
			SpriteEffects_FlipVertically == 2, "If you change these enum values, the mirroring implementation must be updated to match");

		const unsigned int mirrorBits = flags & 3u;

		// Generate the four output vertices.
		for (size_t i = 0; i < VerticesPerSprite; i++)
		{
			// Calculate position.
			const XMVECTOR cornerOffset = XMVectorMultiply(XMVectorSubtract(cornerOffsets[i], origin), destinationSize);

			// Apply 2x2 rotation matrix.
			const XMVECTOR position1 = XMVectorMultiplyAdd(XMVectorSplatX(cornerOffset), rotationMatrix1, destination);
			const XMVECTOR position2 = XMVectorMultiplyAdd(XMVectorSplatY(cornerOffset), rotationMatrix2, position1);

			// Set z = depth.
			const XMVECTOR position = XMVectorPermute<0, 1, 7, 6>(position2, originRotationDepth);

			// Write position as a Float4, even though VertexPositionColor::position is an XMFLOAT3.
			// This is faster, and harmless as we are just clobbering the first element of the
			// following color field, which will immediately be overwritten with its correct value.
			XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&vertices[i].position), position);

			// Write the color.
			XMStoreFloat4(&vertices[i].color, color);

			// Compute and write the texture coordinate.
			const XMVECTOR textureCoordinate = XMVectorMultiplyAdd(cornerOffsets[static_cast<unsigned int>(i) ^ mirrorBits], sourceSize, source);

			XMStoreFloat2(&vertices[i].textureCoordinate, textureCoordinate);
		}
	}

	// Submits a batch of sprites to the GPU.
	void RenderBatch(D3D12_GPU_DESCRIPTOR_HANDLE texture, XMVECTOR textureSize, SpriteInfo const* const* sprites, size_t count)
	{
		auto commandList = mCommandList.Get();

		// Draw using the specified texture.
		// **NOTE** If D3D asserts or crashes here, you probably need to call commandList->SetDescriptorHeaps() with the required descriptor heap(s)
		commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, texture);

		if (mSampler.ptr)
		{
			commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSampler, mSampler);
		}

		// Convert to vector format.
		const XMVECTOR inverseTextureSize = XMVectorReciprocal(textureSize);

		while (count > 0)
		{
			// How many sprites do we want to draw?
			size_t batchSize = count;

			// How many sprites does the D3D vertex buffer have room for?
			const size_t remainingSpace = MaxBatchSize - mSpriteCount;

			if (batchSize > remainingSpace)
			{
				if (remainingSpace < MinBatchSize)
				{
					// If we are out of room, or about to submit an excessively small batch, wrap back to the start of the vertex buffer.
					mSpriteCount = 0;

					batchSize = std::min(count, MaxBatchSize);
				}
				else
				{
					// Take however many sprites fit in what's left of the vertex buffer.
					batchSize = remainingSpace;
				}
			}

			// Allocate a new page of vertex memory if we're starting the batch
			if (mSpriteCount == 0)
			{
				mVertexSegment = GraphicsMemory_t::Get(mDeviceResources->mDevice).Allocate(mVertexPageSize);
			}

			auto vertices = static_cast<VertexPositionColorTexture*>(mVertexSegment.Memory()) + mSpriteCount * VerticesPerSprite;

			// Generate sprite vertex data.
			for (size_t i = 0; i < batchSize; i++)
			{
				assert(i < count);
				_Analysis_assume_(i < count);
				RenderSprite(sprites[i], vertices, textureSize, inverseTextureSize);

				vertices += VerticesPerSprite;
			}

			// Set the vertex buffer view
			D3D12_VERTEX_BUFFER_VIEW vbv;
			constexpr size_t spriteVertexTotalSize = sizeof(VertexPositionColorTexture) * VerticesPerSprite;
			vbv.BufferLocation = mVertexSegment.GpuAddress() + (UINT64(mSpriteCount) * UINT64(spriteVertexTotalSize));
			vbv.StrideInBytes = sizeof(VertexPositionColorTexture);
			vbv.SizeInBytes = static_cast<UINT>(batchSize * spriteVertexTotalSize);
			commandList->IASetVertexBuffers(0, 1, &vbv);

			// Ok lads, the time has come for us draw ourselves some sprites!
			const UINT indexCount = static_cast<UINT>(batchSize * IndicesPerSprite);

			commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

			// Advance the buffer position.
			mSpriteCount += batchSize;

			sprites += batchSize;
			count -= batchSize;
		}
	}

	// Sends queued sprites to the graphics device.
	void FlushBatch()
	{
		if (!mSpriteQueueCount)
			return;

		SortSprites();

		// Walk through the sorted sprite list, looking for adjacent entries that share a texture.
		D3D12_GPU_DESCRIPTOR_HANDLE batchTexture = {};
		XMVECTOR batchTextureSize = {};
		size_t batchStart = 0;

		for (size_t pos = 0; pos < mSpriteQueueCount; pos++)
		{
			const D3D12_GPU_DESCRIPTOR_HANDLE texture = mSortedSprites[pos]->texture;
			assert(texture.ptr != 0);
			const XMVECTOR textureSize = mSortedSprites[pos]->textureSize;

			// Flush whenever the texture changes.
			if (texture != batchTexture)
			{
				if (pos > batchStart)
				{
					RenderBatch(batchTexture, batchTextureSize, &mSortedSprites[batchStart], pos - batchStart);
				}

				batchTexture = texture;
				batchTextureSize = textureSize;
				batchStart = pos;
			}
		}

		// Flush the final batch.
		RenderBatch(batchTexture, batchTextureSize, &mSortedSprites[batchStart], mSpriteQueueCount - batchStart);

		// Reset the queue.
		mSpriteQueueCount = 0;

		// When sorting is disabled, we persist mSortedSprites data from one batch to the next, to avoid
		// uneccessary work in GrowSortedSprites. But we never reuse these when sorting, because re-sorting
		// previously sorted items gives unstable ordering if some sprites have identical sort keys.
		if (mSortMode != SpriteSortMode_Deferred)
		{
			mSortedSprites.clear();
		}
	}

public:
	// Per-SpriteBatch constructor.
	Impl::Impl(
		ID3D12Device* device
		, ResourceUploadBatch_t& upload
		, const SpriteBatchPipelineStateDescription& psoDesc
		, const D3D12_VIEWPORT* viewport
	)
		: mRotation( DXGI_MODE_ROTATION_IDENTITY )
		, mSetViewport(false)
		, mViewPort{ }
		, mSampler{ }
		, mSpriteQueueCount(0)
		, mSpriteQueueArraySize(0)
		, mInBeginEndPair(false)
		, mSortMode(SpriteSortMode_Deferred)
		, mTransformMatrix(MatrixIdentity)
		, mVertexSegment{}
		, mVertexPageSize(sizeof(VertexPositionColorTexture) * MaxBatchSize * VerticesPerSprite)
		, mSpriteCount(0)
		, mDeviceResources( deviceResourcesPool.DemandCreate(device, upload) )
	{
		if (viewport != nullptr)
		{
			mViewPort = *viewport;
			mSetViewport = true;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc = {};
		d3dDesc.InputLayout = s_DefaultInputLayoutDesc;
		d3dDesc.BlendState = psoDesc.blendDesc;
		d3dDesc.DepthStencilState = psoDesc.depthStencilDesc;
		d3dDesc.RasterizerState = psoDesc.rasterizerDesc;
		d3dDesc.DSVFormat = psoDesc.renderTargetState.dsvFormat;
		d3dDesc.NodeMask = psoDesc.renderTargetState.nodeMask;
		d3dDesc.NumRenderTargets = psoDesc.renderTargetState.numRenderTargets;
		memcpy(d3dDesc.RTVFormats, psoDesc.renderTargetState.rtvFormats, sizeof(DXGI_FORMAT) * D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
		d3dDesc.SampleDesc = psoDesc.renderTargetState.sampleDesc;
		d3dDesc.SampleMask = psoDesc.renderTargetState.sampleMask;
		d3dDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		d3dDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Three choices: (1) static sampler, (2) heap sampler, or (3) custom signature & shaders
		if (psoDesc.customRootSignature)
		{
			mRootSignature = psoDesc.customRootSignature;
		}
		else
		{
			mRootSignature = (psoDesc.samplerDescriptor.ptr) ? mDeviceResources->rootSignatureHeap.Get() : mDeviceResources->rootSignatureStatic.Get();
		}
		d3dDesc.pRootSignature = mRootSignature.Get();

		if (psoDesc.customVertexShader.pShaderBytecode)
		{
			d3dDesc.VS = psoDesc.customVertexShader;
		}
		else
		{
			d3dDesc.VS = (psoDesc.samplerDescriptor.ptr) ? s_DefaultVertexShaderByteCodeHeap : s_DefaultVertexShaderByteCodeStatic;
		}

		if (psoDesc.customPixelShader.pShaderBytecode)
		{
			d3dDesc.PS = psoDesc.customPixelShader;
		}
		else
		{
			d3dDesc.PS = (psoDesc.samplerDescriptor.ptr) ? s_DefaultPixelShaderByteCodeHeap : s_DefaultPixelShaderByteCodeStatic;
		}

		if (psoDesc.samplerDescriptor.ptr)
		{
			mSampler = psoDesc.samplerDescriptor;
		}

		Sys::Hr hr;
		hr = device->CreateGraphicsPipelineState(
			&d3dDesc,
			IID_PPV_ARGS(mPSO.GetAddressOf()));

		SetDebugObjectName(mPSO.Get(), L"SpriteBatch");
	}

	// Begins a batch of sprite drawing operations.
	void XM_CALLCONV Begin(
		ID3D12GraphicsCommandList* commandList,
		SpriteSortMode sortMode,
		FXMMATRIX transformMatrix = MatrixIdentity)
	{
		if (mInBeginEndPair)
		{
			DebugTrace("ERROR: Cannot nest Begin calls on a single SpriteBatch\n");
			throw std::logic_error("SpriteBatch::Begin");
		}

		mSortMode = sortMode;
		mTransformMatrix = transformMatrix;
		mCommandList = commandList;
		mSpriteCount = 0;

		if (sortMode == SpriteSortMode_Immediate)
		{
			PrepareForRendering();
		}

		mInBeginEndPair = true;
	}

	// Ends a batch of sprite drawing operations.
	void End()
	{
		if (!mInBeginEndPair)
		{
			DebugTrace("ERROR: Begin must be called before End\n");
			throw std::logic_error("SpriteBatch::End");
		}

		if (mSortMode != SpriteSortMode_Immediate)
		{
			PrepareForRendering();
			FlushBatch();
		}

		// Release this memory
		mVertexSegment.Reset();

		// Break circular reference chains, in case the state lambda closed
		// over an object that holds a reference to this SpriteBatch.
		mCommandList = nullptr;
		mInBeginEndPair = false;
	}

    // Helper converts a RECT to XMVECTOR.
    inline XMVECTOR LoadRect(_In_ RECT const* rect) noexcept
    {
		using namespace DirectX;
        XMVECTOR v = XMLoadInt4(reinterpret_cast<uint32_t const*>(rect));

        v = XMConvertVectorIntToFloat(v, 0);

        // Convert right/bottom to width/height.
        v = XMVectorSubtract(v, XMVectorPermute<0, 1, 4, 5>(g_XMZero, v));

        return v;
    }

	// Dynamically expands the array used to store pending sprite information.
	void GrowSpriteQueue()
	{
		// Grow by a factor of 2.
		const size_t newSize = std::max(InitialQueueSize, mSpriteQueueArraySize * 2);

		// Allocate the new array.
		auto newArray = std::make_unique<SpriteInfo[]>(newSize);

		// Copy over any existing sprites.
		for (size_t i = 0; i < mSpriteQueueCount; i++)
		{
			newArray[i] = mSpriteQueue[i];
		}

		// Replace the previous array with the new one.
		mSpriteQueue = std::move(newArray);
		mSpriteQueueArraySize = newSize;

		// Clear any dangling SpriteInfo pointers left over from previous rendering.
		mSortedSprites.clear();
	}

    //void XM_CALLCONV Draw(
    //    D3D12_GPU_DESCRIPTOR_HANDLE texture,
    //    XMUINT2 const& textureSize,
    //    FXMVECTOR destination,
    //    _In_opt_ RECT const* sourceRectangle,
    //    FXMVECTOR color,
    //    FXMVECTOR originRotationDepth,
    //    unsigned int flags);

	// Adds a single sprite to the queue.
	void XM_CALLCONV SpriteBatch::Impl::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
		XMUINT2 const& textureSize,
		FXMVECTOR destination,
		RECT const* sourceRectangle,
		FXMVECTOR color,
		FXMVECTOR originRotationDepth,
		unsigned int flags)
	{
		using namespace DirectX;
		if (!mInBeginEndPair)
		{
			DebugTrace("ERROR: Begin must be called before Draw\n");
			throw std::logic_error("SpriteBatch::Draw");
		}

		if (!texture.ptr)
			throw std::invalid_argument("Invalid texture for Draw");

		// Get a pointer to the output sprite.
		if (mSpriteQueueCount >= mSpriteQueueArraySize)
		{
			GrowSpriteQueue();
		}

		SpriteInfo* sprite = &mSpriteQueue[mSpriteQueueCount];

		XMVECTOR dest = destination;

		if (sourceRectangle)
		{
			// User specified an explicit source region.
			const XMVECTOR source = LoadRect(sourceRectangle);

			XMStoreFloat4A(&sprite->source, source);

			// If the destination size is relative to the source region, convert it to pixels.
			if (!(flags & SpriteInfo::DestSizeInPixels))
			{
				dest = XMVectorPermute<0, 1, 6, 7>(dest, XMVectorMultiply(dest, source)); // dest.zw *= source.zw
			}

			flags |= SpriteInfo::SourceInTexels | SpriteInfo::DestSizeInPixels;
		}
		else
		{
			// No explicit source region, so use the entire texture.
			static const XMVECTORF32 wholeTexture = { { {0, 0, 1, 1} } };

			XMStoreFloat4A(&sprite->source, wholeTexture);
		}

		// Convert texture size
		const XMVECTOR textureSizeV = XMLoadUInt2(&textureSize);

		// Store sprite parameters.
		XMStoreFloat4A(&sprite->destination, dest);
		XMStoreFloat4A(&sprite->color, color);
		XMStoreFloat4A(&sprite->originRotationDepth, originRotationDepth);

		sprite->texture = texture;
		sprite->textureSize = textureSizeV;
		sprite->flags = flags;

		if (mSortMode == SpriteSortMode_Immediate)
		{
			// If we are in immediate mode, draw this sprite straight away.
			RenderBatch(texture, textureSizeV, &sprite, 1);
		}
		else
		{
			// Queue this sprite for later sorting and batched rendering.
			mSpriteQueueCount++;
		}
		return;
	}

    DXGI_MODE_ROTATION mRotation;

    bool mSetViewport;
    D3D12_VIEWPORT mViewPort;
    D3D12_GPU_DESCRIPTOR_HANDLE mSampler;

private:

	// Generates a viewport transform matrix for rendering sprites using x-right y-down screen pixel coordinates.
	XMMATRIX GetViewportTransform(_In_ DXGI_MODE_ROTATION rotation)
	{
		if (!mSetViewport)
		{
			DebugTrace("ERROR: SpriteBatch requires viewport information via SetViewport\n");
			throw std::runtime_error("Viewport not set.");
		}

		// Compute the matrix.
		const float xScale = (mViewPort.Width > 0) ? 2.0f / mViewPort.Width : 0.0f;
		const float yScale = (mViewPort.Height > 0) ? 2.0f / mViewPort.Height : 0.0f;

		switch (rotation)
		{
		case DXGI_MODE_ROTATION_ROTATE90:
			return XMMATRIX
			(
				0, -yScale, 0, 0,
				-xScale, 0, 0, 0,
				0, 0, 1, 0,
				1, 1, 0, 1
			);

		case DXGI_MODE_ROTATION_ROTATE270:
			return XMMATRIX
			(
				0, yScale, 0, 0,
				xScale, 0, 0, 0,
				0, 0, 1, 0,
				-1, -1, 0, 1
			);

		case DXGI_MODE_ROTATION_ROTATE180:
			return XMMATRIX
			(
				-xScale, 0, 0, 0,
				0, yScale, 0, 0,
				0, 0, 1, 0,
				1, -1, 0, 1
			);

		default:
			return XMMATRIX
			(
				xScale, 0, 0, 0,
				0, -yScale, 0, 0,
				0, 0, 1, 0,
				-1, 1, 0, 1
			);
		}
	}

    // Constants.
    static constexpr size_t MaxBatchSize = 2048;
    static constexpr size_t MinBatchSize = 128;
    static constexpr size_t InitialQueueSize = 64;
    static constexpr size_t VerticesPerSprite = 4;
    static constexpr size_t IndicesPerSprite = 6;

    //
    // The following functions and members are used to create the default pipeline state objects.
    //
    static const D3D12_SHADER_BYTECODE s_DefaultVertexShaderByteCodeStatic;
    static const D3D12_SHADER_BYTECODE s_DefaultPixelShaderByteCodeStatic;
    static const D3D12_SHADER_BYTECODE s_DefaultVertexShaderByteCodeHeap;
    static const D3D12_SHADER_BYTECODE s_DefaultPixelShaderByteCodeHeap;
    static const D3D12_INPUT_LAYOUT_DESC s_DefaultInputLayoutDesc;


    // Queue of sprites waiting to be drawn.
    std::unique_ptr<SpriteInfo[]> mSpriteQueue;

    size_t mSpriteQueueCount;
    size_t mSpriteQueueArraySize;


    // To avoid needlessly copying around bulky SpriteInfo structures, we leave that
    // actual data alone and just sort this array of pointers instead. But we want contiguous
    // memory for cache efficiency, so these pointers are just shortcuts into the single
    // mSpriteQueue array, and we take care to keep them in order when sorting is disabled.
    std::vector<SpriteInfo const*> mSortedSprites;


    // Mode settings from the last Begin call.
    bool mInBeginEndPair;

    SpriteSortMode mSortMode;
    CPtr<ID3D12PipelineState> mPSO;
    CPtr<ID3D12RootSignature> mRootSignature;
    XMMATRIX mTransformMatrix;
    CPtr<ID3D12GraphicsCommandList> mCommandList;

    // Batched data
    GraphicsResource_t mVertexSegment;
    size_t mVertexPageSize;
    size_t mSpriteCount;
    GraphicsResource_t mConstantBuffer;

    enum RootParameterIndex
    {
        TextureSRV,
        ConstantBuffer,
        TextureSampler,
        RootParameterCount
    };


    // Only one of these helpers is allocated per D3D device, even if there are multiple SpriteBatch instances.
    struct DeviceResources
    {
		// Per-device constructor.
		DeviceResources::DeviceResources(ID3D12Device* device, ResourceUploadBatch_t& upload) :
			indexBufferView{},
			mDevice(device)
		{
			CreateIndexBuffer(device, upload);
			CreateRootSignatures(device);
		}

        CPtr<ID3D12Resource> indexBuffer;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        CPtr<ID3D12RootSignature> rootSignatureStatic;
        CPtr<ID3D12RootSignature> rootSignatureHeap;
        ID3D12Device* mDevice;

    private:
		// Creates the SpriteBatch index buffer.
		void DeviceResources::CreateIndexBuffer(_In_ ID3D12Device* device, ResourceUploadBatch_t& upload)
		{
			static_assert((MaxBatchSize * VerticesPerSprite) < USHRT_MAX, "MaxBatchSize too large for 16-bit indices");

			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
			auto const bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(short) * MaxBatchSize * IndicesPerSprite);

			Sys::Hr hr;
			// Create the constant buffer.
			hr = device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				c_initialCopyTargetState,
				nullptr,
				IID_PPV_ARGS(indexBuffer.ReleaseAndGetAddressOf()));

			SetDebugObjectName(indexBuffer.Get(), L"SpriteBatch");

			auto indexValues = CreateIndexValues();

			D3D12_SUBRESOURCE_DATA indexDataDesc = {};
			indexDataDesc.pData = indexValues.data();
			indexDataDesc.RowPitch = static_cast<LONG_PTR>(bufferDesc.Width);
			indexDataDesc.SlicePitch = indexDataDesc.RowPitch;

			// Upload the resource
			upload.Upload(indexBuffer.Get(), 0, &indexDataDesc, 1);
			upload.Transition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
			SetDebugObjectName(indexBuffer.Get(), L"DirectXTK:SpriteBatch Index Buffer");

			// Create the index buffer view
			indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
			indexBufferView.Format = DXGI_FORMAT_R16_UINT;
			indexBufferView.SizeInBytes = static_cast<UINT>(bufferDesc.Width);
		}

		void DeviceResources::CreateRootSignatures(_In_ ID3D12Device* device)
		{
			ENUM_FLAGS_CONSTEXPR D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

			const CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

			{
				// Same as CommonStates::StaticLinearClamp
				const CD3DX12_STATIC_SAMPLER_DESC sampler(
					0, // register
					D3D12_FILTER_MIN_MAG_MIP_LINEAR,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
					D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
					0.f,
					16,
					D3D12_COMPARISON_FUNC_LESS_EQUAL,
					D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
					0.f,
					D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY_PIXEL);

				CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount - 1] = {};
				rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
				rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

				CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
				rsigDesc.Init(static_cast<UINT>(std::size(rootParameters)), rootParameters, 1, &sampler, rootSignatureFlags);

				Sys::Hr hr;
				hr = CreateRootSignature(device, &rsigDesc, rootSignatureStatic.ReleaseAndGetAddressOf());

				SetDebugObjectName(rootSignatureStatic.Get(), L"SpriteBatch");
			}

			{
				const CD3DX12_DESCRIPTOR_RANGE textureSampler(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

				CD3DX12_ROOT_PARAMETER rootParameters[RootParameterIndex::RootParameterCount] = {};
				rootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);
				rootParameters[RootParameterIndex::ConstantBuffer].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
				rootParameters[RootParameterIndex::TextureSampler].InitAsDescriptorTable(1, &textureSampler, D3D12_SHADER_VISIBILITY_PIXEL);

				CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
				rsigDesc.Init(static_cast<UINT>(std::size(rootParameters)), rootParameters, 0, nullptr, rootSignatureFlags);

				Sys::Hr hr;
				hr = CreateRootSignature(device, &rsigDesc, rootSignatureHeap.ReleaseAndGetAddressOf());

				SetDebugObjectName(rootSignatureHeap.Get(), L"SpriteBatch");
			}
		}

        //static std::vector<short> CreateIndexValues();
		// Helper for populating the SpriteBatch index buffer.
		static 
		std::vector<short> DeviceResources::CreateIndexValues()
		{
			std::vector<short> indices;

			indices.reserve(MaxBatchSize * IndicesPerSprite);

			for (size_t j = 0; j < MaxBatchSize * VerticesPerSprite; j += VerticesPerSprite)
			{
				auto const i = static_cast<short>(j);

				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + 2);

				indices.push_back(i + 1);
				indices.push_back(i + 3);
				indices.push_back(i + 2);
			}

			return indices;
		}

    };

    // Per-device data.
    std::shared_ptr<DeviceResources> mDeviceResources;
    static SharedResourcePool<ID3D12Device*, DeviceResources, ResourceUploadBatch_t&> deviceResourcesPool;
};

inline XMMATRIX XM_CALLCONV XMMatrixIdentity()
{
    XMMATRIX M;
    M.r[0] = DirectX::g_XMIdentityR0.v;
    M.r[1] = DirectX::g_XMIdentityR1.v;
    M.r[2] = DirectX::g_XMIdentityR2.v;
    M.r[3] = DirectX::g_XMIdentityR3.v;
    return M;
}

// Global pools of per-device and per-context SpriteBatch resources.
inline SharedResourcePool<ID3D12Device*, SpriteBatch::Impl::DeviceResources, ResourceUploadBatch_t&> SpriteBatch::Impl::deviceResourcesPool;
// Constants.
inline const XMMATRIX SpriteBatch::MatrixIdentity = XMMatrixIdentity();
const XMFLOAT2 SpriteBatch::Float2Zero(0, 0);

inline const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultVertexShaderByteCodeStatic = { SpriteEffect_SpriteVertexShader, sizeof(SpriteEffect_SpriteVertexShader) };
inline const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultPixelShaderByteCodeStatic = { SpriteEffect_SpritePixelShader, sizeof(SpriteEffect_SpritePixelShader) };

inline const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultVertexShaderByteCodeHeap = { SpriteEffect_SpriteVertexShaderHeap, sizeof(SpriteEffect_SpriteVertexShaderHeap) };
inline const D3D12_SHADER_BYTECODE SpriteBatch::Impl::s_DefaultPixelShaderByteCodeHeap = { SpriteEffect_SpritePixelShaderHeap, sizeof(SpriteEffect_SpritePixelShaderHeap) };

inline const D3D12_INPUT_LAYOUT_DESC SpriteBatch::Impl::s_DefaultInputLayoutDesc = VertexPositionColorTexture::InputLayout;



	inline
	SpriteBatch::SpriteBatch(ID3D12Device* device,
		ResourceUploadBatch_t& upload,
		const SpriteBatchPipelineStateDescription& psoDesc,
		const D3D12_VIEWPORT* viewport)
		: pImpl(std::make_unique<Impl>(device, upload, psoDesc, viewport))
	{}

	inline
	void XM_CALLCONV SpriteBatch::Begin(
		ID3D12GraphicsCommandList* commandList,
		SpriteSortMode sortMode,
		FXMMATRIX transformMatrix)
	{
		pImpl->Begin(commandList, sortMode, transformMatrix);
	}

	inline
	void SpriteBatch::End()
	{
		pImpl->End();
	}

	inline
	void SpriteBatch::SetViewport(const D3D12_VIEWPORT& viewPort)
	{
		pImpl->mSetViewport = true;
		pImpl->mViewPort = viewPort;
	}

	inline
	void XM_CALLCONV SpriteBatch::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture,
		XMUINT2 const& textureSize,
		FXMVECTOR position,
		RECT const* sourceRectangle,
		FXMVECTOR color,
		float rotation,
		FXMVECTOR origin,
		GXMVECTOR scale,
		SpriteEffects effects,
		float layerDepth)
	{
		using namespace DirectX;
		const XMVECTOR destination = XMVectorPermute<0, 1, 4, 5>(position, scale); // x, y, scale.x, scale.y

		const XMVECTOR rotationDepth = XMVectorMergeXY(
			XMVectorReplicate(rotation),
			XMVectorReplicate(layerDepth));

		const XMVECTOR originRotationDepth = XMVectorPermute<0, 1, 4, 5>(origin, rotationDepth);

		pImpl->Draw(texture, textureSize, destination, sourceRectangle, color, originRotationDepth, static_cast<unsigned int>(effects));
	}

} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
