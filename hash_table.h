#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#include <iterator>                // for std::forward_iterator_tag;
#include "vector.h"                 // for vector;
#include "pool_allocator.h"    // for pool_allocator;



/* 采用vector + 链表构建哈希表 */

namespace MySTL {

template<class T>
class identity
{
public:
    const T  operator()(const T value) const { return value; }
};


// 给哈希表准备的质数数组,大致以平方增大
static const int _num_primes = 28;
static const unsigned long _primer_arr[ _num_primes ] = {
    53,         97,           193,         389,        769,
    1543,       3079,         6151,        12289,      24593,
    49157,      98317,        196613,      393241,     786433,
    1572869,    3145739,      6291469,     12582917,   25165843,
    50331653,   100663319,    201326611,   402653189,  805306457,
    1610612741, 3221225473ul, 4294967291ul
};
// 返回大于 n 且最接近 n 的质数
inline unsigned long _next_prime(unsigned long n)
{
    for (int i = 0; i < _num_primes; ++i )
        if( _primer_arr[i] > n ) return _primer_arr[i];
    return 0;
}

// 定义 hash_table 的节点
template <class T>
struct _hash_table_node
{
    _hash_table_node* next;
    T data;
};


// 预定义 _hash_table_iterator
template <class Value,class Key, class HashFch, class ExtractKey, class EqualKey,class Alloc>
class _hash_table_iterator;



// 哈希表的定义
// Value 数据类型， Key 关键字类型， HashFch 哈希函数，
//ExtractKey 从数据类型中提取关键字的仿函数，EqualKey 比较关键字的仿函数
template <class Value,class Key, class HashFcn, class ExtractKey,
          class EqualKey,class Alloc = pool_allocator<Value>>
class hash_table
{
public:
    using node = _hash_table_node<Value>;
    using iterator  = _hash_table_iterator<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>;

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer   = Value*;
    using reference = Value&;
    using value_type = Value;

    using node_allocator = typename Alloc::template rebind<node>::other;

    using hasher = HashFcn;
    using key_equal = EqualKey;
    using key_type = Key;       // 关键字类型
private:
    // 以下为三个仿函数
    hasher hash;
    key_equal equals;
    ExtractKey get_key;
public:
    vector<node*> buckets;
    size_type num_elements; // 不用遍历就能得到元素的个数

public:
    reference find_or_insert(const value_type& x)
    {
        resize(num_elements + 1);
        size_type index = bkt_num(x);
        node* first = buckets[index];
        for (auto cur = first; cur; cur = cur->next)
            if (equals(get_key(cur->data),get_key(x)))
                return cur->data;
        node* tmp = create_node(x);
        tmp->next = first;
        buckets[index] = tmp;
        ++num_elements;
        return tmp->data;
    }

    hasher hash_funct() const { return hash; }
    key_equal key_eq() const { return equals; }
    void empty() const  { return num_elements == 0; }
    size_type size() const { return num_elements; }
    // 篮子个数
    size_type bucket_count() const { return buckets.size(); }
    // 最多有几个篮子数
    size_type max_bucket_count() const { return _primer_arr[_num_primes - 1];}
    iterator begin() {
        node * first;
        for (int i = 0; i < buckets.size(); ++i ){
            first = buckets[i];
            if ( first != nullptr )
                return iterator( first, this);
        }
        return end();
    }
    iterator end() { return iterator(nullptr,this);  }

    // 返回篮子中的数据个数
    size_type elems_in_bucket(size_type index) const
    {
        node* tmp = buckets[index];
        size_type num = 0;
        while ( tmp ){
            ++num;
            tmp = tmp->next;
        }
        return num;
    }
    iterator find(const key_type& x)
    {
        size_type index = bkt_num_key(x);
        node* tmp = buckets[index];
        while ( tmp != nullptr ){
            if ( tmp->data == x )
                return iterator(tmp,this);
            tmp = tmp->next;
        }
        return iterator(nullptr,this);
    }
    size_type count(const key_type& x)
    {
        size_type index = bkt_num_key(x);
        node* tmp = buckets[index];
        size_type num = 0;
        while ( tmp != nullptr){
            if ( tmp->data == x )
                ++num;
            tmp = tmp->next;
        }
        return num;
    }
    void clear()
    {
        for ( size_type i = 0; i < buckets.size(); ++i ){
            node* cur = buckets[i];
            while ( cur != nullptr ){
                node* tmp = cur;
                cur = cur->next;
                delete_node(tmp);
            }
            buckets[i] = nullptr;
        }
        num_elements = 0;
    }

    // 有bug,待修复
//    void copy_from(const hash_table& ht)
//    {
//        clear();
//        buckets.reserve(ht.buckets.size());
//        buckets.insert(buckets.end(),ht.buckets.size(),nullptr);

//        for (size_type i = 0; i < ht.buckets.size(); ++i ){
//            const node* cur = ht.buckets[i];
//            if ( cur ){
//                node* copy = create_node(cur->data);
//                buckets[i] = copy;
//                for ( node* next = cur->next; next; cur = next, next = cur->next ){
//                    copy->next = create_node(next->data);
//                    copy = copy->next;
//                }
//            }
//        }
//        num_elements = ht.num_elements;
//    }



private:
    // 新建节点
    node* create_node(const value_type & x)
    {
        node* p = node_allocator::allocate();
        p->next = nullptr;
        construct( &p->data , x);
        return p;
    }
    // 删除节点
    void delete_node(node* p)
    {
        destory(&p->data);
        node_allocator::deallocate(p);
    }
    // initialize_buckets 由构造函数调用，初始化篮子用
    void initialize_buckets(size_type n)
    {
        const size_type n_buckets = next_size(n);
        buckets.reserve( n_buckets );
        buckets.insert( buckets.end(),n_buckets, nullptr);
        num_elements = 0;
    }
    // 返回大于n的下一个质数
    size_type next_size(size_type n) const { return _next_prime(n); }
    // 无重复的插入-辅助函数
    std::pair<iterator,bool> insert_unique_aux(const value_type& x)
    {
        const size_type n = bkt_num(x);
        node* first = buckets[n];
        for ( node* cur = first; cur; cur = cur->next)
            if ( equals(get_key( cur->data ), get_key(x) ) )
                return std::pair<iterator,bool>( iterator(cur,this), false );
        node* tmp = create_node(x);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return std::pair<iterator,bool>( iterator(tmp,this), true );
    }
    // 重复元素可插入-辅助函数
    std::pair<iterator,bool> insert_equal_aux(const value_type& x)
    {
        const size_type n = bkt_num(x);
        node* first = buckets[n];
        for ( node* cur = first; cur != nullptr ; cur = cur->next)
            if ( equals( get_key( cur->data), get_key(x) ) ){
                node* tmp = create_node(x);
                tmp->next = cur->next;
                cur->next = tmp;
                ++num_elements;
                return std::pair<iterator,bool>(iterator(tmp,this),true);
            }
        // 否则需要插在链表头
        node* tmp = create_node(x);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return std::pair<iterator,bool>(iterator(tmp,this),true);
    }
public:
    // 接受数据和篮子个数
    size_type bkt_num(const value_type& x, size_t n) const
    {
        return bkt_num_key( get_key(x),n);
    }
    // 只接受数据
    size_type bkt_num(const value_type& x) const
    {
        return bkt_num_key( get_key(x) );
    }
    // 接受关键字
    size_type bkt_num_key(const key_type& key) const
    {
        return bkt_num_key(key,buckets.size());
    }
    // 接受关键字和篮子个数
    size_type bkt_num_key(const key_type& key ,size_t n) const
    {
        return hash(key)%n;
    }
public:
    // 构造函数
    hash_table(size_type n,const HashFcn& hf, const EqualKey& eql)
        :hash(hf),equals(eql),get_key( ExtractKey() ),num_elements(0)
    { initialize_buckets(n); }

    // 析构函数
    hash_table(){ clear(); }

    // 不允许重复元素插入
    std::pair<iterator,bool> insert_unique(const value_type& x)
    {
        resize( num_elements + 1); // 判断是否需要重建表格
        return insert_unique_aux(x);
    }
    // 允许重复元素插入
    std::pair<iterator,bool> insert_equal(const value_type& x)
    {
        resize(num_elements + 1);
        return insert_equal_aux(x);
    }
    void resize(const size_type n)
    {
        const size_type old_num = buckets.size();

        if ( n > old_num ){
            const size_type new_num = next_size(n);
            if ( new_num > old_num ){
                vector<node*> tmp( new_num, (node*) 0);

                for(size_type bucket = 0; bucket < old_num; ++bucket)
                {
                    node* first = buckets[bucket];

                    while( first  ){
                        size_type new_bucket = bkt_num(first->data,new_num);
                        buckets[bucket] = first->next;
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);
            }
        }
    }



};

// 定义哈希表的迭代器
// Value 数据类型， Key 关键字类型， HashFch 哈希函数，
//ExtractKey 从数据类型中提取关键字的仿函数，EqualKey 比较关键字的仿函数
template <class Value,class Key, class HashFcn, class ExtractKey, class EqualKey,class Alloc>
class _hash_table_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Value;
    using difference_type = ptrdiff_t;
    using pointer = Value*;
    using reference = Value&;
    using size_type = size_t;

    using node = _hash_table_node<Value>;
    using iterator = _hash_table_iterator;
    using hashtable = hash_table<Value,Key,HashFcn,ExtractKey,EqualKey,Alloc>;

public:
    node* cur;
    hashtable* ht;

public:
    // 构造函数
    _hash_table_iterator() = default;
    _hash_table_iterator(node* node_ptr, hashtable* ht_ptr): cur(node_ptr),ht(ht_ptr) {}
    // 析构函数


    value_type operator*() { return cur->data; }
    iterator operator++()
    {
        const node* old = cur;
        cur = cur->next;
        if ( !cur )
        {
           size_type bucket = ht->bkt_num(old->data); // 当前的数组索引
           while ( !cur && ++bucket < ht->buckets.size() ) // 若为空,则一直向后寻找
               cur = ht->buckets[bucket];
        }
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp = *this;
        operator++();
        return tmp;
    }
    bool operator==( const iterator& x) const { return cur == x.cur; }
    bool operator!=( const iterator& x) const { return cur != x.cur; }
};



} // namespace MySTL

#endif // HASH_TABLE_H
