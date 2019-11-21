// c++标准的迭代器

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

// c++标准定义的迭代器类型

#include <new>         // for placement new
#include <cstddef>     // for size_t,ptrdiff_t
#include <cstdlib>     // for exit()
#include <climits>     // for UINT_MAX
#include <iostream>    // for cerr

namespace MySTL {

// 这是一个用来分配内存的函数，调用operator new分配内存
template <class T>
inline T* _allocate(ptrdiff_t size, T* )
{
    std::set_new_handler(0);         // 设置new的处理函数为默认处理函数
    T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
    if (tmp == 0) {
        std::cerr << "out of memory" << std::endl;
        exit(1);
    }
    return tmp;
}

// 释放内存函数，使用operator delete
template <class T>
inline void _deallocate(T* buffer)
{
    ::operator delete(buffer);
}

// operator new 调用构造函数 生成对象
template <class T1, class T2>
inline void _construct(T1* p, const T2& value)
{
    new(p) T1(value); // placement new

}

// 摧毁对象，指针调用 class 的析构函数
template <class T>
inline void _destory(T* ptr)
{
    ptr->~T();
}

// allocator模板类
template <class T>
class allocator
{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    // rebind 重新绑定 important in container like list
    template <class U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    // hint used for locality if not equal to 0
    pointer allocate(size_type n, const void* hint = 0)
    {
        return _allocate((difference_type) n, (pointer)0 );
    }

    void deallocate(pointer p,size_type n)
    {
        _deallocate(p);
    }

    void construct(pointer p, const T& value)
    {
        _construct(p, value);
    }

    void destory(pointer p)
    {
        _destory(p);
    }

    pointer address(reference x)
    {
        return (pointer) &x;
    }

    const_pointer address(const_reference x)
    {
        return (const_pointer) &x;
    }

    size_type max_size() const
    {
        return size_type(UINT_MAX/sizeof(T));
    }
};

}

#endif // ALLOCATOR_H
