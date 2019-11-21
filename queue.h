#ifndef QUEUE_H
#define QUEUE_H

#include "deque.h"


namespace MySTL {

template<class T,class Container = deque<T> >
class queue
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
    queue() = default;
    // 用容器的构造函数
    queue(const Container& cont):c(cont) {}
    queue(Container&& cont): c(std::move(cont)) {}
    // 拷贝构造与移动构造
    queue(const queue & other):c(other.c) {}
    queue(queue&& other): c(std::move(other.c)) {}

    queue& operator=(const queue& other) { c = other.c; return *this; }
    queue& operator=(queue&& other) { c = std::move(other.c); return *this; }


public:
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }
    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    reference front() { return c.front(); }
    const_reference front() const { return c.front();}
    reference back() { return c.back(); }
    const_reference back() const { return c.back(); }

    bool operator==(const queue& other) const { return c == other.c; }
    bool operator<(const queue& other) const { return c < other.c;}



};


}// namespace MySTL



#endif // QUEUE_H
