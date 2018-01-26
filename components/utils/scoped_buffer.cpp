#include <scoped_buffer.hpp>

ScopedBuffer::ScopedBuffer(size_t size) : mData(0x0), mSize(size)
{
    mData = new char[mSize];
}

ScopedBuffer::ScopedBuffer(ScopedBuffer &&src) : mData(src.mData), mSize(src.mSize)
{
    src.mData = nullptr;
    src.mSize = 0;
}

ScopedBuffer::~ScopedBuffer()
{
    delete[] mData;
    mData = nullptr;
    mSize = 0;
}

char *ScopedBuffer::operator*()
{
    return mData;
}

const char *ScopedBuffer::operator*() const
{
    return mData;
}

char &ScopedBuffer::operator[](size_t idx)
{
    return mData[idx];
}

const char &ScopedBuffer::operator[](size_t idx) const
{
    return mData[idx];
}

operator char *ScopedBuffer::()
{
    return mData;
}

operator const char *ScopedBuffer::() const
{
    return mData;
}

size_t ScopedBuffer::size() const
{
    return mSize;
}

template<typename TypeT>
TypeT *ScopedBuffer::as()
{
    return reinterpret_cast<TypeT*>(mData);
}

template<typename TypeT>
const TypeT *ScopedBuffer::as() const
{
    return reinterpret_cast<TypeT*>(mData);
}

void ScopedBuffer::copy_from(size_t offset, const void *sptr, size_t ssize)
{
    if (offset + ssize > mSize)
        throw std::out_of_range("ScopedBuffer::copy_from");
    memcpy(mData + offset, sptr, ssize);
}
