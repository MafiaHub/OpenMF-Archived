#include <utils/scoped_buffer.hpp>

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

ScopedBuffer::operator char *()
{
    return mData;
}

ScopedBuffer::operator const char *() const
{
    return mData;
}

size_t ScopedBuffer::size() const
{
    return mSize;
}

void ScopedBuffer::copy_from(size_t offset, const void *sptr, size_t ssize)
{
    if (offset + ssize > mSize)
        throw std::out_of_range("ScopedBuffer::copy_from");
    memcpy(mData + offset, sptr, ssize);
}
