#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "hash_table.h"
#include <functional>


namespace MySTL{


template <class Pair>
struct select1st{
    const typename Pair::first_type operator() (const Pair x) const {return x.first; }
};

template <class Pair>
struct select2nd{
    const typename Pair::second_type operator() (const Pair x) const {return x.second; }
};


template <class Value,class Key,class HashFcn = std::hash<Key>,
              class EqualKey = std::equal_to<Key>,class Alloc = pool_allocator<Value>>
class hash_map {

private:
    using hash_t = hash_table<std::pair<const Key,Value>,Key,HashFcn,
          select1st<std::pair<const Key,Value>>,EqualKey,Alloc>;

    hash_t rep; // repository资料库，仓库
public:
    using key_type =typename hash_t::key_type;
    using data_type = Value;
    using mapped_type = Value;

    using iterator = typename hash_t::iterator;

    using hasher = typename hash_t::hasher;
    using key_equal = typename hash_t::key_equal;

    using value_type = typename hash_t::value_type;  // 类型为pair<...>
    using size_type = typename hash_t::size_type;
    using difference_type = typename hash_t::difference_type;
    using pointer = typename hash_t::pointer;
    using reference = typename hash_t::reference;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    hash_map() :rep(100,hasher(),key_equal()) { }
    explicit hash_map(size_type n):rep(n,hasher(),key_equal()) {}
    hash_map(size_type n,const hasher& hf):rep(n,hf,key_equal()) {}
    hash_map(size_type n,const hasher& hf,const key_equal& eql):rep(n,hf,eql) {}

public:
    Value& operator[](const key_type& key)
    { return rep.find_or_insert( value_type(key,Value())).second; }
    size_type size() const { return rep.size(); }
    bool empty()const { return rep.empty(); }
    // 暂未实现
    void swap(hash_map& hs);
    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }

public:
    std::pair<iterator,bool> insert(const value_type& x) { return rep.insert_unique(x); }
    iterator find(const key_type& key) const { return rep.find(key); }
    size_type count(const key_type& x) { return rep.count(x);}
    void clear() {return rep.clear();}
    // 暂未实现
    void erase(iterator pos);
private:
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count()const { return rep.bucket_count();}
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type index)const { return rep.elems_in_bucket(index);}


};


} // namespace MySTL

#endif // HASH_MAP_H
