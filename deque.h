#ifndef DEQUE_H
#define DEQUE_H

#include "pool_allocator.h"
#include "uninitialized.h"
#include "construct.h"
#include <initializer_list>

namespace MySTL
{

// 这个函数用来计算 deque 的 buffsize
inline size_t _deque_buf_size(size_t n, size_t sz){
    return n != 0 ? n : (sz < 512? static_cast<size_t>(512/sz) : static_cast<size_t>(1));
}

// 定义 deque 的迭代器 Bufsiz 是用户指定的缓冲区大小
template <class T,size_t Bufsiz>
class _deque_iterator
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = ptrdiff_t;

    using size_type = size_t;
    using iterator = _deque_iterator; // 迭代器类型

public:
    // 默认构造函数
    _deque_iterator() = default;

public:
    // buffer_size() 返回一个缓冲区中的元素个数
    static size_type buffer_size()
    {
        return _deque_buf_size(Bufsiz, sizeof(T));
    }
public:
    // 数据成员
    pointer         first;
    pointer         last;
    pointer         cur;
    pointer*        map_pointer; // 指向控制中心
public:
    // 设置迭代器中的 指向控制中心的指针 map_pointer,不设定cur，因为 ++/-- 中的操作不一样
    void set_map_pointer( pointer* x )
    {
        map_pointer = x;
        first = *x;
        last = first + buffer_size();
    }
    // 定义两个迭代器之间的减法，结果是中间的元素个数
    difference_type operator-(const iterator & x ) const
    {
        return (map_pointer - x.map_pointer - 1)
                * static_cast<difference_type>(buffer_size()) + cur - first + x.last - x.cur;
    }
    // 前置加法
    iterator& operator++()
    {
        ++cur;
        if ( cur == last )
        {
            set_map_pointer( ++map_pointer );
            cur = first;
        }
        return *this;
    }
    // 后置加法
    iterator operator++(int) { iterator temp = *this; ++*this; return temp; }
    // 前置减
    iterator& operator--()
    {
        if ( cur == first )
        {
            set_map_pointer( --map_pointer );
            cur = last;
        }
        --cur;
        return *this;
    }
    // 后置减
    iterator operator--(int)
    {
        iterator temp = *this;
        --*this;
        return temp;
    }
    iterator& operator+=(difference_type n)
    {
        difference_type offset = n + cur - first; // 相对于开头的总的偏移量
        // 如果偏移小于一个缓冲区
        if ( offset >= 0 && offset < static_cast<difference_type>( buffer_size() ) )
            cur = first + offset ;
        else
        {
            difference_type map_pointer_offset =
                    offset > 0? offset/static_cast<difference_type>(buffer_size())
                              :-( ( -offset -1)/static_cast<difference_type>( buffer_size() ))- 1 ;
            set_map_pointer( map_pointer + map_pointer_offset );
            cur = first + ( offset - map_pointer_offset * buffer_size());
        }
        return *this;
    }
    iterator& operator-=(difference_type n)
    {
        return *this += (-n) ;
    }
    iterator operator+(difference_type n)
    {
        iterator temp = *this;
        return temp += n;
    }
    iterator operator-(difference_type n)
    {
        iterator temp = *this;
        return temp -= n;
    }
    // 接受语意 deque[5] = *(iterator + 5) // 为deque却做准备
    reference operator[](difference_type n) { iterator temp = *this + n; return *temp;  }
    // 解引用
    reference operator*() { return *cur; }
    bool operator==(const iterator & x) const { return cur == x.cur; }
    bool operator!=(const iterator & x) const { return !( operator==(x) ) ;}
    bool operator<( const iterator & x) const
    { return map_pointer == x.map_pointer? cur > x.cur: map_pointer < x.map_pointer;  }

};

// deque 的定义
template <class T,class Alloc = pool_allocator<T>, size_t Bufsiz = 0 >
class deque
{
public:
    using value_type = T;
    using reference = T& ;
    using pointer = T*;
    using const_reference = T&;
    using const_pointer = T*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    // 迭代器类型
    typedef _deque_iterator<T,Bufsiz> iterator;
    // const迭代器还没有实现
    //using const_iterator = iterator;


    // using iterator = _deque_iterator<T,Bufsiz> ;

    // 下面是两个分配器
private:
    using data_allocator = Alloc;
    using map_pointer_allocator = typename Alloc::template rebind<pointer>::other;
    const static size_type INI_MAP_SIZE = 8;

private:
    // 数据成员
    iterator start;
    iterator finish;
    pointer* map_pointer;
    size_type map_pointer_size; // 指向的"控制中心" 有多少个可以使用的控制端口

public:
    // 默认构造函数
    deque(): map_pointer_size(0),start(),finish(),map_pointer(nullptr){ fill_initialize(0,value_type());}
    // 构造函数
    deque(int n,const value_type& value): map_pointer_size(0),start(),finish(),map_pointer(nullptr)
        { fill_initialize(n,value); }
    deque(int n): map_pointer_size(0),start(),finish(),map_pointer(nullptr)
        { fill_initialize(n,value_type()); }
    // 拷贝构造函数
    deque(const deque& other) noexcept
    {
          create_map_and_buffer(other.size());
          iterator cur = start;
          for (iterator item = other.begin(); item != other.end(); ++item )
              data_allocator::construct((cur++).cur,*item);
    }
    // 接受初始值列表的构造函数
    deque(std::initializer_list<value_type> init)
    {
          create_map_and_buffer( init.size() );
          iterator cur = start;
          for(auto it = init.begin(); it !=init.end(); ++it)
              data_allocator::construct( (cur++).cur, *it);
    }
    // 接受两个迭代器的构造函数
    template<class Iterator>
    deque(Iterator begin,Iterator end)
    {
        Iterator begin_copy = begin;
        size_type num = 0;
        while ( (begin_copy++) != end)
            ++num;
        create_map_and_buffer( num );
        iterator cur = start;
        for (; begin != end; ++begin )
            data_allocator::construct((cur++).cur,*begin);
    }
    // 析构函数
    ~deque(){ _clear();}
    // 移动构造
    deque(deque&& other)
    {
        start               = other.start;
        finish              = other.finish;
        map_pointer         = other.map_pointer;
        map_pointer_size    = other.map_pointer_size;
        other.map_pointer   = nullptr;
    }
    // 拷贝赋值
    deque& operator=(const deque& other)
    {
        create_map_and_buffer(other.size());
        iterator cur = start;
        for (iterator item = other.begin(); item != other.end(); ++item )
            data_allocator::construct((cur++).cur,*item);
        return *this;
    }
    // 移动赋值
    deque& operator=(deque&& other)
    {
        _clear();
        start               = other.start;
        finish              = other.finish;
        map_pointer         = other.map_pointer;
        map_pointer_size    = other.map_pointer_size;
        other.map_pointer   = nullptr;
        return *this;
    }

private:
    // 该函数用在析构函数中释放内存
    void _clear()
    {
        if( map_pointer != nullptr)
        {
            // 释放中间的缓冲区
            for ( pointer* temp = start.map_pointer + 1; temp < finish.map_pointer; ++temp)
            {
                auto item = *temp;
                for ( size_type i = 0; i < buffer_size(); ++i)
                    data_allocator::destory(item++);
                data_allocator::deallocate(*temp,buffer_size());
            }
            // 释放首尾的缓冲区
            if (start.map_pointer == finish.map_pointer )
            {
                auto p = start.cur;
                for (; p != finish.cur; ++p )
                   data_allocator::destory(p);
                data_allocator::deallocate(start.first,buffer_size());
            }
            else{
                pointer p = start.cur;
                for (; p != start.last; ++p )
                    data_allocator::destory(p);
                data_allocator::deallocate(start.first,buffer_size());
                p = finish.first;
                for (; p != finish.cur; ++p )
                    data_allocator::destory(p);
                data_allocator::deallocate(finish.first,buffer_size());
            }
            //size_type num = finish.map_pointer - start.map_pointer + 1;
            map_pointer_allocator::deallocate( map_pointer ,map_pointer_size);
        }
    }
    size_type buffer_size() { return  _deque_buf_size(Bufsiz, sizeof(T)); }
    void fill_initialize(size_type n,const value_type& value)
    {
        create_map_and_buffer(n);
        pointer* cur;
        for (cur = start.map_pointer; cur < finish.map_pointer; ++cur)
            uninitialized_fill(*cur,*cur + buffer_size(), value);
        uninitialized_fill(finish.first, finish.cur,value);
    }
    void create_map_and_buffer(size_type num_elements)
    {
        size_type num_buffer = num_elements/buffer_size() + 1;
        map_pointer_size = std::max(static_cast<size_type>(INI_MAP_SIZE), num_buffer + 2 );
        map_pointer = map_pointer_allocator::allocate(map_pointer_size);

        pointer* nstart = map_pointer + (map_pointer_size - num_buffer)/2;
        pointer* nfinish = nstart + num_buffer -1;
        pointer* cur;
        for (cur = nstart; cur <= nfinish; ++cur)
            *cur = data_allocator::allocate(buffer_size());
        start.set_map_pointer(nstart);
        finish.set_map_pointer(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % buffer_size();
    }

    void push_front_aux(const value_type& val)
    {
        value_type val_copy = val;
        // 如果有必要， 则更换 map
        reserve_map_at_front();
        *(start.map_pointer - 1) = data_allocator::allocate( buffer_size() );
        start.set_map_pointer( --start.map_pointer);
        start.cur = start.last - 1;
        construct(start.cur,val_copy);
    }

    void push_back_aux(const value_type & val)
    {
        value_type val_copy = val;
        // 如果有需要，则更换 map
        reserve_map_at_back();
        *(finish.map_pointer + 1) = data_allocator::allocate( buffer_size() );
        construct( finish.cur,val_copy );
        finish.set_map_pointer(++finish.map_pointer);
        finish.cur = finish.first;
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front )
    {
        size_type old_num_nodes = finish.map_pointer - start.map_pointer + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        pointer* new_nstart;
        // 如果只是分配不均，而 map 空间足够，则重新安排，无需重新分配map
        if ( map_pointer_size > 2 * new_num_nodes ){
            new_nstart =map_pointer + (map_pointer_size - new_num_nodes)/2 +(add_at_front? nodes_to_add:0);
            if ( new_nstart < start.map_pointer )
                std::copy(start.map_pointer,finish.map_pointer+1,new_nstart);
            else
                std::copy_backward(finish.map_pointer,finish.map_pointer,new_nstart);
        }
        else // 否则重新分配 map 空间
        {
            size_type new_map_size = map_pointer_size + std::max(map_pointer_size,nodes_to_add) + 2;
            pointer* new_map = map_pointer_allocator::allocate(new_map_size);
            new_nstart = new_map + ( new_map_size - new_num_nodes)/2
                    + (add_at_front? nodes_to_add:0);
            std::copy(start.map_pointer,finish.map_pointer+1,new_nstart);
            map_pointer_allocator::deallocate(map_pointer,map_pointer_size);
            map_pointer_size = new_map_size;
            map_pointer = new_map;
        }
        start.set_map_pointer(new_nstart);

        finish.set_map_pointer(new_nstart + old_num_nodes - 1);

    }

    void reserve_map_at_back( size_type nodes_to_add = 1)
    {
        if ( nodes_to_add  > map_pointer_size -(finish.map_pointer - map_pointer) - 1 )
            reallocate_map(nodes_to_add,false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1)
    {
        if (nodes_to_add > start.map_pointer - map_pointer)
            reallocate_map(nodes_to_add,true);
    }

    value_type pop_back_aux()
    {
        value_type temp;
        map_pointer_allocator::deallocate(finish.map_pointer,buffer_size());
        finish.set_map_pointer( finish.map_pointer - 1 );
        finish.cur = finish.last - 1;
        temp = *finish.cur;
        destory(finish.cur);
        return temp;
    }
    value_type pop_front_aux()
    {
        value_type temp;
        temp = *start.cur;
        destory(start.cur);
        map_pointer_allocator::deallocate(start.map_pointer,buffer_size());
        start.set_map_pointer(start.map_pointer + 1);
        start.cur = start.first;
        return temp;
    }
    iterator insert_aux(iterator pos,const value_type& x)
    {
        difference_type index = pos - start;
        if ( index < finish - pos){
            push_front( front() );
            iterator old_start = begin() + 1;
            pos = start + index;
            iterator old_pos = pos + 1;
            std::copy(old_start, old_pos, start);
        }
        else{
            push_back( back() );
            iterator old_finish = finish - 1;
            pos = start + index;
            iterator old_pos = pos;
            std::copy_backward(old_pos,old_finish,finish);
        }
        *pos  = x;
        return pos;
    }


public:
    // 方法
    size_type capacity() {return map_pointer_size;}
    iterator begin() { return start;   }
    iterator end()   { return finish;  }
    reference front() { return *start;  }
    reference back() { return *(finish - 1); }
    reference operator[](size_type n) { return start[ static_cast<difference_type>(n) ]; }
    reference at(size_type n)
    { if (n < size()) return operator[](n); else throw std::out_of_range("out of range");}
    size_type size() const { return finish - start; }
    bool empty() const { return start == finish; }
    void push_back(const value_type& val)
    {
        // 缓冲区还剩1个以上，只剩一个应当分配新的缓冲区，以便调整 finish.cur 指针
        if (finish.cur != finish.last - 1 )
            construct(finish.cur++,val);
        else
            push_back_aux(val);
    }

    void push_front(const value_type& val)
    {
        if (start.cur != start.first )
            construct(--start.cur,val);
        else
            push_front_aux(val);
    }
    value_type pop_back()
    {
        value_type temp;
        if (finish.cur != finish.first){
            temp = *(finish.cur - 1);
            destory(finish.cur);
            --finish.cur;
        }
        else
            temp = pop_back_aux(); // cur 在finish头
        return temp;
    }

    value_type pop_front()
    {
        value_type temp;
        if (start.cur != start.last - 1){
            temp = *start.cur;
            destory(start.cur);
            ++start.cur;
        }
        else
            temp = pop_front_aux(); // cur 在start的尾巴前一个
        return temp;
    }
    void clear()
    {
        // 中间控制点的要清楚所有的数据
        for (pointer* temp = start.map_pointer + 1; temp < finish.map_pointer; ++temp)
        {
            destory(*temp,*temp + buffer_size());
            data_allocator::deallocate(*start.map_pointer,buffer_size());
        }
        // 如果有start和finish两个控制点，记得保留start
        if (start.map_pointer != finish.map_pointer){
            destory(start.cur,start.last);
            destory(finish.first,finish.cur);
            data_allocator::deallocate(*finish.map_pointer,buffer_size());
        }
        else{
            destory(start.cur,finish.cur);
        }
        finish = start;
    }
    // 清除某个元素
    iterator earse(iterator pos)
    {
        iterator next = pos;
        ++ next;
        difference_type index = pos - start;
        if (index <= finish - pos ){
            std::copy_backward(start,pos,next);
            pop_front();
        }
        else{
            std::copy(next,finish,pos);
            pop_back();
        }
        return start + index;
    }

    iterator erase(iterator first,iterator last)
    {
        if ( first == start && last == finish ){
            clear();
            return start;
        }
        else{
            difference_type num = last - first;// 元素个数
            difference_type elems_before = first - start;
            if ( elems_before < finish - last ){
                iterator new_start = start + num ;
                std::copy_backward(start,first,last);
                destory(start, new_start);
                for ( pointer* cur = start.map_pointer;  cur < new_start.map_pointer; ++cur)
                    data_allocator::deallocate( *cur ,buffer_size());
                start = new_start;
            }
            else{
                iterator new_finish = finish - num;
                std::copy(last,finish,first);
                destory(new_finish,finish);
                for (pointer* cur = finish.map_pointer; cur >= new_finish; --cur)
                    data_allocator::deallocate(*cur,buffer_size());
                finish = new_finish;
            }
            return start + elems_before;
        }
    }

    iterator insert(iterator pos,const value_type & x)
    {
        if (pos.cur == start.cur){
            push_front(x);
            return start;
        }
        else if( pos.cur == finish.cur )
        {
            push_back(x);
            return finish - 1;
        }
        else
            return insert_aux(pos,x);
    }


};





} // namespace MySTL

#endif // DEQUE_H
