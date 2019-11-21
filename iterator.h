#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

namespace MySTL{

// 定义5种迭代器类别

struct input_iterator_tag{}; //  单向只读
struct output_iterator_tag {}; // 单向只写
struct forward_iterator_tag : public input_iterator_tag {}; // 单向可读可写
struct bidirectional_iterator_tag : public forward_iterator_tag {}; // 双向可读可写
struct random_access_iterator_tag : public bidirectional_iterator_tag {}; // 随机存取迭代器

// 迭代器萃取机制
template <class Iterator>
struct Iterator_traits{
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;

};

// Iterator_traits 针对原生指针的特化版本
template <class T>
struct Iterator_traits<T*>{
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// Iterator_traits 针对 const 原生指针的特化版本
template <class T>
struct Iterator_traits<const T*>{
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};


//  -------------- 三种 迭代器适配器 ----------------------- //

// back_insert_iterator
template <class Container>
class back_insert_iterator
{
protected:
    Container* container;
public:
    using iterator_category =  output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit back_insert_iterator(Container& x) : container(&x) {}
    back_insert_iterator& operator=(const typename Container::value_type& value)
    {
        container->push_back(value);
        return this;
    }

    back_insert_iterator& operator*() { return this;}
    back_insert_iterator& operator++() { return this; }
    back_insert_iterator& operator++(int) { return this;}

};
// back_insert_iterator 的辅助函数
template <class Container>
inline back_insert_iterator<Container> back_inserter(Container & x)
{
    return back_insert_iterator<Container>(x);
}

// front_insert_iterator
template <class Container>
class front_insert_iterator
{
protected:
    Container* container;
public:
    using iterator_category =  output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit front_insert_iterator(Container& x) : container(&x) {}
    front_insert_iterator& operator=(const typename Container::value_type& value)
    {
        container->push_front(value);
        return this;
    }

    front_insert_iterator& operator*() { return this;}
    front_insert_iterator& operator++() { return this; }
    front_insert_iterator& operator++(int) { return this;}

};
// front_insert_iterator 的辅助函数
template <class Container>
inline front_insert_iterator<Container> back_inserter(Container & x)
{
    return front_insert_iterator<Container>(x);
}

// insert_iterator
template <class Container>
class insert_iterator
{
protected:
    Container* container;
    typename Container::iterator iter;
public:
    using iterator_category =  output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit insert_iterator(Container& x,typename Container::iterator i) : container(&x),iter(i) {}
    insert_iterator& operator=(const typename Container::value_type& value)
    {
        iter = container->insert(iter,value);
        ++iter;
        return this;
    }

    insert_iterator& operator*() { return this;}
    insert_iterator& operator++() { return this; }
    insert_iterator& operator++(int) { return this;}

};
// insert_iterator 的辅助函数
template <class Container, class Iterator>
inline insert_iterator<Container> inserter(Container & x, Iterator i)
{
    using iter = typename Container::iterator;
    return front_insert_iterator<Container>(x, static_cast<iter>(i));
}

// --------------- 反向迭代器 ---------------- //
template <class Iterator>
class reverse_iterator
{
protected:
    Iterator current;
public:
    using value_type = typename Iterator_traits<Iterator>::value_type;
    using difference_type = typename Iterator_traits<Iterator>::difference_type;
    using iterator_category = typename Iterator_traits<Iterator>::iterator_category;
    using pointer = typename Iterator_traits<Iterator>::pointer;
    using reference= typename Iterator_traits<Iterator>::reference;


    using iterator_type = Iterator;
    using self = reverse_iterator <Iterator>;
    // 这个 默认构造函数 是否有存在的必要
    reverse_iterator() { }
    explicit reverse_iterator(iterator_type& x) : current(x) {}
    explicit reverse_iterator(const self& x) : current(x.current) {}

    iterator_type base() const { return current; }
    reference operator*() const {  Iterator tmp = current; return *--tmp;   }
    pointer operator->() const { return &(operator*() ); }

    self& operator++() { --current; return this; }
    self& operator++(int) { self tmp = *this; --current; return tmp; }
    self& operator--() { ++current; return this; }
    self& operator--(int) { self tmp = *this; ++current; return tmp; }

    self& operator+(difference_type n ) const  { return self(current - n ); }
    self& operator+=(difference_type n ) const { current-=n; return *this; }


    self& operator-(difference_type n ) const  { return self(current + n ); }
    self& operator-=(difference_type n ) const { current += n; return *this; }

    reference operator[] (difference_type n ) const { return *(*this+n);}
};


} // end of namespace MySTL

#endif // ITERATOR_H
