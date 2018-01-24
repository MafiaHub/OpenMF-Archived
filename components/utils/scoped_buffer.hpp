#ifndef UTILS_BUFFER_H
#define UTILS_BUFFER_H

#include <assert.h>
#include <cstddef> // for size_t
#include <string.h>
#include <stdexcept>


/// Non-Copyable (move passable) utilitary buffer with RAII scoping
class ScopedBuffer {
public:
    ScopedBuffer(size_t size) : mData(0x0), mSize(size)
    {
        mData = new char[mSize];
    }

    ScopedBuffer(ScopedBuffer &&src) : mData(src.mData), mSize(src.mSize)
    {
        src.mData = nullptr;
        src.mSize = 0;
    }

    ~ScopedBuffer()
    {
        delete[] mData;
        mData = nullptr;
        mSize = 0;
    }

    char *operator*()
    {
        return mData;
    }

    const char *operator*() const
    {
        return mData;
    }

    char &operator[](size_t idx)
    {
        return mData[idx];
    }

    const char &operator[](size_t idx) const
    {
        return mData[idx];
    }

    operator char *()
    {
        return mData;
    }

    operator const char *() const
    {
        return mData;
    }

    size_t size() const
    {
        return mSize;
    }

    template<typename TypeT>
    TypeT *as()
    {
        return reinterpret_cast<TypeT*>(mData);
    }

    template<typename TypeT>
    const TypeT *as() const
    {
        return reinterpret_cast<TypeT*>(mData);
    }

    // copies specified data to this instance, starting write at given offset
    void copy_from(size_t offset, const void *sptr, size_t ssize) {
        if (offset + ssize > mSize)
            throw std::out_of_range("ScopedBuffer::copy_from");
        memcpy(mData + offset, sptr, ssize);
    }

private:
    ScopedBuffer(const ScopedBuffer &other) = delete;
    ScopedBuffer &operator=(const ScopedBuffer &other) = delete;

    char *mData;
    size_t mSize;
};

#endif
