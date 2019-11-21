#ifndef HASH_MULTISET_H
#define HASH_MULTISET_H


#include "hash_table.h"

namespace MySTL{

template <class Value,class HashFcn = std::hash<Value>,
          class EqualKey = std::equal_to<Value>,class Alloc = pool_allocator<Value>>
class hash_multiset
{
private:
    using hash_t = hash_table<Value,Value,HashFcn,identity<Value>,EqualKey,Alloc>;

    hash_t rep; // repository资料库，仓库
public:
    using key_type =typename hash_t::key_type;
    using iterator = typename hash_t::iterator;

    using hasher = typename hash_t::hasher;
    using key_equal = typename hash_t::key_equal;

    using value_type = typename hash_t::value_type;
    using size_type = typename hash_t::size_type;
    using difference_type = typename hash_t::difference_type;
    using pointer = typename hash_t::pointer;
    using reference = typename hash_t::reference;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    hash_multiset() :rep(100,hasher(),key_equal()) { }
    explicit hash_multiset(size_type n):rep(n,hasher(),key_equal()) {}
    hash_multiset(size_type n,const hasher& hf):rep(n,hf,key_equal()) {}
    hash_multiset(size_type n,const hasher& hf,const key_equal& eql):rep(n,hf,eql) {}

public:
    size_type size() { return rep.size(); }
    bool empty() { return rep.empty(); }
    void swap(hash_multiset& hs);
    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }

public:
    std::pair<iterator,bool> insert(const value_type& x) { return rep.insert_equal(x); }
    iterator find(const key_type& key) const { return rep.find(key); }
    size_type count(const key_type& x) { return rep.count(x);}
    void clear() {return rep.clear();}
    void erase(iterator pos);
private:
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() { return rep.bucket_count();}
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type index)const { return rep.elems_in_bucket(index);}


};

}

#endif // HASH_MULTISET_H
