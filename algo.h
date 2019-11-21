// 实现了 introsort 算法

#ifndef ALGO_H
#define ALGO_H

#include <iterator>
#include <algobase.h>
#include <algorithm>


namespace MySTL{

const int _stl_threshod = 16;

// 无需边界判断的 插入排序
template <class RandomAccessIterator,class T>
void _unguarded_linear_insert(RandomAccessIterator last, T value)
{
    RandomAccessIterator next = last;
    --next;
    while ( value < *next )
    {
        *last = * next;
        last = next;
        --next;
    }
    *last = value;
}

//  插入排序中的 边界判断
template <class RandomAccessIterator,class T>
inline void _linear_insert(RandomAccessIterator first, RandomAccessIterator last, T)
{
    T value = *last;
    // 如果小于第一个元素，直接插在第一个前面
    if (value < *first ){
        MySTL::copy_backward(first,last,last+1);
        *first = value;
    }
    // 否则, 一定在这之中，无需边界判断
    else
        _unguarded_linear_insert(last,value);
}

// 插入排序法
template <class RandomAccessIterator >
void _insertion_sort(RandomAccessIterator first,
                     RandomAccessIterator last)
{
    if ( first == last ) return;
    for ( RandomAccessIterator i = first + 1; i != last; ++i )
        _linear_insert(first,i,typename std::iterator_traits<RandomAccessIterator>::value_type());
}

// --------------------------------- Quick-sort ---------------------------- //


// 返回三个值之间的中间值
template <class T>
inline const T& _median(const T& a, const T& b, const T& c)
{
    if ( a < b )
        if (b < c )
            return b;
        else if ( a < c )
            return c;
        else
            return a;
    else if ( a < c )
        return a;
    else if (b < c)
        return c;
    else
        return b;
}

// partition
template <class RandomAccessIterator, class T>
RandomAccessIterator _unguarded_partition(RandomAccessIterator first, RandomAccessIterator last, T pivot)
{
    while (true){
        while (*first < pivot ) ++first;
        --last;
        while (pivot < *last ) --last;
        if ( !(first < last) ) return first;
        std::iter_swap(first,last);
        ++first;
    }
}

// _lg(),该函数用来确定quick_sort递归的深度
// 最深层一般为 2 log ^ n
template <class Size>
inline Size _lg( Size n )
{
    Size k;
    for (k = 0; n > 1; n >>= 1) ++k;
    return k;
}

// _unguarded_insertion_sort
template <class RandomAccessIterator,class T>
inline void _unguarded_insertion_sort_aux(RandomAccessIterator first,
                                          RandomAccessIterator last, T)
{
    for (RandomAccessIterator i = first; i != last; ++i)
        _unguarded_linear_insert(i, static_cast<T>(*i));
}

//  _unguarded_insertion_sort
template <class RandomAccessIterator>
inline void _unguarded_insertion_sort(RandomAccessIterator first,RandomAccessIterator last)
{
     _unguarded_insertion_sort_aux(first,last,typename
                                   std::iterator_traits<RandomAccessIterator>::value_type());
}

//  对小于16的序列进行排序
template <class RandomAccessIterator>
void _final_insertion_sort(RandomAccessIterator first,
                     RandomAccessIterator last )
{
    if (last-first > _stl_threshod ){
        _insertion_sort(first,first+_stl_threshod );
        _unguarded_insertion_sort(first+ _stl_threshod, last);
    }
    else
        _insertion_sort(first,last);

}

// _introsort_loop 对 quick-sort 进行优化的一种排序算法
template <class RandomAccessIterator,class T, class Size>
void _introsort_loop(RandomAccessIterator first,
                     RandomAccessIterator last,
                     T, Size depth_limit )
{
    while (last - first > _stl_threshod )
    {
        if ( depth_limit == 0 ){
            std::partial_sort(first, last, last);
            return;
        }
        --depth_limit;
        auto cut = _unguarded_partition(first,last,T( _median(*first,*(first+(last-first)/2),*(last-1))));
        _introsort_loop(cut,last,typename std::iterator_traits<RandomAccessIterator>::value_type(),depth_limit);
        last = cut;
    }
}

// sort 排序算法
template <class RandomAccessIterator>
inline void sort(RandomAccessIterator first,
                 RandomAccessIterator last)
{
    if ( first != last )
    {
        _introsort_loop(first,last,
                   typename std::iterator_traits<RandomAccessIterator>::value_type(),_lg(last-first)*2 );
        _final_insertion_sort(first,last);
    }
}



} // namespace MySTL

#endif // ALGO_H
