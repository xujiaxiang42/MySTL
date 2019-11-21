// 待完成

#ifndef RB_TREE_H
#define RB_TREE_H

#include "pool_allocator.h"
#include "construct.h"


namespace MySTL{

// 定义节点的颜色类型
using Color_type = bool;
const Color_type Red = 0;
const Color_type Black = 1;

// 先定义节点类型
template <class Value>
struct rb_node
{
    using node_pointer = rb_node*;
    node_pointer    parent;
    node_pointer    lchild;
    node_pointer    rchild;
    Value           value;
    Color_type      color;

    static node_pointer min(node_pointer x) { while ( x->lchild != 0) x = x->lchild; return x;}
    static node_pointer max(node_pointer x) { while ( x->rchild != 0) x = x->rchild; return x;}
};

template <class Value>
struct rb_iterator
{
    using value_type = Value;
    using reference = Value&;
    using pointer = Value*;
    using iterator = rb_iterator;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    // using size_type = size_t;

    using node_pointer = rb_node<Value>*;

public:
    node_pointer node;

public:
    rb_iterator() = default;
    rb_iterator(const iterator& it) { node = it.node; }


public:
    iterator& operator++(){
        if ( node->rchild != nullptr )
        {
            node = node->rchild;
            while ( node->lchild != nullptr )
                node = node->lchild;
        }
        else{
            node_pointer tmp = node->parent;
            while ( node == tmp->rchild ){
                node = tmp;
                tmp = tmp->parent;
            }
            if ( node->rchild != tmp)
                node = tmp;
        }
        return *this;
    }

    iterator operator++(int){
        iterator tmp = *this;
        operator++();
        return tmp;
    }

    iterator& operator--(){
        if (node->color == Red && node->parent->parent == node)
            node = node->rchild;
        else if (node->lchild != nullptr)
        {
            node = node->lchild;
            while (node->rchild != nullptr)
                node = node->rchild;
        }
        else{
            node_pointer tmp = node->parent;
            while ( node == tmp->lchild)
            {
                node = tmp;
                tmp = tmp->parent;
            }
            node = tmp;
        }
        return *this;
    }

    iterator operator--(int){
        iterator tmp = *this;
        operator--();
        return tmp;
    }

    Value& operator*(){
        return node->value;
    }

    Value& operator->(){
        return &(operator*());
    }

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }


};

// 定义rb_tree
template <class Key,class Value,class KeyofValue,class Compare,class Alloc = pool_allocator<Value> >
class rb_tree
{
public:
    using void_pointer = void*;

    using iterator = rb_iterator<Value>;
    using pointer = Value*;
    using const_pointer = const Value*;
    using reference = Value&;
    using const_reference = const Value&;

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = Value;

    using color = Color_type;

    using node = rb_node<Value>;
    using node_pointer = node*;
    using node_allocator = typename Alloc::rebind<node>::other;

    node_pointer create_node(const value_type& x) {
        node_pointer tmp = node_allocator::allocate();
        construct(&tmp->data,x);
        return tmp;
    }
    void free_node(node_pointer p){
        destory(&p->data);
        node_allocator::deallocate(p);
    }

    node_pointer clone_node(node_pointer& x)
    {
        node_pointer tmp = create_node( x->value );
        tmp->color = x->color;
        tmp->lchild = nullptr;
        tmp->rchild = nullptr;
        return tmp;
    }

    // 数据内容
protected:
    size_type node_count;
    node_pointer header;
    Compare key_compare;

public:
    node_pointer& root()      const { return header->parent; }
    node_pointer& leftmost()  const { return header->lchild; }
    node_pointer& rightmost() const { return header->rchild; }

    static node_pointer& left(node_pointer x){
        return x->lchild;
    }
    static node_pointer& right(node_pointer x){
        return x->rchild;
    }
    static node_pointer& parent(node_pointer x){
        return x->parent;
    }
    static reference value(node_pointer x){
        return x->value;
    }
    static const Key& key(node_pointer x){
        return KeyofValue()(value(x));
    }
    static Color_type& color(node_pointer x){
        return (x->color);
    }

    static node_pointer minimun(node_pointer x){
        return rb_node::min(x)
    }
    static node_pointer minimun(node_pointer x){
        return rb_node::max(x);
    }

private:
    iterator _insert();

    // 容器初始化
    void init(){
        header = node_allocator::allocate();
        color(header) = Red;
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }
public:
    rb_tree(const Compare& comp = Compare()): node_count(0),key_compare(comp) { init(); }
    ~rb_tree(){ clear(); node_allocator::deallocate(header); }

    rb_tree operator=(const rb_tree& x);

public:
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    iterator end() { return rightmost(); }
    bool empty() { return node_count == 0; }
    size_type size() { return node_count; }
    size_type max_size() { return static_cast<size_type>(-1); }

public:
    std::pair<iterator,bool> insert_unique(const value_type& v)
    {
        node_pointer y = header;
        node_pointer x = root();
        bool comp = true;
        while ( x!= 0 ){
            y = x;
            comp = key_compare(KeyofValue()(v),key(x));
            x = comp ? left(x),right(x);
        }
        iterator j = iterator(y);
    }
    iterator insert_eauql(const value_type& x);
};



}// namespace MySTL

#endif // RB_TREE_H
