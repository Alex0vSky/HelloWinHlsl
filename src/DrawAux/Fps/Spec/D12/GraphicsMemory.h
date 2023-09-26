// DrawAux/Fps/Spec/D12/GraphicsMemory.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {

using ScopedLock = std::lock_guard<std::mutex>;

typedef LinearAllocatorPage LinearAllocatorPage_t;

// Works a little like a smart pointer. The memory will only be fenced by the GPU once the pointer
// has been invalidated or the user explicitly marks it for fencing.
class GraphicsResource
{
public:
    GraphicsResource() noexcept;
    GraphicsResource(
        _In_ LinearAllocatorPage_t* page,
        _In_ D3D12_GPU_VIRTUAL_ADDRESS gpuAddress,
        _In_ ID3D12Resource* resource,
        _In_ void* memory,
        _In_ size_t offset,
        _In_ size_t size) noexcept;

    GraphicsResource(GraphicsResource&& other) noexcept;
    GraphicsResource&& operator= (GraphicsResource&&) noexcept;

    GraphicsResource(const GraphicsResource&) = delete;
    GraphicsResource& operator= (const GraphicsResource&) = delete;

    ~GraphicsResource();

    D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept { return mGpuAddress; }
    ID3D12Resource* Resource() const noexcept { return mResource; }
    void* Memory() const noexcept { return mMemory; }
    size_t ResourceOffset() const noexcept { return mBufferOffset; }
    size_t Size() const noexcept { return mSize; }

    explicit operator bool() const noexcept { return mResource != nullptr; }

    // Clear the pointer. Using operator -> will produce bad results.
    void __cdecl Reset() noexcept;
    void __cdecl Reset(GraphicsResource&&) noexcept;

private:
    LinearAllocatorPage_t*        mPage;
    D3D12_GPU_VIRTUAL_ADDRESS   mGpuAddress;
    ID3D12Resource*             mResource;
    void*                       mMemory;
    size_t                      mBufferOffset;
    size_t                      mSize;
};

class SharedGraphicsResource
{
public:
    SharedGraphicsResource() noexcept;

    SharedGraphicsResource(SharedGraphicsResource&&) noexcept;
    SharedGraphicsResource&& operator= (SharedGraphicsResource&&) noexcept;

    explicit SharedGraphicsResource(GraphicsResource&&);
    SharedGraphicsResource&& operator= (GraphicsResource&&);

    SharedGraphicsResource(const SharedGraphicsResource&) noexcept;
    SharedGraphicsResource& operator= (const SharedGraphicsResource&) noexcept;

    SharedGraphicsResource(const GraphicsResource&) = delete;
    SharedGraphicsResource& operator= (const GraphicsResource&) = delete;

    ~SharedGraphicsResource();

    D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept { return mSharedResource->GpuAddress(); }
    ID3D12Resource* Resource() const noexcept { return mSharedResource->Resource(); }
    void* Memory() const noexcept { return mSharedResource->Memory(); }
    size_t ResourceOffset() const noexcept { return mSharedResource->ResourceOffset(); }
    size_t Size() const noexcept { return mSharedResource->Size(); }

    explicit operator bool() const noexcept { return mSharedResource != nullptr; }

    bool operator == (const SharedGraphicsResource& other) const noexcept { return mSharedResource.get() == other.mSharedResource.get(); }
    bool operator != (const SharedGraphicsResource& other) const noexcept { return mSharedResource.get() != other.mSharedResource.get(); }

    // Clear the pointer. Using operator -> will produce bad results.
    void __cdecl Reset() noexcept;
    void __cdecl Reset(GraphicsResource&&);
    void __cdecl Reset(SharedGraphicsResource&&) noexcept;
    void __cdecl Reset(const SharedGraphicsResource& resource) noexcept;

private:
    std::shared_ptr<GraphicsResource> mSharedResource;
};

//------------------------------------------------------------------------------
struct GraphicsMemoryStatistics
{
    size_t committedMemory;     // Bytes of memory currently committed/in-flight
    size_t totalMemory;         // Total bytes of memory used by the allocators
    size_t totalPages;          // Total page count
    size_t peakCommitedMemory;  // Peak commited memory value since last reset
    size_t peakTotalMemory;     // Peak total bytes
    size_t peakTotalPages;      // Peak total page count
};

//------------------------------------------------------------------------------
class GraphicsMemory
{
public:
    explicit GraphicsMemory(_In_ ID3D12Device* device);

    GraphicsMemory(GraphicsMemory&&) noexcept;
    GraphicsMemory& operator= (GraphicsMemory&&) noexcept;

    GraphicsMemory(GraphicsMemory const&) = delete;
    GraphicsMemory& operator=(GraphicsMemory const&) = delete;

    virtual ~GraphicsMemory();

    // Make sure to keep the GraphicsResource handle alive as long as you need to access
    // the memory on the CPU. For example, do not simply cache GpuAddress() and discard
    // the GraphicsResource object, or your memory may be overwritten later.
    GraphicsResource __cdecl Allocate(size_t size, size_t alignment = 16);

    // Special overload of Allocate that aligns to D3D12 constant buffer alignment requirements
    template<typename T> GraphicsResource AllocateConstant()
    {
        constexpr size_t alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        constexpr size_t alignedSize = (sizeof(T) + alignment - 1) & ~(alignment - 1);
        return Allocate(alignedSize, alignment);
    }
    template<typename T> GraphicsResource AllocateConstant(const T& setData)
    {
        GraphicsResource alloc = AllocateConstant<T>();
        memcpy(alloc.Memory(), &setData, sizeof(T));
        return alloc;
    }

    // Submits all the pending one-shot memory to the GPU.
    // The memory will be recycled once the GPU is done with it.
    void __cdecl Commit(_In_ ID3D12CommandQueue* commandQueue);

    // This frees up any unused memory.
    // If you want to make sure all memory is reclaimed, idle the GPU before calling this.
    // It is not recommended that you call this unless absolutely necessary (e.g. your
    // memory budget changes at run-time, or perhaps you're changing levels in your game.)
    void __cdecl GarbageCollect();

    // Memory statistics
    GraphicsMemoryStatistics __cdecl GetStatistics();
    void __cdecl ResetStatistics();

    // Singleton
    // Should only use nullptr for single GPU scenarios; mGPU requires a specific device
    static GraphicsMemory& __cdecl Get(_In_opt_ ID3D12Device* device = nullptr);

private:
    // Private implementation.
    class Impl;

    std::unique_ptr<Impl> pImpl;
};

typedef LinearAllocator LinearAllocator_t;

namespace
{
    constexpr size_t MinPageSize = 64 * 1024;
    constexpr size_t MinAllocSize = 4 * 1024;
    constexpr size_t AllocatorIndexShift = 12; // start block sizes at 4KB
    constexpr size_t AllocatorPoolCount = 21; // allocation sizes up to 2GB supported
    constexpr size_t PoolIndexScale = 1; // multiply the allocation size this amount to push large values into the next bucket

    static_assert((1 << AllocatorIndexShift) == MinAllocSize, "1 << AllocatorIndexShift must == MinPageSize (in KiB)");
    static_assert((MinPageSize & (MinPageSize - 1)) == 0, "MinPageSize size must be a power of 2");
    static_assert((MinAllocSize & (MinAllocSize - 1)) == 0, "MinAllocSize size must be a power of 2");
    static_assert(MinAllocSize >= (4 * 1024), "MinAllocSize size must be greater than 4K");

    constexpr size_t NextPow2(size_t x) noexcept
    {
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
    #ifdef _WIN64
        x |= x >> 32;
    #endif
        return ++x;
    }

    inline size_t GetPoolIndexFromSize(size_t x) noexcept
    {
        const size_t allocatorPageSize = x >> AllocatorIndexShift;
        // gives a value from range:
        // 0 - sub-4k allocator
        // 1 - 4k allocator
        // 2 - 8k allocator
        // 4 - 16k allocator
        // etc...
        // Need to convert to an index.

    #ifdef _MSC_VER
        unsigned long bitIndex = 0;

    #ifdef _WIN64
        return _BitScanForward64(&bitIndex, allocatorPageSize) ? bitIndex + 1 : 0;
    #else
        return _BitScanForward(&bitIndex, static_cast<unsigned long>(allocatorPageSize)) ? bitIndex + 1 : 0;
    #endif

    #elif defined(__GNUC__)

    #ifdef __LP64__
        return static_cast<size_t>(__builtin_ffsll(static_cast<long long>(allocatorPageSize)));
    #else
        return static_cast<size_t>(__builtin_ffs(static_cast<int>(allocatorPageSize)));
    #endif

    #else
    #error Unknown forward bit-scan syntax
    #endif
    }

    inline size_t GetPageSizeFromPoolIndex(size_t x) noexcept
    {
        x = (x == 0) ? 0 : x - 1; // clamp to zero
        return std::max<size_t>(MinPageSize, size_t(1) << (x + AllocatorIndexShift));
    }

    //--------------------------------------------------------------------------------------
    // DeviceAllocator : honors memory requests associated with a particular device
    //--------------------------------------------------------------------------------------
    class DeviceAllocator
    {
    public:
        explicit DeviceAllocator(_In_ ID3D12Device* device) noexcept(false)
            : mDevice(device)
        {
            if (!device)
                throw std::invalid_argument("Invalid device parameter");

            for (size_t i = 0; i < mPools.size(); ++i)
            {
                size_t pageSize = GetPageSizeFromPoolIndex(i);
                mPools[i] = std::make_unique<LinearAllocator_t>(
                    mDevice.Get(),
                    pageSize);
            }
        }

        DeviceAllocator(DeviceAllocator&&) = delete;
        DeviceAllocator& operator= (DeviceAllocator&&) = delete;

        DeviceAllocator(DeviceAllocator const&) = delete;
        DeviceAllocator& operator= (DeviceAllocator const&) = delete;

        // Explicitly destroy LinearAllocators inside a critical section
        ~DeviceAllocator()
        {
            const ScopedLock lock(mMutex);

            for (auto& allocator : mPools)
            {
                allocator.reset();
            }
        }

        GraphicsResource Alloc(_In_ size_t size, _In_ size_t alignment)
        {
            ScopedLock lock(mMutex);

            // Which memory pool does it live in?
            const size_t poolSize = NextPow2((alignment + size) * PoolIndexScale);
            const size_t poolIndex = GetPoolIndexFromSize(poolSize);
            assert(poolIndex < mPools.size());

            // If the allocator isn't initialized yet, do so now
            auto& allocator = mPools[poolIndex];
            assert(allocator != nullptr);
            assert(poolSize < MinPageSize || poolSize == allocator->PageSize());

            auto page = allocator->FindPageForAlloc(size, alignment);
            if (!page)
            {
                DebugTrace("GraphicsMemory failed to allocate page (%zu requested bytes, %zu alignment)\n", size, alignment);
                throw std::bad_alloc();
            }

            size_t offset = page->Suballocate(size, alignment);

            // Return the information to the user
            return GraphicsResource(
                page,
                page->GpuAddress() + offset,
                page->UploadResource(),
                static_cast<BYTE*>(page->BaseMemory()) + offset,
                offset,
                size);
        }

        // Submit page fences to the command queue
        void KickFences(_In_ ID3D12CommandQueue* commandQueue)
        {
            ScopedLock lock(mMutex);

            for (auto& i : mPools)
            {
                if (i)
                {
                    i->RetirePendingPages();
                    i->FenceCommittedPages(commandQueue);
                }
            }
        }

        void GarbageCollect()
        {
            ScopedLock lock(mMutex);

            for (auto& i : mPools)
            {
                if (i)
                {
                    i->Shrink();
                }
            }
        }

        void GetStatistics(GraphicsMemoryStatistics& stats) const
        {
            size_t totalPageCount = 0;
            size_t committedMemoryUsage = 0;
            size_t totalMemoryUsage = 0;

            ScopedLock lock(mMutex);

            for (const auto& i : mPools)
            {
                if (i)
                {
                    totalPageCount += i->TotalPageCount();
                    committedMemoryUsage += i->CommittedMemoryUsage();
                    totalMemoryUsage += i->TotalMemoryUsage();
                }
            }

            stats = {};
            stats.committedMemory = committedMemoryUsage;
            stats.totalMemory = totalMemoryUsage;
            stats.totalPages = totalPageCount;
        }

    #if !(defined(_XBOX_ONE) && defined(_TITLE)) && !defined(_GAMING_XBOX)
        ID3D12Device* GetDevice() const noexcept { return mDevice.Get(); }
    #endif

    private:
        CPtr< ID3D12Device > mDevice;
        std::array<std::unique_ptr<LinearAllocator_t>, AllocatorPoolCount> mPools;
        mutable std::mutex mMutex;
    };
} // anonymous namespace

//--------------------------------------------------------------------------------------
// GraphicsMemory::Impl
//--------------------------------------------------------------------------------------

class GraphicsMemory::Impl
{
public:
    explicit Impl(GraphicsMemory* owner) noexcept(false)
        : mOwner(owner)
        , m_peakCommited(0)
        , m_peakBytes(0)
        , m_peakPages(0)
    {
    #if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        if (s_graphicsMemory)
        {
            throw std::logic_error("GraphicsMemory is a singleton");
        }

        s_graphicsMemory = this;
    #endif
    }

    Impl(Impl&&) = default;
    Impl& operator= (Impl&&) = default;

    Impl(Impl const&) = delete;
    Impl& operator= (Impl const&) = delete;

    ~Impl()
    {
    #if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        s_graphicsMemory = nullptr;
    #else
        if (mDeviceAllocator && mDeviceAllocator->GetDevice())
        {
            s_graphicsMemory.erase(mDeviceAllocator->GetDevice());
        }
    #endif
        mDeviceAllocator.reset();
    }

    void Initialize(_In_ ID3D12Device* device)
    {
        mDeviceAllocator = std::make_unique<DeviceAllocator>(device);

    #if !(defined(_XBOX_ONE) && defined(_TITLE)) && !defined(_GAMING_XBOX)
        if (s_graphicsMemory.find(device) != s_graphicsMemory.cend())
        {
            throw std::logic_error("GraphicsMemory is a per-device singleton");
        }
        s_graphicsMemory[device] = this;
    #endif
    }

    GraphicsResource Allocate(size_t size, size_t alignment)
    {
        return mDeviceAllocator->Alloc(size, alignment);
    }

    void Commit(_In_ ID3D12CommandQueue* commandQueue)
    {
        mDeviceAllocator->KickFences(commandQueue);
    }

    void GarbageCollect()
    {
        mDeviceAllocator->GarbageCollect();
    }

    void GetStatistics(GraphicsMemoryStatistics& stats)
    {
        mDeviceAllocator->GetStatistics(stats);

        if (stats.committedMemory > m_peakCommited)
        {
            m_peakCommited = stats.committedMemory;
        }
        stats.peakCommitedMemory = m_peakCommited;

        if (stats.totalMemory > m_peakBytes)
        {
            m_peakBytes = stats.totalMemory;
        }
        stats.peakTotalMemory = m_peakBytes;

        if (stats.totalPages > m_peakPages)
        {
            m_peakPages = stats.totalPages;
        }
        stats.peakTotalPages = m_peakPages;
    }

    void ResetStatistics()
    {
        m_peakCommited = 0;
        m_peakBytes = 0;
        m_peakPages = 0;
    }

    GraphicsMemory* mOwner;
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
    static GraphicsMemory::Impl* s_graphicsMemory;
#else
    static std::map<ID3D12Device*, GraphicsMemory::Impl*> s_graphicsMemory;
#endif

private:
    std::unique_ptr<DeviceAllocator> mDeviceAllocator;

    size_t  m_peakCommited;
    size_t  m_peakBytes;
    size_t  m_peakPages;
};

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
inline
GraphicsMemory::Impl* GraphicsMemory::Impl::s_graphicsMemory = nullptr;
#else
inline
std::map<ID3D12Device*, GraphicsMemory::Impl*> GraphicsMemory::Impl::s_graphicsMemory;
#endif

//--------------------------------------------------------------------------------------
// GraphicsMemory
//--------------------------------------------------------------------------------------

// Public constructor.
inline 
GraphicsMemory::GraphicsMemory(_In_ ID3D12Device* device)
    : pImpl(std::make_unique<Impl>(this))
{
    pImpl->Initialize(device);
}


// Move constructor.
inline 
GraphicsMemory::GraphicsMemory(GraphicsMemory&& moveFrom) noexcept
    : pImpl(std::move(moveFrom.pImpl))
{
    pImpl->mOwner = this;
}


// Move assignment.
inline 
GraphicsMemory& GraphicsMemory::operator= (GraphicsMemory&& moveFrom) noexcept
{
    pImpl = std::move(moveFrom.pImpl);
    pImpl->mOwner = this;
    return *this;
}


// Public destructor.
inline 
GraphicsMemory::~GraphicsMemory() = default;


inline 
GraphicsResource GraphicsMemory::Allocate(size_t size, size_t alignment)
{
    assert(alignment >= 4); // Should use at least DWORD alignment
    return pImpl->Allocate(size, alignment);
}


inline 
void GraphicsMemory::Commit(_In_ ID3D12CommandQueue* commandQueue)
{
    pImpl->Commit(commandQueue);
}


inline 
void GraphicsMemory::GarbageCollect()
{
    pImpl->GarbageCollect();
}

inline 
GraphicsMemoryStatistics GraphicsMemory::GetStatistics()
{
    GraphicsMemoryStatistics stats;
    pImpl->GetStatistics(stats);
    return stats;
}

inline 
void GraphicsMemory::ResetStatistics()
{
    pImpl->ResetStatistics();
}

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
inline 
GraphicsMemory& GraphicsMemory::Get(_In_opt_ ID3D12Device*)
{
    if (!Impl::s_graphicsMemory || !Impl::s_graphicsMemory->mOwner)
        throw std::logic_error("GraphicsMemory singleton not created");

    return *Impl::s_graphicsMemory->mOwner;
}
#else
inline 
GraphicsMemory& GraphicsMemory::Get(_In_opt_ ID3D12Device* device)
{
    if (Impl::s_graphicsMemory.empty())
        throw std::logic_error("GraphicsMemory singleton not created");

    std::map<ID3D12Device*, GraphicsMemory::Impl*>::const_iterator it;
    if (!device)
    {
        // Should only use nullptr for device for single GPU usage
        assert(Impl::s_graphicsMemory.size() == 1);

        it = Impl::s_graphicsMemory.cbegin();
    }
    else
    {
        it = Impl::s_graphicsMemory.find(device);
    }

    if (it == Impl::s_graphicsMemory.cend() || !it->second->mOwner)
        throw std::logic_error("GraphicsMemory per-device singleton not created");

    return *it->second->mOwner;
}
#endif



//--------------------------------------------------------------------------------------
// GraphicsResource smart-pointer interface
//--------------------------------------------------------------------------------------

inline 
GraphicsResource::GraphicsResource() noexcept
    : mPage(nullptr)
    , mGpuAddress{}
    , mResource(nullptr)
    , mMemory(nullptr)
    , mBufferOffset(0)
    , mSize(0)
{
}

inline 
GraphicsResource::GraphicsResource(
    _In_ LinearAllocatorPage_t* page,
    _In_ D3D12_GPU_VIRTUAL_ADDRESS gpuAddress,
    _In_ ID3D12Resource* resource,
    _In_ void* memory,
    _In_ size_t offset,
    _In_ size_t size) noexcept
    : mPage(page)
    , mGpuAddress(gpuAddress)
    , mResource(resource)
    , mMemory(memory)
    , mBufferOffset(offset)
    , mSize(size)
{
    assert(mPage != nullptr);
    mPage->AddRef();
}

inline 
GraphicsResource::GraphicsResource(GraphicsResource&& other) noexcept
    : mPage(nullptr)
    , mGpuAddress{}
    , mResource(nullptr)
    , mMemory(nullptr)
    , mBufferOffset(0)
    , mSize(0)
{
    Reset(std::move(other));
}

inline 
GraphicsResource::~GraphicsResource()
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }
}

inline 
GraphicsResource&& GraphicsResource::operator= (GraphicsResource&& other) noexcept
{
    Reset(std::move(other));
    return std::move(*this);
}

inline 
void GraphicsResource::Reset() noexcept
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }

    mGpuAddress = {};
    mResource = nullptr;
    mMemory = nullptr;
    mBufferOffset = 0;
    mSize = 0;
}

inline 
void GraphicsResource::Reset(GraphicsResource&& alloc) noexcept
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }

    mGpuAddress = alloc.GpuAddress();
    mResource = alloc.Resource();
    mMemory = alloc.Memory();
    mBufferOffset = alloc.ResourceOffset();
    mSize = alloc.Size();
    mPage = alloc.mPage;

    alloc.mGpuAddress = {};
    alloc.mResource = nullptr;
    alloc.mMemory = nullptr;
    alloc.mBufferOffset = 0;
    alloc.mSize = 0;
    alloc.mPage = nullptr;
}



//--------------------------------------------------------------------------------------
// SharedGraphicsResource
//--------------------------------------------------------------------------------------

inline 
SharedGraphicsResource::SharedGraphicsResource() noexcept
    : mSharedResource(nullptr)
{
}

inline 
SharedGraphicsResource::SharedGraphicsResource(GraphicsResource&& resource) noexcept(false)
    : mSharedResource(std::make_shared<GraphicsResource>(std::move(resource)))
{
}

inline 
SharedGraphicsResource::SharedGraphicsResource(SharedGraphicsResource&& resource) noexcept
    : mSharedResource(std::move(resource.mSharedResource))
{
}

inline 
SharedGraphicsResource::SharedGraphicsResource(const SharedGraphicsResource& resource) noexcept
    : mSharedResource(resource.mSharedResource)
{
}

inline 
SharedGraphicsResource::~SharedGraphicsResource()
{
}

inline 
SharedGraphicsResource&& SharedGraphicsResource::operator= (SharedGraphicsResource&& resource) noexcept
{
    mSharedResource = std::move(resource.mSharedResource);
    return std::move(*this);
}

inline 
SharedGraphicsResource&& SharedGraphicsResource::operator= (GraphicsResource&& resource)
{
    mSharedResource = std::make_shared<GraphicsResource>(std::move(resource));
    return std::move(*this);
}

inline 
SharedGraphicsResource& SharedGraphicsResource::operator= (const SharedGraphicsResource& resource) noexcept
{
    mSharedResource = resource.mSharedResource;
    return *this;
}

inline 
void SharedGraphicsResource::Reset() noexcept
{
    mSharedResource.reset();
}

inline 
void SharedGraphicsResource::Reset(GraphicsResource&& resource)
{
    mSharedResource = std::make_shared<GraphicsResource>(std::move(resource));
}

inline 
void SharedGraphicsResource::Reset(SharedGraphicsResource&& resource) noexcept
{
    mSharedResource = std::move(resource.mSharedResource);
}

inline 
void SharedGraphicsResource::Reset(const SharedGraphicsResource& resource) noexcept
{
    mSharedResource = resource.mSharedResource;
}

} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
