// 这个头文件包含两个全局函数 destory 和 construct

#ifndef CONSTURCT_H
#define CONSTURCT_H



#include <new>
#include <type_traits>
#include <iterator>

namespace MySTL {

template <class T1, class T2>
void construct(T1* p, T2& value)
{
    new(p) T1(value);
}

// 接受一个指针的destory函数负责析构一个对象
template <class T>
void destory(T* p)
{

    p->~T();
}


/* 注，G4.9源代码中的 type_traits 定义在 type_traits 中，都继承自
 * typedef integral_constant<bool, true>      true_type
 * typedef integral_constant<bool, false>    false_type
 * 这两个类类型。
 */

template <class ForwardIterator>
inline void __destory(ForwardIterator first, ForwardIterator last, std::false_type)
{
    for ( ; first != last; ++first)
    destory(&*first);
}

template <class ForwardIterator>
inline void __destory(ForwardIterator first,ForwardIterator last, std::true_type ) { }


// 接受一对迭代器的destory负责析构一堆对象
template <class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
    __destory(first,last,std::is_trivially_destructible<typename std::iterator_traits<ForwardIterator>::value_type>());

}

inline void destory(char*, char*) {}

inline void destory(wchar_t*, wchar_t*) {}

}

#endif // CONSTURCT_H
