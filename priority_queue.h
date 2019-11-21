/* 注：当给函数传递一个类类型的对象时，除了在常规作用域查找外，
 * 还会在实参类所属的命名空间，这有时候会导致ambiguous错误。
 * 如当前文件中的pop push 函数都应该加上作用域前缀。
 */


#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "pool_allocator.h"
#include "vector.h"
#include "heap_algorithm.h"

namespace MySTL
{
template<class T,class Container = vector<T>, class Compare = std::less<typename Container::value_type>>
class priority_queue
{
public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;

private:
    Container c;
    Compare comp;
public:
    priority_queue() = default;
    explicit priority_queue(const Compare& x): c(),comp(x) {}

    template <class InputIterator>
    priority_queue(InputIterator first,InputIterator last,const Compare& x):c(first,last),comp(x){ MySTL::make_heap(c.begin(),c.end(),comp); }

    template <class InputIterator>
    priority_queue(InputIterator first,InputIterator last):c(first,last),comp() { MySTL::make_heap(c.begin(),c.end(),comp); }



public:
    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    const_reference top() { return c.front(); }
    void push(const value_type& x) { c.push_back(x); MySTL::push_heap(c.begin(),c.end(),comp); }
    void pop() { MySTL::pop_heap(c.begin(),c.end(),comp); c.pop_back();}


};

}

#endif // PRIORITY_QUEUE_H
