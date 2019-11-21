#ifndef VECTOR_H
#define VECTOR_H

// 这个文件是 vector 的头文件
#include "pool_allocator.h"
#include "uninitialized.h"
#include "construct.h"
#include <cstddef>
#include <initializer_list>

namespace MySTL {

template<class T,class Alloc = pool_allocator<T> >
class vector{

public:
    typedef  T*             iterator;
    typedef  const T*       const_iterator;
    typedef  T              value_type;
    typedef  T*             pointer;
    typedef  const T*       const_pointer;
    typedef  T&             reference;
    typedef  const T&       const_reference;
    typedef  size_t         size_type;
    typedef  ptrdiff_t      difference_type;

private:
    pointer _start;
    pointer _finish;
    pointer _end_of_storage;

private:
    void fill_initialize(size_type n,const_reference value);
    void deallocate() { if (_start) Alloc::deallocate(_start, _end_of_storage - _start); }
    iterator allocate_and_fill(size_type n, const_reference value );
    void insert_aux(iterator position,const_reference value);
public:
    vector(): _start(nullptr), _finish(nullptr), _end_of_storage(nullptr) {}
    vector(size_type n,const value_type& value) {  fill_initialize(n,value); }
    explicit vector(size_type n) { fill_initialize(n,T()); }
    // 拷贝构造函数
    vector(const vector & x)
    {
        pointer p = Alloc::allocate(x.size());
        uninitialized_copy( x.cbegin(),x.cend(),p);
        _start = p;
        _end_of_storage = _finish = _start + x.size();
    }
    // 拷贝赋值
    vector& operator=(vector& x)
    {
        reserve(x.size());
        uninitialized_copy(x.begin(),x.end(),begin());
        _start;
        _end_of_storage = _finish = _start + x.size();
    }

    template<class InputIterator>
    vector(InputIterator first,InputIterator last)
    {
        size_type num = 0;
        for (InputIterator temp = first; temp != last; ++temp)
            ++num;

        _start = Alloc::allocate(num);
        _end_of_storage = _finish = _start + num;
        uninitialized_copy(first,last,_start);
    }


    vector(const std::initializer_list<value_type>& list ) {
        _start = Alloc::allocate(list.size());
        _finish = _start + list.size();
        pointer cur = _start;
        for (auto item = list.begin(); item != list.end(); ++item)
            Alloc::construct(cur++,*item);
        _end_of_storage = _finish;
    }

    ~vector() { destory(_start,_finish); deallocate(); }

public:
    iterator begin()  {return _start; }
    const_iterator cbegin() const { return _start; }
    const_iterator cend() const { return _finish; }
    iterator end() { return _finish; }
    size_type size() const { return static_cast<size_type>( _finish - _start ); }
    size_type capacity()const { return static_cast<size_type>(_end_of_storage - _start) ; }
    bool empty()const { return _start == _finish; }
    reference operator[](size_type n) { return *( _start + n ); }
    const_reference operator[](size_type n)const { return *( _start + n ); }
    reference front(){ return *_start; }
    reference back(){ return *(_finish-1); }

    // 插入末尾
    void push_back(const_reference value);
    void insert(iterator position,size_type n, value_type value);
    void clear() { erase(_start,_finish); }

    // 弹出末尾
    value_type pop_back() { --_finish;value_type result = *_finish; destory(_finish); return result; }

    // 擦除 position 位置的值
    iterator erase(iterator position){
        if ( position != _finish -1 )
            std::copy(position+1,_finish,position);
        destory(_finish);
        --_finish;
        return position;
    }

    iterator erase(iterator first,iterator last)
    {
        iterator i = std::copy(last,_finish,first);
        destory(i,_finish);
        _finish = _finish-(last-first);
        return first;
    }

    void resize(size_type n)
    {
        resize(n,size_type());
    }
    void resize(size_type n, const_reference value)
    {
        if ( n < size() )
        {
            destory(_start+n,_finish);
            _finish = _start+n;
        }
        else
            insert( _finish,n,value);
    }
    void reserve(size_type new_cap)
    {
        if ( new_cap > capacity() ){
            pointer new_start = Alloc::allocate(new_cap);
            pointer new_finish;
            new_finish = MySTL::uninitialized_copy(_start,_finish,new_start);
            destory(_start,_finish);
            deallocate();
            _start = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + new_cap;
        }
    }

    void swap(vector& another)
    {
        if ( &another != this)
        std::swap(_start,another._start);
        std::swap(_finish,another._finish);
        std::swap(_end_of_storage,another._end_of_storage);
    }

};

template<class T,class Alloc>
void vector<T,Alloc>::fill_initialize(size_type n,  const_reference value)
{
    _start = allocate_and_fill(n,value);
    _finish = _start + n;
    _end_of_storage = _finish;
}

template<class T,class Alloc>
typename vector<T,Alloc>::iterator vector<T,Alloc>::allocate_and_fill(size_type n, const_reference value )
{
    pointer result = Alloc::allocate(n);
    MySTL::uninitialized_fill_n(result,n,value);
    return result;
}

template<class T,class Alloc>
void vector<T,Alloc>::push_back(const_reference value)
{
    if ( _finish != _end_of_storage )
    {
        construct(_finish,value);
        ++_finish;
    }
    else
        insert_aux(_finish,value);
}

template<class T,class Alloc>
void vector<T,Alloc>::insert_aux(iterator position,const_reference value)
{
    if ( (_end_of_storage - _finish) ){
        ++_finish;
        // copy_backward 从尾部开始拷贝，尾部一一对应
        auto value_copy = value;
        std::copy_backward(position,_finish - 2,_finish - 1);
        *position = value_copy;
    }
    else{
        const size_type old_size = size();
        const size_type len = size() == 0? 1 : 2 * old_size ;
        iterator new_start = Alloc::allocate(len);
        iterator new_finish = new_start;
        try{
            new_finish = uninitialized_copy(_start,position,new_start);
            construct(new_finish,value);
            ++new_finish;
            new_finish = uninitialized_copy(position,_finish,new_finish);
        }
        catch(...){
            destory(new_start,new_finish);
            Alloc::deallocate(new_start,len);
        }

        destory(_start,_finish);
        deallocate();

        _start = new_start;
        _finish = new_finish;
        _end_of_storage = new_start + len;
    }

}

template<class T,class Alloc>
void vector<T,Alloc>::insert(iterator position,size_type n, value_type value)
{
    if ( n != 0 ){
        // 总的空间足够
        if ( _end_of_storage-_finish > n ){
            size_type num = static_cast<size_type>(_finish - position);
            //auto old_finish = _finish;
            value_type value_copy = value;
            if (num > n){
                MySTL::uninitialized_copy(_finish-n,_finish,_finish);
                std::copy_backward(position,_finish-n,_finish);
                std::fill(position,position+n,value);
                _finish += n;
            }
            else{
                MySTL::uninitialized_copy(position,_finish,_finish+n-num);
                MySTL::uninitialized_fill(_finish,_finish+n-num,value);
                MySTL::uninitialized_fill(position,_finish,value);
                _finish += n;
            }
        }
        // 总的空间不足
        else{
            const size_type old_size = size();
            // len 为旧长度的2倍，或者是旧长度加上n
            const size_type len = old_size + std::max(old_size,n);
            pointer new_start = Alloc::allocate(len);
            pointer new_finish;
            try{
                new_finish = MySTL::uninitialized_copy(_start,position,new_start);
                MySTL::uninitialized_fill_n(new_finish,n,value);
                new_finish += n;
                MySTL::uninitialized_copy(position, _finish, new_finish);
            }
            catch(...){
                destory(new_start,new_finish);
                Alloc::deallocate(new_start,len);
                throw;
            }
            destory(_start,_finish);
            deallocate();
            _start  = new_start;
            _finish = new_finish;
            _end_of_storage = new_start + len;

        }
    }
}


} // namespce Mystl

#endif // VECTOR_H
