#ifndef LIST_H
#define LIST_H

// 这个文件是容器 list 的头文件
#include <iterator>
#include "pool_allocator.h"
#include "construct.h"
#include <memory>
#include <cstddef>
#include <type_traits>
#include <initializer_list>

namespace MySTL {

// 先定义 list 节点结构
template <class T>
struct _list_node{
    _list_node<T>*    prev;
    _list_node<T>*    next;
    T                 data;
};

// 定义 list 的迭代器，继承自iterator，内有5个迭代器应有的typedef
template <class T>
class _list_iterator
{
public:
    typedef     ptrdiff_t               difference_type;
    typedef     size_t                  size_type;
    typedef     _list_node<T>*         node_ptr;
    typedef     _list_iterator<T>       iterator;
    typedef     T&                      reference;
    typedef     std::bidirectional_iterator_tag     iterator_category;
    typedef     T*                      pointer;
    typedef     T                       value_type;

public:
    // 构造函数
    _list_iterator() {}
    _list_iterator(node_ptr& val) : ptr_data(val) {}
    _list_iterator(const iterator& val) : ptr_data(val.ptr_data) {}

    bool operator==(const iterator& val) const { return ptr_data == val.ptr_data; }
    bool operator!=(const iterator& val) const { return ptr_data != val.ptr_data; }

    // 成员访问运算符
    reference operator*() const { return ( ptr_data->data ); }
    reference operator->() const { return &(operator*() ); }

    // 自加
    iterator& operator++() { ptr_data = ptr_data->next; return *this; }
    iterator  operator++(int) { iterator temp = *this ; operator++(); return temp; }
    iterator& operator--() { ptr_data = ptr_data->prev; return *this; }
    iterator  operator--(int) { auto temp = *this; operator--(); return temp; }

public:
    node_ptr    ptr_data;

};


// list 数据结构
template <class T,class Alloc = pool_allocator<T> >
class list
{
public:
    typedef     T                          value_type;
    typedef     ptrdiff_t                  difference_type;
    typedef     _list_iterator<T>          iterator;
    typedef     size_t                     size_type;
    typedef     T&                         reference;
    typedef     const T&                   const_reference;
    typedef     T*                         pointer;
    typedef     T*                         const_pointer;
private:
    typedef     _list_node<T>             list_node;
    typedef     list_node*                 node_ptr;
    typedef  typename Alloc::template rebind<list_node>::other  data_allocator;

private:
    node_ptr        ptr_data; // 指向环形链表的空结点;

private:

    // 分配一个节点空间
    node_ptr get_node(){ return data_allocator::allocate();  }

    // 释放一个节点空间
    void free_node( node_ptr p) { data_allocator::deallocate(p); }

    // 分配空间构造节点
    node_ptr create_node(const T& x)
    {
        node_ptr p = get_node();
        construct(&p->data,x);
        return p;
    }
    // 析构节点中的数据并释放空间
    void destory_node(node_ptr p) { destory(&p->data); free_node(p); }

    // 初始化一个空结点
    void empty_initialize()
    {
        ptr_data = get_node();
        ptr_data->next = ptr_data;
        ptr_data->prev = ptr_data;
    }

    // 迁移操作
    void transfer( iterator position, iterator first , iterator last)
    {
        if ( position != last )
        {
            // 保存last前面节点的指针
            node_ptr temp = last.ptr_data->prev;
            // 处理from链表
            first.ptr_data->prev->next = last.ptr_data;
            last.ptr_data->prev = first.ptr_data->prev;
            // 处理to链表
            position.ptr_data->prev->next = first.ptr_data;
            first.ptr_data->prev = position.ptr_data->prev;
            temp->next = position.ptr_data;
            position.ptr_data->prev = temp;

        }
    }
public:
    // 构造函数
    list() { empty_initialize(); }

    // 使用两个迭代器的构造函数
    template <class Iterator>
    list(Iterator begin,Iterator end) {
        empty_initialize();
        for (;begin != end; ++begin )
        {
            push_back(*begin);
        }
    }
    // 使用initializer_list 的构造函数
    list(const std::initializer_list<value_type>& l) {
        empty_initialize();
        for (auto item : l)
        {
            push_back(item);
        }
    }

    bool empty()  { return 0 == size();}

    size_type size()  {
         auto result = std::distance(begin(),end());
                return result;
    }
    reference front() { return ptr_data->next->data ; }
    reference back() { return ptr_data->prev->data  ; }
    iterator begin()  { return ptr_data->next; }
    iterator end()   { return ptr_data; }

    void swap(list & x) {
        iterator temp = x.end();
        x.ptr_data = ptr_data;
        ptr_data = temp.ptr_data;
    }

    // 在position处插入元素
    iterator insert(iterator position,const T& val)
    {
        node_ptr temp = create_node(val);
        temp->next = position.ptr_data;
        temp->prev = position.ptr_data->prev;
        position.ptr_data->prev->next = temp;
        position.ptr_data->prev = temp;
        return temp;
    }

    // 从尾部插入元素
    void push_back(const T& val ) { insert(end(),val); }
    // 从首部插入元素
    void push_front(const T& val) { insert(begin(),val);}
    // 清楚position位置的元素
    iterator erase(iterator position)
    {
        node_ptr temp = position.ptr_data;
        node_ptr result = position.ptr_data->next;
        position.ptr_data->prev->next = result;
        position.ptr_data->next->prev = temp->prev;
        destory_node(temp);
        return result;
    }

    // 从首部弹出一个元素
    T pop_front()
    {
        node_ptr temp = begin().ptr_data;
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        T result = temp->data;
        destory_node(temp);
        return result;
    }

    // 从尾部弹出一个元素
    T pop_back()
    {
        node_ptr temp = (--end()).ptr_data;
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        T result = temp->data;
        destory_node(temp);
        return result;
    }

    // 清楚所有元素
    void clear()
    {
        node_ptr cur = ptr_data->next;
        while( cur != ptr_data )
        {
            node_ptr temp = cur;
            cur = cur->next;
            destory_node(temp);
        }
        cur->next = cur;
        cur->prev = cur;
    }

    // 清楚容器中的所有value元素
    void remove(const T& val)
    {
        iterator first = begin();
        iterator last  = end();
        while ( first != last )
        {
            if( val == *first )
                first = erase(first);
            else
                ++first;
        }
    }

    //  删除连续重复的元素，只保留一个unique
    void unique()
    {
        iterator first = begin();
        iterator last = end();
        iterator next = first;
        while (first != last )
        {
             if ( *first == *(++next) )
                 first = erase(first);
             else
                 ++first;
        }
    }

    // 将某一条链表插入到position
    void splice(iterator position,list & x)
    { if( !x.empty() ) transfer(position,x.begin(),x.end());}
    // 将迭代器指向的元素插入到position之前
    void splice(iterator position,iterator i){
        iterator j = i;
        ++j;
        if(i != position && j != position )
            transfer(position,i,j);
    }
    void splice(iterator position, iterator first, iterator last ){
        if (first != last )
            transfer(position,first,last);
    }

    void reverse(){
        // 0,1个元素不用操作
        if( ptr_data->next != ptr_data || ptr_data->next->next != ptr_data)
        {
            iterator next = begin();
            ++next;
            iterator last = end();
            while ( next != last ){
                iterator temp = next;
                transfer(begin(),temp,++next);
            }
        }
    }
    // 归并排序法中的归并方法
    void merge(list& x)
    {
        iterator first= begin();
        iterator last = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();
        while( first != last && first2 != last2 )
        {
            if ( *first2 < *first ){
                iterator temp = first2;
                transfer(first,temp,++first2);
            }
            else
                ++first;
        }
        if ( first2 != last2 )
            transfer(end(),first2,last2);
    }

    void sort()
    {
        list carry;
        list counter[64];
        int fill = 0;
        while ( !empty() )
        {
            carry.splice(carry.begin(),begin());
            int i = 0;
            while ( i <fill && !counter[i].empty() ){
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill) ++fill;
        }
        for (int i =1; i< fill; ++i)
            counter[i].merge(counter[i-1]);
        swap(counter[fill-1]);
    }



};


} // namespace MySTL


#endif // LIST_H
