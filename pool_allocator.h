// 这个文件以内存池的形式来分配内存

#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "construct.h"  // for construct() and destory()
#include <cstddef>      // for size_t
#include <iostream>     // for cerr
#include <climits>      // for UINT_MAX

namespace MySTL{

enum { __ALIGN = 8 };                               // 调整边界
enum { __MAX_BYTES = 128 };                         // 最大上界
enum { __NUM_FREE_LIST = __MAX_BYTES/__ALIGN };     // 链表个数

class alloc
{
private:
    // 定义内嵌指针以形成链表
    struct obj{
        obj* next;
    };

    // 调整分配的字节数到8的倍数
    static size_t Round_up(size_t bytes) { return ( (bytes + __ALIGN - 1 ) & ~(__ALIGN - 1) ); }

    //计算所处的链表索引
    static size_t Freelist_index(size_t bytes) {
        return (bytes + __ALIGN - 1)/__ALIGN - 1;
    }

private:
    // 挂载内存池中的内存到free_list
    static void* refill(size_t n);

    // 分配内存到内存池
    static char* chunk_alloc(size_t size, int &nobjs );

private:
    // 链表数组，分别管理一个链表，每个链表所连内存大小不同
    static obj* free_list[ __NUM_FREE_LIST ];

    static char* start_pool;     // 指向内存池头
    static char*   end_pool;     // 指向内存池尾
    static size_t heap_size;     // 已分配内存的累积量

public:
    static void * allocate(size_t n,const void* hint = 0);
    static void  deallocate(void *p, size_t n);


};

char * alloc::start_pool = nullptr;
char * alloc::end_pool   = nullptr;
size_t alloc::heap_size  = 0;
alloc::obj* alloc::free_list[ __NUM_FREE_LIST ] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void * alloc::allocate(size_t n,const void* hint)
{
    obj* result;
    obj** current_free_list = free_list + Freelist_index(n); // 当前负责分配内存的链表的指针

    if ( n > static_cast<size_t>( __MAX_BYTES ))
        return std::malloc(n);

    if (  nullptr  == *current_free_list )                    // 若该链表下为空
    {
        void* r = refill(Round_up(n));
        return r;
    }

    // 不为空
    result = *current_free_list;
    *current_free_list = result->next;
    return result;
}

void  alloc::deallocate(void *p, size_t n)
{
    if ( n > static_cast<size_t>(__MAX_BYTES) )
    {
        std::free(p);
        return;
    }
    obj * free = reinterpret_cast<obj*>( p );
    obj ** current_free_list = free_list + Freelist_index(n);
    free->next = *current_free_list;
    *current_free_list = free;
}

void* alloc::refill(size_t n)
{
    int nobjs = 20;
    char * chunk = chunk_alloc(n, nobjs );
    if ( 1 == nobjs )
        return chunk;
    obj** current_free_list = free_list + Freelist_index(n);

    obj* current_obj, * next_obj;

    *current_free_list = current_obj = reinterpret_cast<obj*>(chunk + n);

    for ( int i = 0; i != nobjs-1; ++i )
    {
        if ( i == 18 )
            current_obj->next = nullptr;
        else
        {
            next_obj = reinterpret_cast<obj*>( reinterpret_cast<char*>(current_obj) + n) ;
            current_obj->next = next_obj;
            current_obj = next_obj;
        }
    }
    return chunk;
}

char* alloc::chunk_alloc(size_t size, int &nobjs)
{
    size_t total_bytes = size * nobjs;
    size_t pool_left_bytes = end_pool - start_pool;
    char* result;

    if ( pool_left_bytes > total_bytes )
    {
        result = start_pool;
        start_pool += total_bytes;
        return result;
    }
    else if ( pool_left_bytes >= size )
    {
        result = start_pool;
        nobjs = pool_left_bytes/size;
        total_bytes = size * nobjs;
        start_pool += total_bytes;
        return result;
    }
    else
    {
        if ( pool_left_bytes > 0 )
        {
            obj** current_free_list = free_list + Freelist_index(pool_left_bytes);
            (reinterpret_cast<obj*>(start_pool))->next = *current_free_list;
            *current_free_list = reinterpret_cast<obj*>(start_pool);
        }
        // 索要的内存大小由下式决定
        size_t byte_to_get = 2 * total_bytes + Round_up(heap_size >> 4 );
        start_pool = static_cast<char*>(malloc(byte_to_get));

        if ( 0 == start_pool )
        {
            int i ;
            obj** current_free_list;
            obj* p;
            for (i = size +__ALIGN; i < __MAX_BYTES; i+= __ALIGN )
            {
                current_free_list = free_list + Freelist_index(i);
                p = *current_free_list;
                if ( p != nullptr )
                {
                    *current_free_list = p->next;
                    start_pool = reinterpret_cast<char*>(p);
                    end_pool = start_pool + i;
                    return chunk_alloc(size,nobjs);
                }
            }
            end_pool = nullptr;
            std::cerr <<"out of memory" << std::endl;
            throw std::bad_alloc();
        }

        heap_size += byte_to_get;
        end_pool = start_pool + byte_to_get;
        return chunk_alloc(size, nobjs);
     }
}

// 定义 allocator 的一个公有接口
template <class T, class Alloc = alloc >
class pool_allocator
{

public:
    typedef T             value_type;
    typedef T*            pointer;
    typedef const T*      const_pointer;
    typedef T&            reference;
    typedef const T&      const_reference;
    typedef size_t        size_type;
    typedef ptrdiff_t     ptrdiff_type;

public:
    static T* allocate(size_t n) { return 0 == n? 0 : reinterpret_cast<T*>( Alloc::allocate( n*sizeof(T) )) ; }
    static T* allocate() { return reinterpret_cast<T*>(  Alloc::allocate( sizeof(T) )); }
    static void deallocate(T* p, size_t n ) { if (0 != n) Alloc::deallocate( p, n*sizeof(T) ); }
    static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }

public:
    template <class U>
    struct rebind
    {
        typedef pool_allocator<U> other;
    };

public:
    static void construct(pointer p,const T& value) { MySTL::construct(p,value); }
    static void destory(pointer p) { MySTL::destory(p); }
    pointer address(reference x) { return static_cast<pointer>(&x); }
    const_pointer address(const_reference x) { return static_cast<const_pointer>(&x); }
    size_type max_size() const { return static_cast<size_type>( UINT_MAX/sizeof(T) ); }

};


} // end of namespace MySTL
#endif // POOL_ALLOCATOR_H
