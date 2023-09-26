// DrawAux/Fps/Spec/D12/BinaryReader.h - from DirectXTK
#pragma once // NOLINT copyright
namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12 {
// Helper for reading binary data, either from the filesystem a memory buffer.
class BinaryReader {
    // The data currently being read.
    uint8_t const* mPos;
    uint8_t const* mEnd;

    std::unique_ptr<uint8_t[]> mOwnedData;

 public:
	// Constructor reads from an existing memory buffer.
	BinaryReader::BinaryReader(_In_reads_bytes_(dataSize) uint8_t const* dataBlob, size_t dataSize) noexcept :
		mPos(dataBlob),
		mEnd(dataBlob + dataSize)
	 {}

    BinaryReader(BinaryReader const&) = delete;
    BinaryReader& operator= (BinaryReader const&) = delete;

    // Reads a single value.
    template<typename T> T const& Read() {
        return *ReadArray<T>(1);
    }

    // Reads an array of values.
    template<typename T> T const* ReadArray(size_t elementCount){
        static_assert(std::is_standard_layout<T>::value, "Can only read plain-old-data types");

        uint8_t const* newPos = mPos + sizeof(T) * elementCount;

        if (newPos < mPos)
            throw std::overflow_error("ReadArray");

        if (newPos > mEnd)
            throw std::runtime_error("End of file");

        auto result = reinterpret_cast<T const*>(mPos);

        mPos = newPos;

        return result;
    }
};
} // namespace prj_3d::HelloWinHlsl::DrawAux::Fps::Spec::D12
