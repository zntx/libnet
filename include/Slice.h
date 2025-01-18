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

    // 移动赋值运算符
    // 和复制赋值运算符的区别在于，其参数是右值引用
    Slice& operator= (Slice<T> &&slice)
    {
        //printf("Slice1 移动赋值运算符\n");
        this->addr = slice.addr;
        this->len  = slice.len;

        return *this;
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



template <typename T>
class Space : public Slice<T>{
public:
    std::size_t data_size{0};

    static Option<Space<T>> Create(std::size_t size){
        T* addr = new(std::nothrow)  T[size] ;

        if( addr == nullptr){
            return  None();
        }

        //printf(" Space new %p\n", addr );
        return Some( std::move(Space<T>( addr,size )));
    }

    ~Space(){
        //std::cout << "~Space() :"  << std::endl;
        if( this->addr != nullptr) {
            printf(" ~Space() : delete %p\n", this->addr );
            //std::cout << "~Space() : delete1"  << std::endl;
            delete[] this->addr;
            //std::cout << "~Space() : delete2"  << std::endl;
        }
    }

    Space( T* _addr, std::size_t size) : Slice<T>(_addr, size){
        this->data_size = 0;
    }

    Space( Space&& old) : Slice<T>(std::move(old)){
        this->addr = old.addr ;
        this->len = old.len;
        this->data_size = old.data_size;

        old.addr = nullptr ;
        old.len = 0;
    }

    Space(Space&) = default;
    Space(const Space&) = default;

    Option<Slice<T>> slice( std::size_t pos = 0){
        if( pos >= this->len)
            return None();
		
        return Some(std::move(Slice<T>( this->addr + pos,  data_size - pos )));
    }

    T* get_wptr( )
    {
        return this->addr + this->data_size;
    }

    Option<Slice<T>> get_wbuf(  ) {
        std::size_t wbuf_size = this->len - this->data_size;
        if( wbuf_size == 0)
        {
            return None();
        }
        
        return Some(std::move(Slice<T>(this->addr + this->wirte_index, wbuf_size)));
    }


    std::size_t getsize_w(){
        return  this->len - this->data_size;
    }

    bool feed( std::size_t size) {
        if (size > this->len - this->data_size)
        {
            return false;
        }

        this->data_size += size;
        return true;
    }

    bool put(T ch){
        if( this->len - this->data_size <= 0)
            return false;

        *( this->addr + this->data_size ) = ch;
        this->data_size += 1;
        return true;
    }

    void clear() {
        this->data_size = 0;
    }


    // bool copy(Slice<T> data) {

    // }
};


template <typename T>
class Circular : public Slice<T>{
public:
    std::size_t read_index{0};
    std::size_t wirte_index{0};
    std::size_t data_size{0};

    static Option<Circular<T>> Create(std::size_t size){
        T* addr = new(std::nothrow)  T[size] ;

        if( addr == nullptr){
            return  None();
        }

        return Some( std::move(Circular<T>( addr,size )));
    }

    static Circular<T> Clone(Circular& old){
        Circular<T> val(old.addr ,old.len);

        val.data_size  = old.data_size;
        val.read_index = old.read_index;
        val.wirte_index = old.wirte_index;
        return std::move(val);
    }

    ~Circular(){
        //std::cout << "~Circular() :"  << std::endl;
        if( this->addr != nullptr) {
            std::cout << "~Circular() : delete1"  << std::endl;
            delete[] this->addr;
            //std::cout << "~Circular() : delete2"  << std::endl;
        }
    }

    Circular( T* _addr, std::size_t size) : Slice<T>(_addr, size) {
        this->data_size = 0;
        this->read_index = 0;
        this->wirte_index = 0;
    }

    Circular( Circular&& old) : Slice<T>(std::move(old)) {
        this->addr = old.addr ;
        this->len = old.len;
        this->data_size = old.data_size;
        this->read_index = old.read_index;
        this->wirte_index = old.wirte_index;

        old.addr = nullptr ;
        old.len = 0;
        old.data_size = 0;
        old.read_index = 0;
        old.wirte_index = 0;
    }

    Circular(Circular&) = default;

    Option<Slice<T>> slice( std::size_t pos, std::size_t len = 0){
        if( pos >= this->len)
            return None();
        if( pos + len > this->len)
            return None();

        if(len == 0 )
            return Some(std::move(Slice<T>( this->addr + pos,  this->wirte_index - this->pos )));

        return Some(std::move(Slice<T>( this->addr + pos,  len )));
    }

    T* get_wptr( )
    {
        return this->addr + this->wirte_index;
    }

    Option<Slice<T>> get_wbuf(  ) {
        std::size_t wbuf_size = this->len - this->data_size;
        if( wbuf_size == 0)
        {
            return None();
        }
        if ( wbuf_size + this->wirte_index <= this->len)
        {
            return Some(std::move(Slice<T>(this->addr + this->wirte_index, wbuf_size)));
        }
        else
        {
            return Some(std::move(Slice<T>(this->addr + this->wirte_index, this->len - this->wirte_index)));
                   // Some(std::move(Slice<T>(this->addr , wbuf_size - (this->len - this->wirte_index))))};
        }
    }


//    std::pair<Option<Slice<T>>, Option<Slice<T>>> get_wbuf(  ) {
//        std::size_t wbuf_size = this->len - this->data_size;
//        if( wbuf_size == 0)
//        {
//            return {None(), None()};
//        }
//        if ( wbuf_size + this->wirte_index <= this->len)
//        {
//            return {Some(std::move(Slice<T>(this->addr + this->wirte_index, wbuf_size))), None()};
//        }
//        else
//        {
//            // bs_buf->nb_seg = 2;
//            // bs_buf->seg[0].seg_base = this->addr + this->wirte_index;
//            // bs_buf->seg[0].seg_len = this->len - this->wirte_index;
//            // bs_buf->seg[1].seg_base = this->addr;
//            // bs_buf->seg[1].seg_len = wbuf_size - bs_buf->seg[0].seg_len;
//            return {Some(std::move(Slice<T>(this->addr + this->wirte_index, this->len - this->wirte_index))),
//                    Some(std::move(Slice<T>(this->addr , wbuf_size - (this->len - this->wirte_index))))};
//        }
//    }

    std::size_t getsize_w(){
        return  this->len - this->data_size;
    }

    bool feed( std::size_t size) {
        if (size > this->len - this->data_size)
        {
            return false;
        }

        this->wirte_index += size;
        if (this->wirte_index >= this->len)
        {
            this->wirte_index -= this->len;
        }
        this->data_size += size;

        return true;
    }

    T* get_rptr() {
        return this->addr + this->read_index;
    }

    bool put(T ch){
        if( this->len - this->data_size <= 0)
            return false;

        *( this->addr + this->read_index ) = ch;
        this->feed(1);
        return true;
    }

    Option<Slice<T>> get_rbuf( )
    {
        if(  this->data_size  <= 0)
        {
            return None();
        }

        if (this->data_size + this->read_index <= this->len)
        {
            return Some( std::move(Slice<T>(this->addr + this->read_index, this->data_size)));
        }
        else
        {
            return Some( std::move(Slice<T>(this->addr + this->read_index, this->len - this->read_index)));
        }
    }

//    std::pair<Option<Slice<T>>, Option<Slice<T>>> get_rbuf( )
//    {
//        if(  this->data_size  <= 0)
//        {
//            return {None(),None()};
//        }
//
//        if (this->data_size + this->read_index <= this->len)
//        {
//            return { Some({this->addr + this->read_index, this->data_size}), None};
//        }
//        else
//        {
//            // bs_buf->nb_seg = 2;
//            // bs_buf->seg[0].seg_base = handle->buf + handle->ridx;
//            // bs_buf->seg[0].seg_len = handle->buf_size - handle->ridx;
//            // bs_buf->seg[1].seg_base = handle->buf;
//            // bs_buf->seg[1].seg_len = rbuf_size-bs_buf->seg[0].seg_len;
//
//            return { Some({this->addr + this->read_index, this->len - this->read_index}),
//                     Some({this->addr, this->data_size - (this->len - this->read_index)})};
//        }
//
//        return 0;
//    }


    std::size_t getsize_r( )
    {
        return this->data_size;
    }

    bool drain(  std::size_t size)
    {
        if (size > this->data_size)
        {
            // not enough data
            return false;
        }
        this->read_index += size;
        if (this->read_index >= this->len)
        {
            this->read_index -= this->len;
        }
        this->data_size -= size;
        return true;
    }

    Option<T> take_one(){
        if( this->data_size <= 0)
            return None();

        T ch(*(this->addr + this->read_index));
        this->drain(1);
        return Some(ch);
    }

    void clear() {
        this->data_size = 0;
        this->read_index = 0;
        this->wirte_index = 0;
    }

    std::string to_string() {
        std:: string ss("Circular : ");

        //ss += std::string("addr ");       ss += std::string((char*)this->addr);
        ss += std::string("len ");        ss += std::to_string(this->len);
        ss += std::string("data_size ");  ss += std::to_string(data_size);
        ss += std::string("read_index "); ss += std::to_string(read_index);
        ss += std::string("wirte_index ");ss += std::to_string(wirte_index);

        return ss;
    }
};


#endif