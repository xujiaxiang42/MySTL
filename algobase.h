// copy 算法 和 copy_backword 算法

#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <iterator.h>
#include <iterator>
#include <type_traits>
#include <string.h>

namespace MySTL{

// copy_d 函数， 处理 random_access_iterator 类型的 不能调用memmove 的类型
template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last,
                              OutputIterator result, Distance)
{

    for (Distance n = last - first; n > 0; --n,++result,++first)
        *result = *first;
    return result;
}

// 该函数处理的是 input_iterator 型别的迭代器
template <class InputIterator, class OutputIterator>
inline OutputIterator _copy(InputIterator first, InputIterator last,
                            OutputIterator result,std::input_iterator_tag)
{
    for (; first != last; ++result,++first)
        *result = *first;
    return result;
}

// 该函数处理的是 random_access_iterator 型别的迭代器
template <class RandomAccessIterator, class OutputIterator>
inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last,
                            OutputIterator result, std::random_access_iterator_tag)
{
    // 转为调用 _copy_d() 函数， 因为下面有一个函数处理方式与该函数一致
    return _copy_d(first, last, result, typename Iterator_traits<RandomAccessIterator>::difference_type());
}

// 该函数处理 trivial_assignment 的指针类型，可以直接调用memmove
template <class T>
inline T* _copy_t(const T* first, const T* last, T* result, std::true_type)
{
    memmove(result, first, sizeof(T) * (last-first));
    return result+(last-first);
}

// 该函数处理 non_trivial_assignment 的指针类型， 不能调用 memmove,转为调用 _copy_d() 函数.
template <class T>
inline T* _copy_t(const T* first, const T* last, T* result, std::false_type)
{
    return _copy_d(first,last,result, static_cast<ptrdiff_t>(0));
}



// 三个分流 仿函数
// 该仿函数为 迭代器类型 分流
template <class InputIterator, class OutputIterator>
struct _copy_dispatch{
    OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
    {
        return _copy(first,last, result, typename std::iterator_traits<InputIterator>::iterator_category() );
    }
};

// 该仿函数为 原生指针类型 分流
template <class T>
struct _copy_dispatch<T*, T*>
{
    T* operator()(T* first, T* last, T* result)
    {

        return _copy_t(first,last, result, std::has_trivial_copy_assign<T>());
    }
};

// 该仿函数为 原生指针类型 分流
template <class T>
struct _copy_dispatch<const T*, T*>
{
    T* operator()(const T* first, const T* last, T* result)
    {

        return _copy_t(first,last, result, std::has_trivial_copy_assign<T>());
    }
};



// copy 函数
template <class InputIterator,  class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
{
    return _copy_dispatch<InputIterator,OutputIterator>()(first,last,result);
}

// 重载的copy 函数,为 char* 设计
inline char* copy(const char* first, const char* last, char* result)
{
    memmove(result,first,last-first);
    return result + (last - first);
}
// 重载的copy 函数,为 wchar_t* 设计
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
{
    return result + (last - first);
    memmove(result,first,sizeof(wchar_t) *(last-first) );
}

/************************************* copy_backword ********************************/

// 该函数处理 迭代器类型，或者 non_trival_assignment 类型
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 __copy_backward( BidirectionalIterator1 first,
                                              BidirectionalIterator1 last,
                                              BidirectionalIterator2 result) {
  while ( first != last ) *--result = *--last;
  return result;
}

// 该函数处理 迭代器类型， 转为调用 _copy_backward
template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __copy_backward_dispatch
{
  BidirectionalIterator2 operator()(BidirectionalIterator1 first,
                                    BidirectionalIterator1 last,
                                    BidirectionalIterator2 result) {
    return __copy_backward(first, last, result);
  }
};


// 该函数处理 trival assignment 类型
template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result,
                            std::true_type) {
  const ptrdiff_t N = last - first;
  memmove(result - N, first, sizeof(T) * N);
  return result - N;
}

// 该函数处理 non_trival assignment 类型， 转为调用函数 _copy_backward
template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result,
                            std::false_type) {
  return __copy_backward(first, last, result);
}

//  该对象为原生指针分流
template <class T>
struct __copy_backward_dispatch<T*, T*>
{
    T* operator()(T* first, T* last, T* result) {
        return __copy_backward_t(first, last, result, std::has_trivial_copy_assign<T>() );
    }
};

//  该对象为原生指针分流
template <class T>
struct __copy_backward_dispatch<const T*, T*>
{
    T* operator()(const T* first, const T* last, T* result)
    {
        return __copy_backward_t(first, last, result, std::has_trivial_copy_assign<T>());
    }
};


// 该函数为总的泛化版本,调用dispatch
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                            BidirectionalIterator1 last,
                                            BidirectionalIterator2 result)
{
    return __copy_backward_dispatch<BidirectionalIterator1,
                                  BidirectionalIterator2>()(first, last,result);
}

} // namespace MySTL

#endif // ALGOBASE_H
