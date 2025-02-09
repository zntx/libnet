#ifndef SLICE_H
#define SLICE_H

//#include <atomic>
//#include <stddef.h>
//#include <stddef.h>

#include "Option.h"
#include "Result.h"
template <typename T>
class Slice {
public:
    T* addr;
    std::size_t len;
//    ~Slice() {
//        std::cout << "~Slice() :"  << std::endl;
//    }

    Slice(T* address, std::size_t size)
    {
        addr = address;
        len = size;
    }

    Slice(const Slice<T> &slice)
    {
        //printf("Slice1 拷贝构造\n");
        this->addr = slice.addr;
        this->len  = slice.len;
    }
    // 移动构造
    Slice(Slice<T> &&slice)
    {
        //printf("Slice1 移动构造\n");
        this->addr = slice.addr;
        this->len  = slice.len;
    }

    //复制赋值运算符
    Slice& operator=(Slice<T> &slice)
    noexcept {
        //printf("Slice1 移动赋值运算符\n");
        this->addr = slice.addr;
        this->len = slice.len;

        return *this;
    }

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Slice& operator= (Slice<T> &&slice)
    {
        //printf("Slice1 移动赋值运算符\n");
        this->addr = slice.addr;
        this->len  = slice.len;

        return *this;
    }

    bool no_legal() {
        return this->addr == nullptr || this->len <= 0;
    }

    bool operator==( Slice<T> other) {
        if(this->len != other.size())
            return false;

        for( size_t index = 0; index < other.size(); index++ )
            if(this->at(index) != other.at(index) )
                return false;

        return true;
    }

    T operator[](int32_t i)
    {
        if( i > this->len)
            throw "out of array";
        if( i < 0 )
            return this->addr[this->len + i ];

        return this->addr[i];
    }

    std::size_t size(){
        return this->len;
    }

    bool at(size_t index, T &ch) {
        if (index >= this->len)
            return false;

        ch = *(this->addr + index);
        return true;
    }

    /*   */
    T at(size_t index)
    {
        return *(this->addr + index);
    }

    bool set(size_t index, T ch)
    {
        if( index <= this->len )
        {
            *( this->addr + index) = ch;
            return true;
        }

        return false;
    }

    Option<std::size_t> find( T ch)
    {
        for( std::size_t index = 0; index < this->len; index++)
        {
            if( *( this->addr + index) == ch)
                return Some(index);
        }
        return None();
    }

    std::size_t find_count( T ch)
    {
        std::size_t count = 0;
        for( std::size_t index = 0; index < this->len; index++)
        {
            if( *( this->addr + index) == ch)
                count++;
        }
        return count;
    }

    Option<std::size_t> find( Slice<T> ch)
    {
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

    Option<Slice> slice(size_t start, size_t end)
    {
        if ( start >= this->len)
            return None();

        if ( end >= this->len)
            return None();

        if ( end <= start)
            return None();

        return Some(Slice(this->addr + start, end - start));
    }

    Option<Slice> slice(size_t pos )
    {
        if ( pos >= this->len)
            return None();

        return Some(Slice(this->addr + pos, this->len - pos));
    }

    std::string to_string()
    {
        return std::string((char*)this->addr, this->len);
    }


    std::size_t copy(Slice& other) {
        auto i = 0;
        for( i = 0; i< this->len && i< other.len; i++) {
            *(this->addr + i ) = *(other.addr + i);
        }

        return i;
    }
};

template<typename T>
class SliceTuple {
    Slice<T> aa;
    Slice<T> bb;
public:
    SliceTuple(Slice<T> &a, Slice<T> &b) : aa(a), bb(b){

    }

//    SliceTuple(Slice<T> a, Slice<T> b) {
//        aa = a;
//        bb = b;
//    }

    bool no_legal() {
        return aa.no_legal() ;
    }

    size_t size() {
        size_t len = 0;

        if (!aa.no_legal())
            len += aa.size();

        if (!bb.no_legal())
            len += bb.size();

        return len;
    }

    T at(size_t pos) {
        if (pos >= this->size()) {
            //std::
        }

        if (pos < aa.size()) {
            return aa.at(pos);
        } else {
            return bb.at(pos - aa.size());
        }
    }

    bool at(size_t pos, T &ch) {
        if (pos >= this->size()) {
            return false;
        }

        if (pos < aa.size()) {
            return aa.at(pos, ch);
        } else {
            return bb.at(pos - aa.size(), ch);
        }
    }

    size_t find(T ch) {
        size_t len = this->size();

        for (size_t index = 0; index < len; index++) {
            if (this->at(index) != ch) {
                return index;
            }
        }
        return len;
    }

    size_t find(Slice<T> &ch) {
        size_t len = this->size();

        for (size_t index = 0; index < len - ch.size(); index++) {
            size_t pos = 0;
            for (pos = 0; pos < ch.size(); pos++) {
                if (this->at(index + pos) != ch.at(pos)) {
                    break;
                }
            }
            if (pos == ch.size()) {
                return index;
            }
        }

        return len;
    }

    bool slice(Slice<T> &ch) {
        if (ch.len > this->size()) {
            return false;
        }

        for (size_t index = 0; index < ch.size(); index++) {
            ch.set(index, this->at(index)) ;
        }

        return true;
    }

};




#endif