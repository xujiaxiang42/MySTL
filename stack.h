#ifndef STACK_H
#define STACK_H

#include "deque.h"
#include <initializer_list>

/* 容器都应该定义的几种类型：value_type,pointer,
 * reference,size_type,difference_type
 * const_pointer,const_reference,iterator,
 * const_iterator,reserve_iterator
 * 容器适配器则不然
 */

namespace MySTL
{
template <class T,class Container = deque<T> >
class stack
{
public:
    using value_type = typename Container::value_type;
    using size_type  = typename Container::size_type;
    using reference  = typename Container::reference;
    using pointer    = typename Container::pointer;
    using const_pointer = typename Container::const_pointer;
    using const_reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;

 private:
    Container c;
public:
    // 构造函数
    stack() = default;
    // 使用容器初始化
    stack(Container& other): c(other) {}
    stack(Container&& other): c(std::move(other)) {}
    // 拷贝构造
    stack(const stack& other): c(other.c) { }
    // 移动构造
    stack(stack&& other): c(std::move(other.c)) {}

    // 使用一个 initializer_list 初始化为 栈
    stack(const std::initializer_list<value_type>& x): c(x) { }
    // 使用两个迭代器来初始化
    template <class Iterator>
    stack(Iterator begin,Iterator end) : c(begin,end) { }

public:
    void push(const value_type& x) { c.push_back(x); }
    value_type pop() { return c.pop_back(); }
    bool empty()  { return c.empty();}
    size_type size()  { return c.size(); }
    reference top() { return c.back(); }
    // const 容器 使用的 top
    const_reference top() const { return c.back() ;}
    bool operator==(const stack& rhs)const { return c == rhs.c; }
    bool operator<(const stack& rhs) const { return c < rhs.c ; }

};

}// MySTL

#endif // STACK_H
