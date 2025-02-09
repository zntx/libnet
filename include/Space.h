//
// Created by zz on 2025/1/27.
//

#ifndef EASYDARWIN_SPACE_H
#define EASYDARWIN_SPACE_H


#include "Option.h"
#include "Result.h"
#include "Slice.h"
template <typename T>
class Space {
public:
    T* addr;
    std::size_t data_size;
    std::size_t len;

    static Space<T> Create(std::size_t size) {
        T *addr = new(std::nothrow)  T[size];

        if (addr == nullptr) {
            return {nullptr, 0};
        }

        //printf(" Space new %p\n", addr );
        return Space<T>(addr, size);
    }

    ~Space() {
        //std::cout << "~Space() :"  << std::endl;
        if (this->addr != nullptr) {
            printf(" ~Space() : delete %p\n", this->addr);
            //std::cout << "~Space() : delete1"  << std::endl;
            delete[] this->addr;
            //std::cout << "~Space() : delete2"  << std::endl;
        }
    }

    Space(T* address, std::size_t size)  {
        addr = address;
        data_size = size;
        len = 0;
    }

    Space(T* address, std::size_t size, size_t len)  {
        addr = address;
        data_size = size;
        len = len;
    }

    Space( Space<T> &other) = delete;
    Space(const Space<T> &other) = delete;
    // 移动构造
    Space(Space<T> &&other)  {
        //printf("Slice1 移动构造\n");
        this->addr = other.addr;
        this->len  = other.len;
        this->data_size = other.data_size;

        other.addr = nullptr;
        other.len = 0;
        other.data_size = 0;
    }

    //复制赋值运算符
    Space& operator=(Space<T> &other) = delete;

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Space& operator= (Space<T> &&other) noexcept     {
        this->addr = other.addr;
        this->len = other.len;
        this->data_size = other.data_size;

        other.addr = nullptr;
        other.len = 0;
        other.data_size = 0;

        return *this;
    }

    std::size_t size(){
        return this->data_size;
    }

    bool is_legal() {
        return this->addr != nullptr && this->data_size > 0;
    }

    bool no_legal() {
        return this->addr == nullptr || this->data_size <= 0;
    }

    Option<Slice<T>> to_slice(size_t start, size_t end = 0)
    {
        if ( end == 0)
            end = this->data_size;

        if ( start >= this->data_size)
            return None();

        if ( end >= this->data_size)
            return None();

        if ( end <= start)
            return None();

        return Some(Slice<T>(this->addr + start, end - start));
    }
//    bool operator==( Space<T> other) {
//        if(this->addr != other.size())
//            return false;
//
//        for( size_t index = 0; index < other.size(); index++ )
//            if(this->at(index) != other.at(index) )
//                return false;
//
//        return true;
//    }

    /*   */
    Option<T> at(size_t index) {
        if( index >= this->len)
            return None();
        return Some(*(this->addr + index));
    }

    bool set(size_t index, T &ch) {
        if (index >= this->data_size)
            return false;

        ch = *(this->addr + index);
        return true;
    }

    bool set(size_t index, T ch) {
        if( index <= this->data_size ) {
            *( this->addr + index) = ch;
            return true;
        }

        return false;
    }

    Option<std::size_t> find( T ch) {
        for( std::size_t index = 0; index < this->len; index++) {
            if( *( this->addr + index) == ch)
                return Some(index);
        }
        return None();
    }

    std::size_t find_count( T ch) {
        std::size_t count = 0;
        for( std::size_t index = 0; index < this->len; index++) {
            if( *( this->addr + index) == ch)
                count++;
        }
        return count;
    }

    Option<std::size_t> find( Slice<T> ch) {
        std::size_t j = 0;
        for( std::size_t i = 0; i < this->len - ch.len; i++)
        {
            for(  j = 0; i < ch.len; j++)
            {
                if( *( this->addr + i + j) != *( ch.addr + j))
                    break;
            }

            if( j == ch.len)
                return Some(i);


        }
        return None();
    }

    Slice<T> data( ) {
        return  Slice<T>(this->addr, this->len);
    }

    std::size_t copy(Slice<T>& other) {
        auto i = 0;
        for( i = 0;   i< other.len ; i++) {
            if( this->len + 1 > this->data_size)
                break;

            *(this->addr + this->len ) = *(other.addr + i);

            this->len += 1;
        }

        return i ;
    }
};


template <typename T>
class RefSpace :public Space<T>{
private:
    // 自定义内存空间，用于存放Base子对象
    alignas(Space<T>) char base_mem[sizeof(Space<T>)];
public:
    RefSpace(Space<T>& other) {
        new (base_mem) Space<T>(other.addr, other.data_size, other.len);
    }

    ~RefSpace() {

    }

    RefSpace( RefSpace<T> &other) = default;
    RefSpace(const RefSpace<T> &other) = default;
    // 移动构造
    RefSpace(RefSpace<T> &&other) = default;
    //复制赋值运算符
    RefSpace& operator=(RefSpace<T> &other) = default;

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    RefSpace& operator= (RefSpace<T> &&other) = default;

};
#endif //EASYDARWIN_SPACE_H
