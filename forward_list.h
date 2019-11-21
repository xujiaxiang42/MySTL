#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <cstddef>
#include "pool_allocator.h"
#include "construct.h"
#include <initializer_list>
#include <iostream>
#include "vector.h"

namespace MySTL
{

// 先定义链表节点类型
template <class T>
struct _forward_list_node
{
    _forward_list_node* next;
    T data;
};

// 定义 forward_list 的迭代器类型
template <class T>
class _forward_list_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = ptrdiff_t;

    using iterator = _forward_list_iterator;
public:
    // 构造函数
    _forward_list_iterator( _forward_list_node<T>* x) : node_pointer(x) {}
    // 默认构造函数
    //_forward_list_iterator(): node_pointer(nullptr) {}
public:
    // 数据成员
    _forward_list_node<T>* node_pointer;
public:
    reference operator*() { return node_pointer->data; }
    iterator& operator++() { node_pointer = node_pointer->next; return *this; }
    iterator operator++(int) { iterator temp = *this; node_pointer = node_pointer->next; return temp; }
    bool operator==(const iterator& x) const { return node_pointer == x.node_pointer; }
    bool operator!=(const iterator& x) const { return node_pointer != x.node_pointer; }
};

template<class T,class Alloc = pool_allocator<T>>
class forward_list
{
public:
    using value_type = T;
    using size_type = size_t;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using iterator = _forward_list_iterator<T>;
    using node = _forward_list_node<T>;
    // 分配器
    using data_allocator =typename Alloc::template rebind<node>::other;

public:
    // 数据成员
    node* node_pointer;

public:
    // 默认构造函数
    forward_list(): node_pointer(nullptr) {}

    // 接受两个迭代器的构造函数
    template <class InputIterator>
    forward_list(InputIterator first,InputIterator last):node_pointer(nullptr)
    {
        // 迫不得已，使用vector作为中介，以保证forward_list的正确顺序
        MySTL::vector<value_type> vec(first,last);
        auto temp = vec.end();
        --temp;
        for( ;temp >= vec.begin(); --temp)
            push_front(*temp);
        }

    // 接受 initialized_list 的构造函数
    forward_list(const std::initializer_list<value_type>& list) : node_pointer(nullptr)
    {
        // 迫不得已，使用vector作为中介，以保证forward_list的正确顺序
        MySTL::vector<value_type> vec(list.begin(),list.end());

        auto item = vec.end();
        --item;
        for (; item >= vec.begin(); --item)
            push_front(*item);

    }

    // 析构函数
    ~forward_list()
    {
        while (node_pointer != nullptr )
            pop_front();
    }

public:
    node* create_node(const value_type & x)
    {
        node* p = data_allocator::allocate();
        construct( &p->data,x);
        p->next = nullptr;
        return p;
    }
    void delete_node(node* p)
    {
        destory( &p->data );
        data_allocator::deallocate(p);
    }

    iterator begin() { return iterator(node_pointer);}
    iterator end() { return iterator(nullptr); }
    size_type size() const
    {
        size_type num = 0;
        for ( node* temp = node_pointer; temp != nullptr ; temp = temp->next)
            ++num;
        return num;
    }
    bool empty() const { return node_pointer == nullptr;}
    reference front() { return node_pointer->data; }
    void push_front(const value_type& x) {
        node* p = create_node(x);
        if ( node_pointer != nullptr )
        {
            p->next = node_pointer;
            node_pointer = p;
        }
        else
            node_pointer = p;
    }
    void pop_front() {
        if ( node_pointer != nullptr )
        {
            node* p = node_pointer;
            node_pointer = p->next;
            delete_node(p);
        }
    }

    // 与其他容器插入前方不同，forward_list插入到后方
    void insert(iterator pos,const value_type& x)
    {
        node* p = create_node(x);
        p->next = pos.node_pointer->next;
        pos.node_pointer->next = p;
    }

    void clear() { while (node_pointer != nullptr ) pop_front(); }

    void erase(iterator pos) {
        if (pos == begin())
            pop_front();
        else{
            // 先找到该位置的前一个位置
            iterator temp = begin();
            ++temp;
            auto backup = begin();
            while (temp != pos ){
                ++backup;
                ++temp;
            }
            backup.node_pointer->next = pos.node_pointer->next;
            delete_node(pos.node_pointer);
        }
    }
    // 交换两个链表的指针即可
     void swap( forward_list& f_list) {
         std::swap(node_pointer,f_list.node_pointer );
     }

};


} // namespace MySTL


#endif // FORWARD_LIST_H
