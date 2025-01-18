//
// Created by zhangyuzhu8 on 2025/1/8.
//

#ifndef APP_CIRCULAR_H
#define APP_CIRCULAR_H

#include <iostream>
#include <vector>
#include "Slice.h"

template<typename T>
class Circular : public Slice<T> {
public:
    std::size_t read_index{0};
    std::size_t wirte_index{0};

    static Circular<T> Create(std::size_t size) {
        T *addr = new(std::nothrow)  T[size];

        if (addr == nullptr) {
            return {nullptr, 0};
        }

        return  Circular<T>(addr, size);
    }

    Circular(T *_addr, std::size_t size) : Slice<T>(_addr, size) {
        this->read_index = 0;
        this->wirte_index = 0;
    }

    Circular(Circular &&old) noexcept: Slice<T>(std::move(old)) {
        this->addr = old.addr;
        this->len = old.len;
        this->read_index = old.read_index;
        this->wirte_index = old.wirte_index;

        old.addr = nullptr;
        old.len = 0;
        old.read_index = 0;
        old.wirte_index = 0;
    }

    Circular(Circular const &) = default;

    Circular(Circular &) = default;

    ~Circular() {
        //std::cout << "~Circular() :"  << std::endl;
        if (this->addr != nullptr) {
            std::cout << "~Circular() : delete1" << std::endl;
            delete[] this->addr;
            //std::cout << "~Circular() : delete2"  << std::endl;
        }
    }

    size_t data_len() {
        if (this->wirte_index >= this->read_index)
            return this->wirte_index - this->read_index;
        else
            return this->wirte_index + this->len - this->read_index;
    }

    size_t sapce_len() {
        return this->len - this->data_len() - 1;
    }

    T *get_wptr() {
        return this->addr + this->wirte_index;
    }

    size_t get_wsize() {
        if (this->wirte_index >= this->read_index)
            return this->len - this->wirte_index + this->read_index;
        else
            //预留1个空余空间
            return this->read_index - this->wirte_index - 1;
    }

    Slice<T> get_wbuf() {
        auto size = this->wirte_index >= this->read_index ?
                    this->len - this->wirte_index : this->read_index - this->wirte_index - 1;

        return Slice<T>(this->addr + this->wirte_index, size);
    }

    Slice<T> get_next_wbuf() {
        auto size = this->wirte_index >= this->read_index ? read_index : 0;
        return Slice<T>(this->addr, size);
    }

    bool feed(std::size_t size) {
        if (size > this->sapce_len()) {
            return false;
        }

        this->wirte_index = (this->wirte_index + size) % this->len;

        return true;
    }

    bool put(T ch) {
        if (0 >= this->sapce_len()) {
            return false;
        }

        *(this->addr + this->wirte_index) = ch;

        this->wirte_index = (this->wirte_index + 1) % this->len;

        return true;
    }

    bool put(Slice<T> &slice) {
        if (slice.len > this->sapce_len()) {
            return false;
        }

        for (size_t index = 0; index < slice.len; index++)
            this->put(slice.at(index));

        return true;
    }

    T *get_rptr() {
        return this->addr + this->read_index;
    }

    size_t get_rsize() {
        if (this->wirte_index >= this->read_index)
            return this->wirte_index - this->read_index;
        else
            return this->len - this->read_index + this->wirte_index;
    }

    Slice<T> get_rbuf() {
        auto size = this->wirte_index >= this->read_index ? this->wirte_index - this->read_index : this->len -
                                                                                                   this->read_index;
        return Slice<T>(this->addr + this->read_index, size);
    }

    Slice<T> get_next_rbuf() {
        auto size = this->wirte_index >= this->read_index ? 0 : this->wirte_index;
        return Slice<T>(this->addr + this->read_index, size);
    }

    SliceTuple<T> get_read_buf() {
        Slice<T> a(nullptr, 0) ;
        Slice<T> b(nullptr, 0) ;

        if (this->wirte_index >= this->read_index) {
            a = Slice<T>(this->addr + this->read_index, this->wirte_index - this->read_index);
            b = Slice<T>(nullptr, 0 );
        } else {
            a = Slice<T>(this->addr + this->read_index, this->len - this->read_index);
            b = Slice<T>(this->addr, this->wirte_index);
        }

        return SliceTuple<T>{ a, b };
    }

    bool drain(std::size_t size) {
        if (size > this->get_rsize()) {
            // not enough data
            return false;
        }
        this->read_index = (this->read_index + size) % this->read_index;

        return true;
    }

    bool take(T &data) {
        if (this->get_rsize() <= 0) {
            // not enough data
            return false;
        }

        data = *(this->addr + this->read_index);

        this->read_index = (this->read_index + 1) % this->read_index;

        return true;
    }


    bool take(Slice<T> &data) {
        if (this->get_rbuf() > data.len) {
            return false;
        }

        while (1) {
            auto slice = this->get_rbuf();
            if (slice.no_legal()) {
                break;
            }

            for (size_t index = 0; index < slice.len; index++)
                data.set(slice.at(index));
            this->read_index = (this->read_index + slice.len) % this->read_index;
        }

        return true;
    }

    void clear() {
        this->data_size = 0;
        this->read_index = 0;
        this->wirte_index = 0;
    }

    std::string prin_info() {
        std::string ss("Circular : ");

        //ss += std::string("addr ");       ss += std::string((char*)this->addr);
        ss += std::string(" len ");
        ss += std::to_string(this->len);
        //ss += std::string(" data_size ");  ss += std::to_string(data_size);
        ss += std::string(" read_index ");
        ss += std::to_string(read_index);
        ss += std::string(" wirte_index ");
        ss += std::to_string(wirte_index);

        return ss;
    }
};


template<typename T>
class RerfCircular : public Circular<T> {
private:
    RerfCircular() : Circular<T>(nullptr, 0){  }
    // 注意函数的第一个参数和返回值都是固定的
    void* operator new(size_t size) = delete;
    void* operator new[](size_t size) = delete;
    void operator delete(void* ptr){ } // 重载了new就需要重载delete

public:
    static RerfCircular<T> From(Circular<T> &circular) {
        auto ref = RerfCircular<T>();
        ref.addr = circular.addr;
        ref.len = circular.len;
        ref.read_index = circular.read_index;
        ref.wirte_index = circular.wirte_index;

        return ref;
    }

    RerfCircular(RerfCircular const &) = default;

    RerfCircular(RerfCircular &) = default;

    ~RerfCircular() = default;

};
#endif //APP_CIRCULAR_H
