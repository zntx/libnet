#ifndef ZSDT_INT_H
#define ZSDT_INT_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

template<typename T, std::size_t N>
bool Arrays_Compare(const T(&arr1)[N], const T(&arr2)[N]) {
    for (std::size_t i = 0; i < N; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    return true;
}

template<typename T, std::size_t N>
void Arrays_Copy( T(&arr1)[N], const T(&arr2)[N]) {
    for (std::size_t i = 0; i < N; i++) {
        arr1[i] = arr2[i] ;
    }
}


#if 0
// 短整型大小端互换

#define BigLittleSwap16(A)  ((((uint16_t)(A) & 0xff00) >> 8) |  (((uint16_t)(A) & 0x00ff) << 8))

 // 长整型大小端互换

 

#define BigLittleSwap32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \

                            (((uint32_t)(A) & 0x00ff0000) >> 8) | \

                            (((uint32_t)(A) & 0x0000ff00) << 8) | \

                            (((uint32_t)(A) & 0x000000ff) << 24))

#endif

inline bool is_big()
{
    union _u{
        int i;
        char c;
    }u;

    u.i = 1;
    if(u.c ==1){
        printf("sys Little endian!\n");
        return false;
    }else{
        printf("sys Big endian!\n");
        return true;
    }
}

struct u16
{
    uint16_t value;


    u16 ( uint16_t a)
    {
        value = a;
    }
    u16 from_be_bytes(uint8_t (&array)[2])
    {
        uint16_t a = array[0];
        uint16_t b = array[1];
        
        return from_be_bytes( a, b );
    }

    u16 from_be_bytes(uint8_t a, uint8_t b)
    {
        if( is_big()) 
        {
            uint16_t as = ( a << 8) | (b ) ;
            return  as ;
        }

        return ( a ) | ( b << 8) ;
    }

   
}; // namespace name


struct u32
{
    uint32_t value;
    
    u32 ( uint32_t a)
    {
        value = a;
    }

    static u32 from_be_bytes(uint8_t (&array)[4])
    {
        if( is_big()) 
        {
            return ( array[0] << 24) | ( array[1] << 16) | ( array[2] << 8) | ( array[3] );
        }

        return ( array[0] ) | ( array[1] << 8) | ( array[2] << 16) | ( array[3] << 24);
    }

    bool operator==(const uint32_t & other)
    {
        return  this->value == other;
    }

    bool operator==(const u32 & other)
    {
        return  this->value == other.value;
    }


}; // namespace name



struct u128
{
    uint64_t a;
    uint64_t b;  

    u128(  uint64_t _a, uint64_t _b )
    {
        a = _a;
        b = _b;
    } 

    static u128 from_be_bytes(uint8_t (&array)[16])
    {
        if( is_big()) 
        {
            uint64_t a =( static_cast<uint64_t>(array[0])  << 56 ) | 
						( static_cast<uint64_t>(array[1])  << 48 ) | 
						( static_cast<uint64_t>(array[2])  << 40 ) |
						( static_cast<uint64_t>(array[3])  << 32 ) |
						( static_cast<uint64_t>(array[4])  << 24 ) | 
						( static_cast<uint64_t>(array[5])  << 16 ) | 
						( static_cast<uint64_t>(array[6])  << 8  ) | 
                        ( array[7] );

            uint64_t b =( static_cast<uint64_t>(array[8 ])  << 56 ) | 
						( static_cast<uint64_t>(array[9 ])  << 48 ) | 
						( static_cast<uint64_t>(array[10])  << 40 ) |
						( static_cast<uint64_t>(array[11])  << 32 ) |
						( static_cast<uint64_t>(array[12])  << 24 ) | 
						( static_cast<uint64_t>(array[13])  << 16 ) | 
						( static_cast<uint64_t>(array[14])  << 8  ) | 
                        ( array[15] );
            
            return u128(a, b);
        }

        uint64_t a =( static_cast<uint64_t>(array[15])  << 56 ) | 
                    ( static_cast<uint64_t>(array[14])  << 48 ) | 
                    ( static_cast<uint64_t>(array[13])  << 40 ) |
                    ( static_cast<uint64_t>(array[12])  << 32 ) |
                    ( static_cast<uint64_t>(array[11])  << 24 ) | 
                    ( static_cast<uint64_t>(array[10])  << 16 ) | 
                    ( static_cast<uint64_t>(array[9 ])  << 8  ) | 
                    ( array[8] );

        uint64_t b =( static_cast<uint64_t>(array[7 ])  << 56 ) | 
                    ( static_cast<uint64_t>(array[6 ])  << 48 ) | 
                    ( static_cast<uint64_t>(array[5 ])  << 40 ) |
                    ( static_cast<uint64_t>(array[4 ])  << 32 ) |
                    ( static_cast<uint64_t>(array[3])  << 24 ) | 
                    ( static_cast<uint64_t>(array[2])  << 16 ) | 
                    ( static_cast<uint64_t>(array[1])  << 8  ) | 
                    ( array[9] );

        return u128(a, b);
    }

    bool operator==(const u128 & other)
    {
        return  this->a == other.a && this->b == other.b;
    }
};





#endif //ZSDT_INT_H
