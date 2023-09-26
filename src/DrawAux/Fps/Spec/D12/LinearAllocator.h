// DrawAux/Fps/Spec/D12/LinearAllocator.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {
class LinearAllocatorPage
{
public:
    LinearAllocatorPage() noexcept;

    LinearAllocatorPage(LinearAllocatorPage&&) = delete;
    LinearAllocatorPage& operator= (LinearAllocatorPage&&) = delete;

    LinearAllocatorPage(LinearAllocatorPage const&) = delete;
    LinearAllocatorPage& operator=(LinearAllocatorPage const&) = delete;

    size_t Suballocate(_In_ size_t size, _In_ size_t alignment);

    void* BaseMemory() const noexcept { return mMemory; }
    ID3D12Resource* UploadResource() const noexcept { return mUploadResource.Get(); }
    D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept { return mGpuAddress; }
    size_t BytesUsed() const noexcept { return mOffset; }
    size_t Size() const noexcept { return mSize; }

    void AddRef() noexcept { mRefCount.fetch_add(1); }
    int32_t RefCount() const noexcept { return mRefCount.load(); }
    void Release() noexcept;

protected:
    friend class LinearAllocator;

    LinearAllocatorPage*                    pPrevPage;
    LinearAllocatorPage*                    pNextPage;

    void*                                   mMemory;
    uint64_t                                mPendingFence;
    D3D12_GPU_VIRTUAL_ADDRESS               mGpuAddress;
    size_t                                  mOffset;
    size_t                                  mSize;
    Microsoft::WRL::ComPtr<ID3D12Resource>  mUploadResource;

private:
    std::atomic<int32_t>                    mRefCount;
};

class LinearAllocator
{
public:
    // These values will be rounded up to the nearest 64k.
    // You can specify zero for incrementalSizeBytes to increment
    // by 1 page (64k).
    LinearAllocator(
        _In_ ID3D12Device* pDevice,
        _In_ size_t pageSize,
        _In_ size_t preallocateBytes = 0) noexcept(false);

    LinearAllocator(LinearAllocator&&) = default;
    LinearAllocator& operator= (LinearAllocator&&) = default;

    LinearAllocator(LinearAllocator const&) = delete;
    LinearAllocator& operator=(LinearAllocator const&) = delete;

    ~LinearAllocator();

    LinearAllocatorPage* FindPageForAlloc(_In_ size_t requestedSize, _In_ size_t alignment);

    // Call this at least once a frame to check if pages have become available.
    void RetirePendingPages() noexcept;

    // Call this after you submit your work to the driver.
    // (e.g. immediately before Present.)
    void FenceCommittedPages(_In_ ID3D12CommandQueue* commandQueue);

    // Throws away all currently unused pages
    void Shrink() noexcept;

    // Statistics
    size_t CommittedPageCount() const noexcept { return m_numPending; }
    size_t TotalPageCount() const noexcept { return m_totalPages; }
    size_t CommittedMemoryUsage() const noexcept { return m_numPending * m_increment; }
    size_t TotalMemoryUsage() const noexcept { return m_totalPages * m_increment; }
    size_t PageSize() const noexcept { return m_increment; }

#if defined(_DEBUG) || defined(PROFILE)
        // Debug info
    const wchar_t* GetDebugName() const noexcept { return m_debugName.c_str(); }
    void SetDebugName(const wchar_t* name);
    void SetDebugName(const char* name);
#endif

private:
    LinearAllocatorPage*                    m_pendingPages; // Pages in use by the GPU
    LinearAllocatorPage*                    m_usedPages;    // Pages to be submitted to the GPU
    LinearAllocatorPage*                    m_unusedPages;  // Pages not being used right now
    size_t                                  m_increment;
    size_t                                  m_numPending;
    size_t                                  m_totalPages;
    uint64_t                                m_fenceCount;
    Microsoft::WRL::ComPtr<ID3D12Device>    m_device;
    Microsoft::WRL::ComPtr<ID3D12Fence>     m_fence;

    LinearAllocatorPage* GetPageForAlloc(size_t sizeBytes, size_t alignment);
    LinearAllocatorPage* GetCleanPageForAlloc();

    LinearAllocatorPage* FindPageForAlloc(LinearAllocatorPage* list, size_t sizeBytes, size_t alignment) noexcept;

    LinearAllocatorPage* GetNewPage();

    void UnlinkPage(LinearAllocatorPage* page) noexcept;
    void LinkPage(LinearAllocatorPage* page, LinearAllocatorPage*& list) noexcept;
    void LinkPageChain(LinearAllocatorPage* page, LinearAllocatorPage*& list) noexcept;
    void ReleasePage(LinearAllocatorPage* page) noexcept;
    void FreePages(LinearAllocatorPage* list) noexcept;

#if defined(_DEBUG) || defined(PROFILE)
    std::wstring m_debugName;

    static void ValidateList(LinearAllocatorPage* list);
    void ValidatePageLists();

    void SetPageDebugName(LinearAllocatorPage* list) noexcept;
#endif
};

// Set this to 1 to enable some additional debug validation
#define VALIDATE_LISTS 0

#if VALIDATE_LISTS
#   include <unordered_set>
#endif

inline 
LinearAllocatorPage::LinearAllocatorPage() noexcept
    : pPrevPage(nullptr)
    , pNextPage(nullptr)
    , mMemory(nullptr)
    , mPendingFence(0)
    , mGpuAddress{}
    , mOffset(0)
    , mSize(0)
    , mRefCount(1)
{}

inline 
size_t LinearAllocatorPage::Suballocate(_In_ size_t size, _In_ size_t alignment)
{
    const size_t offset = AlignUp(mOffset, alignment);
    if (offset + size > mSize)
    {
        // Use of suballocate should be limited to pages with free space,
        // so really shouldn't happen.
        throw std::runtime_error("LinearAllocatorPage::Suballocate");
    }
    mOffset = offset + size;
    return offset;
}

inline 
void LinearAllocatorPage::Release() noexcept
{
    assert(mRefCount > 0);

    if (mRefCount.fetch_sub(1) == 1)
    {
        mUploadResource->Unmap(0, nullptr);
        delete this;
    }
}

//--------------------------------------------------------------------------------------
inline 
LinearAllocator::LinearAllocator(
    _In_ ID3D12Device* pDevice,
    _In_ size_t pageSize,
    _In_ size_t preallocateBytes) noexcept(false)
    : m_pendingPages(nullptr)
    , m_usedPages(nullptr)
    , m_unusedPages(nullptr)
    , m_increment(pageSize)
    , m_numPending(0)
    , m_totalPages(0)
    , m_fenceCount(0)
    , m_device(pDevice)
{
    assert(pDevice != nullptr);
#if defined(_DEBUG) || defined(PROFILE)
    m_debugName = L"LinearAllocator";
#endif

    const size_t preallocatePageCount = ((preallocateBytes + pageSize - 1) / pageSize);
    for (size_t preallocatePages = 0; preallocateBytes != 0 && preallocatePages < preallocatePageCount; ++preallocatePages)
    {
        if (GetNewPage() == nullptr)
        {
            DebugTrace("LinearAllocator failed to preallocate pages (%zu required bytes, %zu pages)\n",
                preallocatePageCount * m_increment, preallocatePageCount);
            throw std::bad_alloc();
        }
    }

    ThrowIfFailed(pDevice->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_GRAPHICS_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
}

inline 
LinearAllocator::~LinearAllocator()
{
    // Must wait for all pending fences!
    while (m_pendingPages != nullptr)
    {
        RetirePendingPages();
    }

    assert(m_pendingPages == nullptr);

    // Return all the memory
    FreePages(m_unusedPages);
    FreePages(m_usedPages);

    m_pendingPages = nullptr;
    m_usedPages = nullptr;
    m_unusedPages = nullptr;
    m_increment = 0;
}

inline 
LinearAllocatorPage* LinearAllocator::FindPageForAlloc(_In_ size_t size, _In_ size_t alignment)
{
#ifdef _DEBUG
    if (size > m_increment)
        throw std::out_of_range("Size must be less or equal to the allocator's increment");
    if (alignment > m_increment)
        throw std::out_of_range("Alignment must be less or equal to the allocator's increment");
    if (size == 0)
        throw std::invalid_argument("Cannot honor zero size allocation request.");
#endif

    auto page = GetPageForAlloc(size, alignment);
    if (!page)
    {
        return nullptr;
    }

    return page;
}

// Call this after you submit your work to the driver.
inline 
void LinearAllocator::FenceCommittedPages(_In_ ID3D12CommandQueue* commandQueue)
{
    // No pending pages
    if (m_usedPages == nullptr)
        return;

    // For all the used pages, fence them
    UINT numReady = 0;
    LinearAllocatorPage* readyPages = nullptr;
    LinearAllocatorPage* unreadyPages = nullptr;
    LinearAllocatorPage* nextPage = nullptr;
    for (auto page = m_usedPages; page != nullptr; page = nextPage)
    {
        nextPage = page->pNextPage;

        // Disconnect from the list
        page->pPrevPage = nullptr;

        // This implies the allocator is the only remaining reference to the page, and therefore the memory is ready for re-use.
        if (page->RefCount() == 1)
        {
            // Signal the fence
            numReady++;
            page->mPendingFence = ++m_fenceCount;
            ThrowIfFailed(commandQueue->Signal(m_fence.Get(), m_fenceCount));

            // Link to the ready pages list
            page->pNextPage = readyPages;
            if (readyPages) readyPages->pPrevPage = page;
            readyPages = page;
        }
        else
        {
            // Link to the unready list
            page->pNextPage = unreadyPages;
            if (unreadyPages) unreadyPages->pPrevPage = page;
            unreadyPages = page;
        }
    }

    // Replace the used pages list with the new unready list
    m_usedPages = unreadyPages;

    // Append all those pages from the ready list to the pending list
    if (numReady > 0)
    {
        m_numPending += numReady;
        LinkPageChain(readyPages, m_pendingPages);
    }

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

// Call this once a frame after all of your driver submissions.
// (immediately before or after Present-time)
inline 
void LinearAllocator::RetirePendingPages() noexcept
{
    const uint64_t fenceValue = m_fence->GetCompletedValue();

    // For each page that we know has a fence pending, check it. If the fence has passed,
    // we can mark the page for re-use.
    auto page = m_pendingPages;
    while (page != nullptr)
    {
        auto nextPage = page->pNextPage;

        assert(page->mPendingFence != 0);

        if (fenceValue >= page->mPendingFence)
        {
            // Fence has passed. It is safe to use this page again.
            ReleasePage(page);
        }

        page = nextPage;
    }
}

inline 
void LinearAllocator::Shrink() noexcept
{
    FreePages(m_unusedPages);
    m_unusedPages = nullptr;

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

inline 
LinearAllocatorPage* LinearAllocator::GetCleanPageForAlloc()
{
    // Grab the first unused page, if one exists. Else, allocate a new page.
    auto page = m_unusedPages;
    if (!page)
    {
        // Allocate a new page
        page = GetNewPage();
        if (!page)
        {
            return nullptr;
        }
    }

    // Mark this page as used
    UnlinkPage(page);
    LinkPage(page, m_usedPages);

    assert(page->mOffset == 0);

    return page;
}

inline 
LinearAllocatorPage* LinearAllocator::GetPageForAlloc(
    size_t sizeBytes,
    size_t alignment)
{
    // Fast path
    if (sizeBytes == m_increment && (alignment == 0 || alignment == m_increment))
    {
        return GetCleanPageForAlloc();
    }

    // Find a page in the pending pages list that has space.
    auto page = FindPageForAlloc(m_usedPages, sizeBytes, alignment);
    if (!page)
    {
        page = GetCleanPageForAlloc();
    }

    return page;
}

inline 
LinearAllocatorPage* LinearAllocator::FindPageForAlloc(
    LinearAllocatorPage* list,
    size_t sizeBytes,
    size_t alignment) noexcept
{
    for (auto page = list; page != nullptr; page = page->pNextPage)
    {
        const size_t offset = AlignUp(page->mOffset, alignment);
        if (offset + sizeBytes <= m_increment)
            return page;
    }
    return nullptr;
}

inline 
LinearAllocatorPage* LinearAllocator::GetNewPage()
{
    const CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_increment);

    // Allocate the upload heap
    CPtr<ID3D12Resource> spResource;
    HRESULT hr = m_device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_GRAPHICS_PPV_ARGS(spResource.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        if (hr != E_OUTOFMEMORY)
        {
            DebugTrace("LinearAllocator::GetNewPage resource allocation failed due to unexpected error %08X\n", static_cast<unsigned int>(hr));
        }
        return nullptr;
    }

#if defined(_DEBUG) || defined(PROFILE)
    spResource->SetName(m_debugName.empty() ? L"LinearAllocator" : m_debugName.c_str());
#endif

    // Get a pointer to the memory
    void* pMemory = nullptr;
    ThrowIfFailed(spResource->Map(0, nullptr, &pMemory));
    memset(pMemory, 0, m_increment);

    // Add the page to the page list
    auto page = new LinearAllocatorPage;
    page->mMemory = pMemory;
    page->mGpuAddress = spResource->GetGPUVirtualAddress();
    page->mSize = m_increment;
    page->mUploadResource.Swap(spResource);

    // Set as head of the list
    page->pNextPage = m_unusedPages;
    if (m_unusedPages) m_unusedPages->pPrevPage = page;
    m_unusedPages = page;
    m_totalPages++;

#if VALIDATE_LISTS
    ValidatePageLists();
#endif

    return page;
}

inline 
void LinearAllocator::UnlinkPage(LinearAllocatorPage* page) noexcept
{
    if (page->pPrevPage)
        page->pPrevPage->pNextPage = page->pNextPage;

    // Check that it isn't the head of any of our tracked lists
    else if (page == m_unusedPages)
        m_unusedPages = page->pNextPage;
    else if (page == m_usedPages)
        m_usedPages = page->pNextPage;
    else if (page == m_pendingPages)
        m_pendingPages = page->pNextPage;

    if (page->pNextPage)
        page->pNextPage->pPrevPage = page->pPrevPage;

    page->pNextPage = nullptr;
    page->pPrevPage = nullptr;

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

inline 
void LinearAllocator::LinkPageChain(LinearAllocatorPage* page, LinearAllocatorPage*& list) noexcept
{
#if VALIDATE_LISTS
    // Walk the chain and ensure it's not in the list twice
    for (LinearAllocatorPage* cur = list; cur != nullptr; cur = cur->pNextPage)
    {
        assert(cur != page);
    }
#endif
    assert(page->pPrevPage == nullptr);
    assert(list == nullptr || list->pPrevPage == nullptr);

    // Follow chain to the end and append
    LinearAllocatorPage* lastPage = nullptr;
    for (lastPage = page; lastPage->pNextPage != nullptr; lastPage = lastPage->pNextPage) {}

    lastPage->pNextPage = list;
    if (list)
        list->pPrevPage = lastPage;

    list = page;

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

inline 
void LinearAllocator::LinkPage(LinearAllocatorPage* page, LinearAllocatorPage*& list) noexcept
{
#if VALIDATE_LISTS
    // Walk the chain and ensure it's not in the list twice
    for (LinearAllocatorPage* cur = list; cur != nullptr; cur = cur->pNextPage)
    {
        assert(cur != page);
    }
#endif
    assert(page->pNextPage == nullptr);
    assert(page->pPrevPage == nullptr);
    assert(list == nullptr || list->pPrevPage == nullptr);

    page->pNextPage = list;
    if (list)
        list->pPrevPage = page;

    list = page;

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

inline 
void LinearAllocator::ReleasePage(LinearAllocatorPage* page) noexcept
{
    assert(m_numPending > 0);
    m_numPending--;

    UnlinkPage(page);
    LinkPage(page, m_unusedPages);

    // Reset the page offset (effectively erasing the memory)
    page->mOffset = 0;

#ifdef _DEBUG
    memset(page->mMemory, 0, m_increment);
#endif

#if VALIDATE_LISTS
    ValidatePageLists();
#endif
}

inline 
void LinearAllocator::FreePages(LinearAllocatorPage* page) noexcept
{
    while (page != nullptr)
    {
        auto nextPage = page->pNextPage;

        page->Release();

        page = nextPage;
        assert(m_totalPages > 0);
        m_totalPages--;
    }
}

#if VALIDATE_LISTS
inline 
void LinearAllocator::ValidateList(LinearAllocatorPage* list)
{
    for (auto page = list, *lastPage = nullptr;
        page != nullptr;
        lastPage = page, page = page->pNextPage)
    {
        if (page->pPrevPage != lastPage)
        {
            throw std::runtime_error("Broken link to previous");
        }
    }
}

void LinearAllocator::ValidatePageLists()
{
    ValidateList(m_pendingPages);
    ValidateList(m_usedPages);
    ValidateList(m_unusedPages);
}
#endif

#if defined(_DEBUG) || defined(PROFILE)
inline 
void LinearAllocator::SetDebugName(const char* name)
{
    wchar_t wname[MAX_PATH] = {};
    const int result = MultiByteToWideChar(CP_UTF8, 0, name, static_cast<int>(strlen(name)), wname, MAX_PATH);
    if (result > 0)
    {
        SetDebugName(wname);
    }
}

inline 
void LinearAllocator::SetDebugName(const wchar_t* name)
{
    m_debugName = name;

    // Rename existing pages
    m_fence->SetName(name);
    SetPageDebugName(m_pendingPages);
    SetPageDebugName(m_usedPages);
    SetPageDebugName(m_unusedPages);
}

inline 
void LinearAllocator::SetPageDebugName(LinearAllocatorPage* list) noexcept
{
    for (auto page = list; page != nullptr; page = page->pNextPage)
    {
        page->mUploadResource->SetName(m_debugName.c_str());
    }
}
#endif

} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
