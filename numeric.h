#ifndef NUMERIC_H
#define NUMERIC_H

#include "iterator.h"

// some 数值 algorithm
namespace MySTL
{
// add all number among [first,last) to init value
template <class InputIterator,class T>
T accumulate(InputIterator first, InputIterator last, T init)
{
    for (; first != last ; ++first)
        init += *first;
    return init;
}

// use operation we offer in op
template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init,BinaryOperation op)
{
    for (; first != last ; ++first)
        init = op(init, *first);
    return init;
}

template<class InputIterator,class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
{
    if (first == last) return result;
    *result  = *first; // 第一个存放首元素，接下来为相邻元素之差
    InputIterator tmp = first;
    ++tmp;
    for ( ; tmp != last; ++first, ++tmp)
    {
        *(++result) = *tmp - *first;

    }
    return ++result;
}

template<class InputIterator,class OutputIterator,class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                            OutputIterator result,BinaryOperation op)
{
    if (first == last) return result;
    *result  = *first; // 第一个存放首元素，接下来为相邻元素之差
    InputIterator tmp = first;

    ++tmp;
    for ( ; tmp != last; ++first, ++tmp)
    {
        *(++result) = op(*tmp, *first);

    }
    return ++result;
}

// 内积
template <class InputIterator1,class InputIterator2, class T>
T inner_product(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2, T init)
{
    while (first1 != last1)
    {
        init += (*first1) * (*first2);
        ++first1;
        ++first2;
    }
    return init;
}

template <class InputIterator1,class InputIterator2, class T, class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1,InputIterator1 last1,InputIterator2 first2, T init,
                BinaryOperation1 op1, BinaryOperation2 op2)
{
    while (first1 != last1)
    {
        init =op2(init, op1((*first1),(*first2)) );
        ++first1;
        ++first2;
    }
    return init;
}

// 前n个元素的和
template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result)
{
    if (first == last) return result;
    typename Iterator_traits<InputIterator>::value_type tmp = *first;
    *result = tmp;
    ++first;
    for (;first != last ; ++first)
    {
        tmp += *first;
        *(++result) = tmp;
    }
    return ++result;
}

template <class InputIterator, class OutputIterator,class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result,BinaryOperation op)
{
    if (first == last) return result;
    typename Iterator_traits<InputIterator>::value_type tmp = *first;
    *result = tmp;
    ++first;
    for (;first != last ; ++first)
    {
        tmp = op(tmp,*first);
        *(++result) = tmp;
    }
    return ++result;
}

// 幂操作
template <class T,class Integer>
inline T power(T x, Integer times)
{
    if (times == 0) return static_cast<T>(1);
    while ( (times & 1) == 0 )
    {
        times >>= 1;
        x = x*x;
    }
    T result = x;
    times >>= 1;
    while ( times != 0 )
    {
        x = x * x;
        if ( (times & 1) != 0)
            result *= x;
        times >>= 1;
    }
    return result;

}




} // namespace MySTL

#endif // NUMERIC_H
