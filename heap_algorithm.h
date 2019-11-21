#ifndef HEAP_ALGORITHM_H
#define HEAP_ALGORITHM_H

namespace MySTL
{

#include <iterator>
#include <type_traits>

// 自底向上重新构建"最大堆"
template <class RandomAccessIterator,class Distance,class T>
void __push_heap(RandomAccessIterator first,Distance holeindex,Distance topindex, T value)
{
    Distance parent = (holeindex-1)/2;
    while ( holeindex > topindex && *(first+parent) < value)
    {
        *(first + holeindex) = *(first + parent);
        holeindex = parent;
        parent = (holeindex - 1)/2;
    }
    *(first + holeindex) = value;
}


template <class RandomAccessIterator,class Distance, class T>
inline void _push_heap_aux(RandomAccessIterator first,RandomAccessIterator last,Distance,T)
{
    __push_heap(first,Distance( (last-first) - 1),Distance(0),T( *(last-1 )) );
}


template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first,RandomAccessIterator last)
{
    _push_heap_aux(first,last,typename std::iterator_traits<RandomAccessIterator>::difference_type(),
                   typename std::iterator_traits<RandomAccessIterator>::value_type() );
}


template <class RandomAccessIterator,class Distance,class T>
void __adjust_heap(RandomAccessIterator first,Distance holeindex, Distance len, T value)
{
    Distance topindex = holeindex;
    Distance secondChild = 2*holeindex + 2;
    while (secondChild < len){
        if  (*(first + secondChild) < * (first + (secondChild -1)) )
            secondChild--;
        *(first + holeindex) = *(first + secondChild );
        holeindex = secondChild;
        secondChild = 2*(secondChild + 1);
    }
    if (secondChild == len)
    {
        *(first + holeindex) = *(first + (secondChild - 1));
        holeindex = secondChild -1;
    }
    __push_heap(first,holeindex,topindex,value);
}




template <class RandomAccessIterator,class T,class Distance>
inline void __pop_heap(RandomAccessIterator first,RandomAccessIterator last,
                           RandomAccessIterator result,T value, Distance)
{
    *result = *first;
    __adjust_heap(first,Distance(0),Distance(last-first),value);
}


template <class RandomAccessIterator,class T>
inline void _pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last,T)
{
    __pop_heap(first,last-1,last-1,T(*(last -1 )),typename std::iterator_traits<RandomAccessIterator>::difference_type());
}


template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last)
{
    _pop_heap_aux(first,last,typename std::iterator_traits<RandomAccessIterator>::value_type());
}


template <class RandomAccessIterator>
void  sort_heap(RandomAccessIterator first, RandomAccessIterator last)
{
    while (last - first > 1)
        pop_heap(first,last--);
}

template <class RandomAccessIterator,class T, class Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,T,Distance)
{
    if (last - first < 2)
        return;
    Distance len = last -first; // 元素个数

    Distance parent = (len - 2)/2; // 首个非叶节点
    while (true){
        __adjust_heap(first,parent,len,T(*(first + parent) ) );
        if (parent == 0) return;
        parent--;
    }
}

template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first,RandomAccessIterator last)
{
    _make_heap(first,last,typename std::iterator_traits<RandomAccessIterator>::value_type(),
               typename std::iterator_traits<RandomAccessIterator>::difference_type() );
}

// 以上为重载版本，可以指定比较操作
// 自底向上重新构建"最大堆"
template <class RandomAccessIterator,class Distance,class T,class Compare>
void __push_heap(RandomAccessIterator first,Distance holeindex,Distance topindex, T value,Compare comp)
{
    Distance parent = (holeindex-1)/2;
    while ( holeindex > topindex && comp(*(first+parent),value) )
    {
        *(first + holeindex) = *(first + parent);
        holeindex = parent;
        parent = (holeindex - 1)/2;
    }
    *(first + holeindex) = value;
}


template <class RandomAccessIterator,class Distance, class T,class Compare>
inline void _push_heap_aux(RandomAccessIterator first,RandomAccessIterator last,Distance,T,Compare comp)
{
    MySTL::__push_heap(first,Distance( (last-first) - 1),Distance(0),T( *(last-1 )),comp );
}


template <class RandomAccessIterator,class Compare>
inline void push_heap(RandomAccessIterator first,RandomAccessIterator last,Compare comp)
{
    _push_heap_aux(first,last,typename std::iterator_traits<RandomAccessIterator>::difference_type(),
                   typename std::iterator_traits<RandomAccessIterator>::value_type(),comp );
}


template <class RandomAccessIterator,class Distance,class T,class Compare>
void __adjust_heap(RandomAccessIterator first,Distance holeindex, Distance len, T value,Compare comp)
{
    Distance topindex = holeindex;
    Distance secondChild = 2*holeindex + 2;
    while (secondChild < len){
        if  (comp(*(first + secondChild), *(first + (secondChild -1))) )
            secondChild--;
        *(first + holeindex) = *(first + secondChild );
        holeindex = secondChild;
        secondChild = 2*(secondChild + 1);
    }
    if (secondChild == len)
    {
        *(first + holeindex) = *(first + (secondChild - 1));
        holeindex = secondChild -1;
    }
    __push_heap(first,holeindex,topindex,value);
}




template <class RandomAccessIterator,class T,class Distance,class Compare>
inline void __pop_heap(RandomAccessIterator first,RandomAccessIterator last,
                           RandomAccessIterator result,T value, Distance,Compare comp)
{
    *result = *first;
    MySTL::__adjust_heap(first,Distance(0),Distance(last-first),value,comp);
}


template <class RandomAccessIterator,class T,class Compare>
inline void _pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last,T,Compare comp)
{
    __pop_heap(first,last-1,last-1,T(*(last -1 )),typename std::iterator_traits<RandomAccessIterator>::difference_type(),comp);
}


template <class RandomAccessIterator,class Compare>
inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last,Compare comp)
{
    _pop_heap_aux(first,last,typename std::iterator_traits<RandomAccessIterator>::value_type(),comp);
}


template <class RandomAccessIterator,class Compare>
void  sort_heap(RandomAccessIterator first, RandomAccessIterator last,Compare comp)
{
    while (last - first > 1)
        pop_heap(first,last--,comp);
}

template <class RandomAccessIterator,class T, class Distance,class Compare>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last,T,Distance,Compare comp)
{
    if (last - first < 2)
        return;
    Distance len = last -first; // 元素个数

    Distance parent = (len - 2)/2; // 首个非叶节点
    while (true){
        MySTL::__adjust_heap(first,parent,len,T(*(first + parent) ),comp );
        if (parent == 0) return;
        parent--;
    }
}

template <class RandomAccessIterator,class Compare>
inline void make_heap(RandomAccessIterator first,RandomAccessIterator last,Compare comp)
{
    _make_heap(first,last,typename std::iterator_traits<RandomAccessIterator>::value_type(),
               typename std::iterator_traits<RandomAccessIterator>::difference_type(),comp );
}


}// namespace MySTL


#endif // HEAP_ALGORITHM_H
