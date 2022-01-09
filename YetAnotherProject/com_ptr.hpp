#ifndef INCLUDED_COMPTR
#define INCLUDED_COMPTR

#pragma once

#include <utility>


template <class T>
class com_ptr
{
private:
    T* ptr;

    static void release(T* ptr)
    {
        if (ptr)
            ptr->Release();
    }

    static void acquire(T* ptr)
    {
        if (ptr)
            ptr->AddRef();
    }

public:
    com_ptr(nullptr_t)
        : ptr(nullptr)
    {
    }

    explicit com_ptr(T* ptr = nullptr)
        : ptr(ptr)
    {
    }

    com_ptr(const com_ptr& p)
        : ptr(p.ptr)
    {
        acquire(ptr);
    }

    com_ptr(com_ptr&& p)
        : ptr(p.ptr)
    {
        p.ptr = nullptr;
    }

    ~com_ptr()
    {
        release(ptr);
    }

    com_ptr& operator =(const com_ptr& p)
    {
        acquire(p.ptr);
        release(ptr);
        ptr = p.ptr;
        return *this;
    }

    com_ptr& operator =(com_ptr&& p)
    {
        swap(*this, p);
        return *this;
    }

    T* operator ->() const { return ptr; }

    operator T* () const { return ptr; }

    T* const& get() const { return ptr; }

    T** rebind()
    {
        release();
        return &ptr;
    }

    void release()
    {
        if (ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }

    friend void swap(com_ptr& a, com_ptr& b)
    {
        using std::swap;
        swap(a.ptr, b.ptr);
    }
};

template <class T>
inline com_ptr<T> make_com_ptr(T* ptr)
{
    return com_ptr<T>(ptr);
}

#endif  // INCLUDED_COMPTR
