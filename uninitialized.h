#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

// 这个文件包含三个全局函数
// uninitialized_copy
// uninitialized_fill
// uninitialized_fill_n

#include <iterator>
#include <type_traits>
#include "construct.h"

namespace MySTL{

// pod 版本就调用这个版本
template<class InputIterator,class ForwardIterator>
inline ForwardIterator
__uninitialized_copy(InputIterator first,InputIterator last, ForwardIterator result, std::true_type)
{
    return std::copy(first,last,result);
}

// 不是 pod 类型将调用这个版本
template<class InputIterator,class ForwardIterator>
inline ForwardIterator
__uninitialized_copy(InputIterator first,InputIterator last, ForwardIterator result, std::false_type)
{
    auto cur = result;
    try{
        for ( ; first != last; ++first,++cur)
            construct(&*cur,*first);
    }
    catch(...){
        for (; result != cur; ++result)
            destory(&*result);
    }
    return cur;
}


// 函数 uninitialized_copy 将 first 与 last 之间的元素拷贝到 result 的空间中
template<class InputIterator,class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
{
    return __uninitialized_copy(first,last,result,
                                std::is_pod<typename std::iterator_traits<InputIterator>::value_type>() );
}


// pod 类型将会调用这个类型
template<class ForwardIterator,class T>
inline void
__uninitialized_fill(ForwardIterator first, ForwardIterator last, T value,std::true_type)
{
    std::fill(first,last,value);
}

// 非 pod 类型将会调用这个函数
template<class ForwardIterator,class T>
inline void
__uninitialized_fill(ForwardIterator first, ForwardIterator last, T value, std::false_type)
{
    ForwardIterator cur = first;
    try{
        for (; cur != last; ++cur)
            construct(&*cur,value);
    }
    catch(...){
        for (; first != cur; ++first)
            destory(&*cur);
    }

}

// 函数 uninitialized_fill 将 value 的值拷贝到 first 和 last 之间的空间
template<class ForwardIterator,class T>
inline void
uninitialized_fill(ForwardIterator first, ForwardIterator last, T value)
{
    __uninitialized_fill(first,last,value,
                         std::is_pod <typename std::iterator_traits<ForwardIterator>::value_type>() );

}

// pod 类型将调用这个函数
template<class ForwardIterator,class Size,class T>
inline ForwardIterator
__uninitialized_fill_n(ForwardIterator result,Size n, T value,std::true_type)
{
    return std::fill_n(result,n,value);
}

// 非 pod 类型将调用这个函数
template<class ForwardIterator,class Size,class T>
inline ForwardIterator
__uninitialized_fill_n(ForwardIterator result,Size n, T value, std::false_type)
{
    ForwardIterator cur = result;
    auto result_bk = result;
    try{
        for ( ; n > 0; --n,++cur)
            construct(&*cur, value);
    }
    catch(...){
        for (; result != cur; ++result)
            destory(&*result);
    }
    return cur;
}


// 函数 uninitialized_fill_n 用 value 初始化 result 开始的 n 个空间
template<class ForwardIterator,class Size,class T>
inline ForwardIterator
uninitialized_fill_n(ForwardIterator result,Size n, T value)
{
    return __uninitialized_fill_n(result,n,value,
                                  std::is_pod<typename std::iterator_traits<ForwardIterator>::value_type>() );
}



} // namespace MySTL


#endif // UNINITIALIZED_H
