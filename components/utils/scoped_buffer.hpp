#ifndef UTILS_BUFFER_H
#define UTILS_BUFFER_H

#include <assert.h>
#include <cstddef>     // for size_t
#include <string.h>
#include <stdexcept>

/// Non-Copyable (move passable) utilitary buffer with RAII scoping

class ScopedBuffer
{
public:
    ScopedBuffer(size_t size);
    ScopedBuffer(ScopedBuffer &&src);
    ~ScopedBuffer();
    char *operator*();
    const char *operator*() const;
    char &operator[](size_t idx);
    const char &operator[](size_t idx) const;
    operator char *();
    operator const char *() const;
    size_t size() const;

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
    void copy_from(size_t offset, const void *sptr, size_t ssize);

private:
    ScopedBuffer(const ScopedBuffer &other) = delete;
    ScopedBuffer &operator=(const ScopedBuffer &other) = delete;

    char *mData;
    size_t mSize;
};

#endif
