#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include "zstdint.h"
#include "IpAddr.h"

/* IP地址合法性判�?宏定�?,�?针�?�IPv4地址 */
#define NET_SWAP_32(x)						((((x)&0xff000000) >> 24)\
												| (((x)&0x00ff0000) >> 8)\
												| (((x)&0x0000ff00) << 8)\
												| (((x)&0x000000ff) << 24))
#define IN_IS_ADDR_NETMASK(mask)			(!((~(NET_SWAP_32(mask)) + 1) & (~(NET_SWAP_32(mask)))))
#define IN_IS_ADDR_LOOPBACK(ip)				(((ip)&0xff) == 0x7F)				//127
#define IN_IS_ADDR_MULTICAST(ip)			(((ip)&0xf0) == 0xE0)				//224~239
#define IN_IS_ADDR_RESERVE_MULTICAST(ip)	((((ip)&0xff) == 0xE0)\
												&& ((((ip)&0x00ff0000) >> 16) == 0)\
												&& ((((ip)&0x0000ff00) >> 8) == 0)\
												&& ((((ip)&0xff000000) >> 24) <= 0x12))//保留的�?�播地址�?224.0.0.0~224.0.0.18
#define IN_IS_ADDR_BROADCAST(ip, mask)		(((ip)&(~(mask))) == (~(mask)))	//广播地址，即主机号全1
#define IN_IS_ADDR_NETADDR(ip, mask)		(!((ip)&(~(mask))))	//网络地址，即主机号全�?
#define IN_IS_ADDR_UNSPECIFIED(ip)			(!(ip))	//地址为全�?


Result<Ipv4Addr> Ipv4Addr::Create(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    if ( a > 255 || b> 255 || c >255 || d > 255)
    {
        return Err(std::string("is big ten 255"));
    }

    return Ok(Ipv4Addr(a, b, c, d));
}

Result<Ipv4Addr> Ipv4Addr::Create(std::string ips)
{
	return Create(Slice<const char>(ips.c_str(),ips.length()));
}

Result<Ipv4Addr> Ipv4Addr::Create(Slice<const char> ips)
{
    uint32_t  key = 0;
    uint32_t  spot = 0;
    for(std::size_t i = 0; i < ips.len; i++ )
    {
        if( ips.addr[i] == '.')
        {
            if( key >255)
            {
                return Err(std::string("is big ten 255"));
            }
            spot++;
            key = 0;
        }
        else
        {
            if( ips.addr[i] >= '0' && ips.addr[i] <= '9')
            {
                key = key *10 + ips.addr[i] - '0';

                if( key == 0 && ips.addr[i] >= '0')
                {
                    return Err(std::string("can not start whith 0"));
                }
            }
            else
            {
                return Err(std::string("not a number"));
            }
        }
    }

    if( spot != 3)
    {
        return Err(std::string("Delimiter not equal to 3"));
    }

    char ip_string[16] = {0};
    for(std::size_t i = 0; i < ips.len; i++ )
    {
        sprintf(ip_string + strlen(ip_string), "%c", *(ips.addr + i));
    }

    printf("ip_string %s\n", ip_string);
    struct in_addr ip_a;
    int ret = inet_pton(AF_INET, ip_string, &ip_a);
    if (ret <= 0)
    {
        return Err(std::string(StrError(Errno)));
    }

    return Ok(Ipv4Addr(ip_a));
}

Ipv4Addr::Ipv4Addr( )
{
    _octets[0] = 0;
    _octets[1] = 0;
    _octets[2] = 0;
    _octets[3] = 0;
}

Ipv4Addr::Ipv4Addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    _octets[0] = a;
    _octets[1] = b;
    _octets[2] = c;
    _octets[3] = d;
}

// Ipv4Addr::Ipv4Addr(uint32_t d)
// {
//     _octets[0] = (d & 0xff000000) >> 24;
//     _octets[1] = (d & 0x00ff0000) >> 16;
//     _octets[2] = (d & 0x0000ff00) >> 8;
//     _octets[3] = (d & 0x000000ff);
// }

Ipv4Addr::Ipv4Addr(const uint8_t (&array)[4])
{
    _octets[0] = array[0];
    _octets[1] = array[1];
    _octets[2] = array[2];
    _octets[3] = array[3];
}

Ipv4Addr::Ipv4Addr(struct in_addr _sin)
{
    sin.s_addr = _sin.s_addr;
}

Ipv4Addr::Ipv4Addr(struct in_addr *_sin)
{
    sin.s_addr = _sin->s_addr;
}

Ipv4Addr::Ipv4Addr( const Ipv4Addr &other)
{
    this->sin.s_addr = other.sin.s_addr;
}

Ipv4Addr::Ipv4Addr(Ipv4Addr &&other)
{
    this->sin.s_addr = other.sin.s_addr;
}

Ipv4Addr& Ipv4Addr::operator=(const Ipv4Addr& other)
{
    this->sin.s_addr = other.sin.s_addr;
    return *this;
}


bool Ipv4Addr::operator==(const Ipv4Addr &other)
{
    return Arrays_Compare(this->_octets, other._octets);;
}


uint8_t (&Ipv4Addr::octets())[4]
{
    return this->_octets;
}

bool Ipv4Addr::is_unspecified()
{
    return u32::from_be_bytes(this->_octets) == 0;
}

bool Ipv4Addr::is_loopback()
{
    return this->_octets[0] == 127;
}

bool Ipv4Addr::is_private()
{
    if (this->_octets[0] == 10)
    {
        return true;
    }

    if (this->_octets[0] == 172 && this->_octets[1] >= 16 && this->_octets[1] <= 31)
    {
        return true;
    }

    if (this->_octets[0] == 192 && this->_octets[1] >= 168)
    {
        return true;
    }

    return false;
}

bool Ipv4Addr::is_link_local()
{
    if (this->_octets[0] == 169 && this->_octets[1] >= 254)
    {
        return true;
    }

    return false;
}

bool Ipv4Addr::is_global()
{
    if (this->_octets[0] == 0 // "This network"
        || this->is_private() || this->is_shared() || this->is_loopback() || this->is_link_local()
        // addresses reserved for future protocols (`192.0.0.0/24`)
        || (this->_octets[0] == 192 && this->_octets[1] == 0 && this->_octets[2] == 0) || this->is_documentation() || this->is_benchmarking() || this->is_reserved() || this->is_broadcast())
        return false;
    return true;
}

bool Ipv4Addr::is_shared()
{
    return this->_octets[0] == 100 && ((this->_octets[1] & 0b11000000) == 0b01000000);
}

bool Ipv4Addr::is_benchmarking()
{
    return this->_octets[0] == 198 && (this->_octets[1] & 0xfe) == 18;
}

bool Ipv4Addr::is_reserved()
{
    return this->_octets[0] & 240 == 240 && !this->is_broadcast();
}

bool Ipv4Addr::is_multicast()
{
    return this->_octets[0] >= 224 && this->_octets[0] <= 239;
}

bool Ipv4Addr::is_broadcast()
{
    uint8_t (&arr)[4]  = Ipv4Addr_Static::BROADCAST.octets();
    return u32::from_be_bytes(this->_octets) == u32::from_be_bytes(arr);
}

bool Ipv4Addr::is_documentation()
{
    if (this->_octets[0] == 192 && this->_octets[1] == 0 && this->_octets[2] == 2)
        return true;
    if (this->_octets[0] == 198 && this->_octets[1] == 51 && this->_octets[2] == 100)
        return true;
    if (this->_octets[0] == 203 && this->_octets[1] == 0 && this->_octets[2] == 113)
        return true;

    return false;
}

Ipv6Addr Ipv4Addr::to_ipv6_compatible()
{
    return Ipv6Addr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, this->_octets[0], this->_octets[1], this->_octets[2], this->_octets[3]);
}

Ipv6Addr Ipv4Addr::to_ipv6_mapped()
{
    return Ipv6Addr(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, this->_octets[0], this->_octets[1], this->_octets[2], this->_octets[3]);
}

std::string Ipv4Addr::to_string()
{
    auto octets = this->_octets;
    return std::to_string(octets[0]) + "." + std::to_string(octets[1]) + "." + std::to_string(octets[2]) + "." + std::to_string(octets[3]);
}



namespace Ipv4Addr_Static {
    /// An IPv4 address with the address pointing to localhost: `127.0.0.1`
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// let addr = Ipv4Addr::LOCALHOST;
    /// assert_eq!(addr, Ipv4Addr::new(127, 0, 0, 1));
    /// ```
    // #[stable(feature = "ip_constructors", since = "1.30.0")]
    Ipv4Addr LOCALHOST(127, 0, 0, 1);

    /// An IPv4 address representing an unspecified address: `0.0.0.0`
    ///
    /// This corresponds to the constant `INADDR_ANY` in other languages.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// let addr = Ipv4Addr::UNSPECIFIED;
    /// assert_eq!(addr, Ipv4Addr::new(0, 0, 0, 0));
    /// ```
    // #[doc(alias = "INADDR_ANY")]
    // #[stable(feature = "ip_constructors", since = "1.30.0")]
    Ipv4Addr UNSPECIFIED(0, 0, 0, 0);

    /// An IPv4 address representing the broadcast address: `255.255.255.255`
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// let addr = Ipv4Addr::BROADCAST;
    /// assert_eq!(addr, Ipv4Addr::new(255, 255, 255, 255));
    /// ```
    // #[stable(feature = "ip_constructors", since = "1.30.0")]
    Ipv4Addr BROADCAST(255, 255, 255, 255);
}


/*

*****************************************************************

*/
namespace Ipv6Addr_Static
{
    /// An IPv6 address representing localhost: `::1`.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// let addr = Ipv6Addr::LOCALHOST;
    /// assert_eq!(addr, Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 1));
    /// ```
    // #[stable(feature = "ip_constructors", since = "1.30.0")]
    Ipv6Addr LOCALHOST(0, 0, 0, 0, 0, 0, 0, 1);

    /// An IPv6 address representing the unspecified address: `::`
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// let addr = Ipv6Addr::UNSPECIFIED;
    /// assert_eq!(addr, Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 0));
    /// ```
    // #[stable(feature = "ip_constructors", since = "1.30.0")]
    Ipv6Addr UNSPECIFIED(0, 0, 0, 0, 0, 0, 0, 0);

}

bool isHexDigit(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

Result<Ipv6Addr> Ipv6Addr::Create(Slice<const char> ips)
{
    // IPv6地址包含8组16位十六进制数，各组之间由冒号分隔
    // 可以有零压缩，即连续的冒号可以省略多组0
    int compresse = 0;
    int compressed_groups = 0;
    std::size_t number = 0;

    for (std::size_t index = 0; index < ips.len; index++)
    {
        if ( *(ips.addr + index) == ':')
        {
            if ( number > 4)
            {
                return Err(std::string("characters to match")); // 压缩区内的多余冒号
            }
            number = 0;

            compresse++;
            if (compresse == 1)
            {
                compressed_groups++;
            }
            if (compresse > 2)
            {
                return Err(std::string(": to match")); // 压缩区内的多余冒号
            }
        } else if (isHexDigit(*(ips.addr + index)))
        {
            compresse = 0;
            number++;
        } else
        {
            return Err(std::string("Illegal characters"));
        }
    }

    if ( number > 4)
    {
        return Err(std::string("characters to match")); // 压缩区内的多余冒号
    }

    // 必须有且仅有8组（考虑零压缩）
    if (compressed_groups > 8)
    {
        return Err(std::string("Illegal characters"));
    }

    struct in6_addr _sin;
    int ret = inet_pton(AF_INET6, ips.addr, &_sin);
    if (ret == 0 || Errno == EAFNOSUPPORT)
    {
        return Err(std::string(StrError(Errno)));
    }
	
	return Ok(Ipv6Addr(_sin));
}

Ipv6Addr::Ipv6Addr(struct in6_addr _sin)
{
    sin = _sin;
}


Ipv6Addr::Ipv6Addr(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e, uint16_t f, uint16_t g, uint16_t h)
{
    addr8[0] = a >> 8;
    addr8[1] = a & 0xff;
    addr8[2] = b >> 8;
    addr8[3] = b & 0xff;
    addr8[4] = c >> 8;
    addr8[5] = c & 0xff;
    addr8[6] = d >> 8;
    addr8[7] = d & 0xff;
    addr8[8] = e >> 8;
    addr8[9] = e & 0xff;
    addr8[10] = f >> 8;
    addr8[11] = f & 0xff;
    addr8[12] = g >> 8;
    addr8[13] = g & 0xff;
    addr8[14] = h >> 8;
    addr8[15] = h & 0xff;
}

Ipv6Addr::Ipv6Addr(uint8_t a1, uint8_t a2, uint8_t b1, uint8_t b2, uint8_t c1, uint8_t c2, uint8_t d1, uint8_t d2, uint8_t e1, uint8_t e2, uint8_t f1, uint8_t f2, uint8_t g1, uint8_t g2, uint8_t h1, uint8_t h2)
{
    addr8[0] = a1;
    addr8[1] = a2;
    addr8[2] = b1;
    addr8[3] = b2;
    addr8[4] = c1;
    addr8[5] = c2;
    addr8[6] = d1;
    addr8[7] = d2;
    addr8[8] = e1;
    addr8[9] = e2;
    addr8[10] = f1;
    addr8[11] = f2;
    addr8[12] = g1;
    addr8[13] = g2;
    addr8[14] = h1;
    addr8[15] = h2;
}

uint16_t Ipv6Addr::segments(uint32_t index)
{
    if (index > 7)
        return 0;
    uint8_t a =  this->addr8[index];
    uint8_t b = this->addr8[index + 1];
    //u16  tmp = u16::from_be_bytes( a, b);

    return a *256 + b;
}

bool Ipv6Addr::is_unspecified()
{
    // return u128::from_be_bytes(this->_octets) == u128::from_be_bytes(Ipv6Addr::UNSPECIFIED.octets());
    return Arrays_Compare(this->addr8, Ipv6Addr_Static::UNSPECIFIED.octets());
}

bool Ipv6Addr::is_loopback()
{
    // return u128::from_be_bytes(this->_octets) == u128::from_be_bytes(Ipv6Addr::LOCALHOST.octets());
    return Arrays_Compare(this->addr8, Ipv6Addr_Static::LOCALHOST.octets());
}

bool Ipv6Addr::is_global()
{
    return !(this->is_unspecified() || this->is_loopback()
        // IPv4-mapped Address (`::ffff:0:0/96`)
        || (this->segments(0) == 0 && this->segments(1) == 0 && this->segments(2) == 0 && this->segments(3) == 0 && this->segments(4) == 0 && this->segments(5) == 0xff)
        // IPv4-IPv6 Translat. (`64:ff9b:1::/48`)
        || (this->segments(0) == 64 && this->segments(1) == 0xff9b && this->segments(2) == 1)
        // Discard-Only Address Block (`100::/64`)
        //|| matches!(self.segments(), [0x100, 0, 0, 0, _, _, _, _])
        || (this->segments(0) == 0x100 && this->segments(1) == 0 && this->segments(2) == 0 && this->segments(3) == 0)
        // IETF Protocol Assignments (`2001::/23`)
        //|| (matches!(self.segments(), [0x2001, b, _, _, _, _, _, _] if b < 0x200)
        || ((this->segments(0) == 0x2001 && this->segments(1) < 0x200) 
            && !(
                // Port Control Protocol Anycast (`2001:1::1`)
                // u128::from_be_bytes(this->_octets) == 0x20010001000000000000000000000001
                Arrays_Compare(this->addr8, (uint8_t[]){0x20, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01})
                // Traversal Using Relays around NAT Anycast (`2001:1::2`)
                //|| u128::from_be_bytes(this->_octets) == 0x20010001000000000000000000000002
                || Arrays_Compare(this->addr8, (uint8_t[]){0x20, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02})
                // AMT (`2001:3::/32`)
                //|| matches!(self.segments(), [0x2001, 3, _, _, _, _, _, _])
                || (this->segments(0) == 0x2001 && this->segments(1) == 3)
                // AS112-v6 (`2001:4:112::/48`)
                //|| matches!(self.segments(), [0x2001, 4, 0x112, _, _, _, _, _])
                || (this->segments(0) == 0x2001 && this->segments(1) == 4 && this->segments(2) == 0x112)
                // ORCHIDv2 (`2001:20::/28`)
                //|| matches!(self.segments(), [0x2001, b, _, _, _, _, _, _] if b >= 0x20 && b <= 0x2F)
                || (this->segments(0) == 0x2001 && this->segments(1) >= 0x20 && this->segments(1) <= 0x2F)
                || (this->segments(0) == 0x2001 && this->segments(1) == 3)
            )
        ) 
    || this->is_documentation() 
    || this->is_unique_local() 
    || this->is_unicast_link_local()
    );
}

bool Ipv6Addr::is_unique_local()
{
    return (this->segments(0) & 0xfe00) == 0xfc00;
}

// #[inline]
bool Ipv6Addr::is_unicast()
{
    return !this->is_multicast();
}

bool Ipv6Addr::is_unicast_link_local()
{
    return (this->segments(0) & 0xffc0) == 0xfe80;
}

bool Ipv6Addr::is_documentation()
{
    return (this->segments(0) == 0x2001) && (this->segments(1) == 0xdb8);
}

bool Ipv6Addr::is_benchmarking()
{
    return (this->segments(0) == 0x2001) && (this->segments(1) == 0x2) && (this->segments(2) == 0);
}

bool Ipv6Addr::is_unicast_global()
{
    return this->is_unicast() && !this->is_loopback() && !this->is_unicast_link_local() && !this->is_unique_local() && !this->is_unspecified() && !this->is_documentation() && !this->is_benchmarking();
}

Option<Ipv6Addr::Ipv6MulticastScope> Ipv6Addr::multicast_scope()
{
    if (this->is_multicast())
    {
        uint16_t ret = this->segments(0) & 0x000f;
        switch (ret)
        {
        case 1:
            return Some(Ipv6Addr::Ipv6MulticastScope::InterfaceLocal);
            break;
        case 2:
            return Some(Ipv6Addr::Ipv6MulticastScope::LinkLocal);
            break;
        case 3:
            return Some(Ipv6Addr::Ipv6MulticastScope::RealmLocal);
            break;
        case 4:
            return Some(Ipv6Addr::Ipv6MulticastScope::AdminLocal);
            break;
        case 5:
            return Some(Ipv6Addr::Ipv6MulticastScope::SiteLocal);
            break;
        case 8:
            return Some(Ipv6Addr::Ipv6MulticastScope::OrganizationLocal);
            break;
        case 14:
            return Some(Ipv6Addr::Ipv6MulticastScope::Global);
            break;
        default:
            return None();
            break;
        }
    }
    else
    {
        return None();
    }
}

bool Ipv6Addr::is_multicast()
{
    return (this->segments(0) & 0xff00) == 0xff00;
}

Option<Ipv4Addr> Ipv6Addr::to_ipv4_mapped()
{
    if (this->addr8[0] == 0 && this->addr8[1] == 0 && this->addr8[2] == 0 &&
        this->addr8[3] == 0 && this->addr8[4] == 0 && this->addr8[5] == 0 &&
        this->addr8[6] == 0 && this->addr8[7] == 0 && this->addr8[8] == 0 &&
        this->addr8[9] == 0 && this->addr8[10] == 0xff && this->addr8[11] == 0xff)
    {
        Ipv4Addr ipv4(this->addr8[12], this->addr8[13], this->addr8[14], this->addr8[15]);
        return Some(ipv4);
    }

    return None();
}

/// Converts this address to an [`IPv4` address] if it is either
/// an [IPv4-compatible] address as defined in [IETF RFC 4291 section 2.5.5.1],
/// or an [IPv4-mapped] address as defined in [IETF RFC 4291 section 2.5.5.2],
/// otherwise returns [`None`].
///
/// Note that this will return an [`IPv4` address] for the IPv6 loopback address `::1`. Use
/// [`Ipv6Addr::to_ipv4_mapped`] to avoid this.
///
/// `::a.b.c.d` and `::ffff:a.b.c.d` become `a.b.c.d`. `::1` becomes `0.0.0.1`.
/// All addresses *not* starting with either all zeroes or `::ffff` will return `None`.
///
/// [`IPv4` address]: Ipv4Addr
/// [IPv4-compatible]: Ipv6Addr#ipv4-compatible-ipv6-addresses
/// [IPv4-mapped]: Ipv6Addr#ipv4-mapped-ipv6-addresses
/// [IETF RFC 4291 section 2.5.5.1]: https://tools.ietf.org/html/rfc4291#section-2.5.5.1
/// [IETF RFC 4291 section 2.5.5.2]: https://tools.ietf.org/html/rfc4291#section-2.5.5.2
///
/// # Examples
///
/// ```
/// use std::net::{Ipv4Addr, Ipv6Addr};
///
/// assert_eq!(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0).to_ipv4(), None);
/// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).to_ipv4(),
///            Some(Ipv4Addr::new(192, 10, 2, 255)));
/// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 1).to_ipv4(),
///            Some(Ipv4Addr::new(0, 0, 0, 1)));
/// ```
// #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
// #[stable(feature = "rust1", since = "1.0.0")]
// #[must_use = "this returns the result of the operation, \
    //               without modifying the original"]
// #[inline]
// Option<Ipv4Addr> to_ipv4(&self)
// {
//     if let [0, 0, 0, 0, 0, 0 | 0xffff, ab, cd] = self.segments() {
//         let [a, b] = ab.to_be_bytes();
//         let [c, d] = cd.to_be_bytes();
//         Some(Ipv4Addr::new(a, b, c, d))
//     } else {
//         None
//     }
// }

/// Converts this address to an `IpAddr::V4` if it is an IPv4-mapped addresses, otherwise it
/// returns self wrapped in an `IpAddr::V6`.
///
/// # Examples
///
/// ```
/// #![feature(ip)]
/// use std::net::Ipv6Addr;
///
/// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0x7f00, 0x1).is_loopback(), false);
/// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0x7f00, 0x1).to_canonical().is_loopback(), true);
/// ```
// #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
// #[unstable(feature = "ip", issue = "27709")]
// #[must_use = "this returns the result of the operation, \
    //               without modifying the original"]
// #[inline]
IpAddr Ipv6Addr::to_canonical()
{
    Option<Ipv4Addr> mapped = this->to_ipv4_mapped();
    if (!mapped.is_empty())
    {
        Ipv4Addr _mapped = mapped.unwrap();

        return IpAddr(_mapped);
    }
    return IpAddr(*this);
}

/// Returns the sixteen eight-bit integers the IPv6 address consists of.
///
/// ```
/// use std::net::Ipv6Addr;
///
/// assert_eq!(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0).octets(),
///            [255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
/// ```
// #[rustc_const_stable(feature = "const_ip_32", since = "1.32.0")]
// #[stable(feature = "ipv6_to_octets", since = "1.12.0")]
// #[must_use]
// #[inline]

uint8_t (&Ipv6Addr::octets())[16]
{
    return this->addr8;
}
//};

/// Write an Ipv6Addr, conforming to the canonical style described by
/// [RFC 5952](https://tools.ietf.org/html/rfc5952).
// #[stable(feature = "rust1", since = "1.0.0")]
// impl fmt::Display for Ipv6Addr {
std::string Ipv6Addr::to_string()
{
    // fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    //  If there are no alignment requirements, write the IP address directly to `f`.
    //  Otherwise, write it to a local buffer and then use `f.pad`.
    // if f.precision().is_none() && f.width().is_none() {
    // let segments = self.segments();

    // Special case for :: and ::1; otherwise they get written with the
    // IPv4 formatter
    if (this->is_unspecified())
    {
        return std::string("::");
    }
    else if (this->is_loopback())
    {
        return std::string("::1");
    }
    else
    {
#if 0
        Option<Ipv4Addr> ipv4 = this->to_ipv4();
        if (!ipv4.isEmpty())
        {
            Ipv4Addr _ipv4 = ipv4.unwrap();
            //match segments[5]
            {

                if (this->segments(5) == 0)
                    // IPv4 Compatible address
                    return "::{}" + ipv4.to_string();
                // IPv4 Mapped address
                else if (this->segments(5) == 0xff)
                    return "::ffff:{}" + ipv4.to_string();
                else
                // _ => unreachable!(),
            }
            else
            {
                std::stringstream ss;
                bool is_zero = false;
                for (int i = 0; i < 8; i++)
                {
                    ss << std::hex << this->segments(i);
                }

                return ss.str();
                // #[derive(Copy, Clone, Default)]
                //  struct Span {
                //      uint32_t start;
                //      uint32_t len ;
                //  };

                // Find the inner 0 span

                // Span longest ;
                // Span current ;

                // for (i, &segment) in segments.iter().enumerate() {
                //     if segment == 0 {
                //         if current.len == 0 {
                //             current.start = i;
                //         }

                //         current.len += 1;

                //         if current.len > longest.len {
                //             longest = current;
                //         }
                //     } else {
                //         current = Span::default();
                //     }
                // }

                // /// Write a colon-separated part of the address
                // //#[inline]
                // //fn fmt_subslice(f: &mut fmt::Formatter<'_>, chunk: &[u16]) -> fmt::Result {
                //     if let Some((first, tail)) = chunk.split_first() {
                //         write!(f, "{:x}", first)?;
                //         for segment in tail {
                //             f.write_char(':')?;
                //             write!(f, "{:x}", segment)?;
                //         }
                //     }
                //     Ok(())
                // }

                // if zeroes.len > 1 {
                //     fmt_subslice(f, &segments[..zeroes.start])?;
                //     f.write_str("::")?;
                //     fmt_subslice(f, &segments[zeroes.start + zeroes.len..])
                // } else {
                //     fmt_subslice(f, &segments)
                // }
            }
            // } else {
            //     const LONGEST_IPV6_ADDR: &str = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";

            //     let mut buf = DisplayBuffer::<{ LONGEST_IPV6_ADDR.len() }>::new();
            //     // Buffer is long enough for the longest possible IPv6 address, so this should never fail.
            //     write!(buf, "{}", self).unwrap();

            //     f.pad(buf.as_str())
            // }
        }
#endif
        char ip6text[INET6_ADDRSTRLEN] = {0};
        /* convert ip from binary to text  for IPv6  */
        const char* str = inet_ntop(AF_INET6, &this->sin, ip6text, sizeof(ip6text));
        if (str == NULL) {
            perror("inet_ntop error");
            return "";
        }

        return std::string(ip6text);
    }
}

Ipv6Addr::Ipv6Addr(const uint8_t (&arr)[16])
{
}

Ipv6Addr::Ipv6Addr(const uint16_t (&arr)[8])
{
}

bool Ipv6Addr::operator==(const Ipv6Addr &other)
{
    return Arrays_Compare(this->addr8, other.addr8);
}


/*******************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
********************************************************************/
// #[stable(feature = "rust1", since = "1.0.0")]
// impl fmt::Debug for Ipv6Addr {
//     fn fmt(&self, fmt: &mut fmt::Formatter<'_>) -> fmt::Result {
//         fmt::Display::fmt(self, fmt)
//     }
// }

// #[stable(feature = "ip_cmp", since = "1.16.0")]

Result<IpAddr> IpAddr::create(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    auto  ipv4 = Ipv4Addr::Create(a, b, c, d);
    if (ipv4.is_ok())
    {
        Ipv4Addr ips = ipv4.unwrap();

        return Ok( IpAddr(ips) );
    }

    return Err(ipv4.unwrap_err());
}

Result<IpAddr> IpAddr::Create(std::string host)
{
    return Create(Slice<const char>(host.c_str(),host.length()));
}

Result<IpAddr> IpAddr::Create(Slice<const char> host)
{
    if( host.find(':').is_empty() )
    {
        auto r_v4 = Ipv4Addr::Create(host);
        if (r_v4.is_ok())
        {
            Ipv4Addr v4 = r_v4.unwrap();
            return Ok(IpAddr(v4));
        }
        else
        {
            return Err(r_v4.unwrap_err());
        }
    }
    else
    {
        auto r_v6 = Ipv6Addr::Create(host);
        if (r_v6.is_ok())
        {
            Ipv6Addr v6 = r_v6.unwrap();
            return Ok(IpAddr(v6));
        }
        else
        {
            return Err(r_v6.unwrap_err());
        }
    }
}

IpAddr::IpAddr() :is_v4(true), sin4()
{
}

IpAddr::IpAddr(Ipv4Addr ipv4)
{
    this->is_v4 = true;
    this->sin4 = ipv4;
}

IpAddr::IpAddr(Ipv6Addr ipv6)
{
    this->is_v4 = false;
    this->sin6 = ipv6;
}

IpAddr::IpAddr(const uint8_t (&arr)[4])
{
    this->is_v4 = true;
    this->sin4 = Ipv4Addr(arr);
    //Arrays_Copy(this->V4._octets, arr);
}

IpAddr::IpAddr(const uint8_t (&arr)[16])
{
    this->is_v4 = false;
    this->sin6 = Ipv6Addr(arr);
}

IpAddr::IpAddr(const uint16_t (&arr)[8])
{
    this->is_v4 = false;
    this->sin6 = Ipv6Addr(arr);
}

IpAddr::IpAddr(const IpAddr &other)
{
    this->is_v4 = other.is_v4;

    if (this->is_v4)
    {
        this->sin4 = other.sin4;
    }
    else
    {
        this->sin6 = other.sin6;
    }
}
IpAddr::IpAddr(IpAddr&& other)
{
    this->is_v4 = other.is_v4;

    if (this->is_v4)
    {
        this->sin4 = other.sin4;
    }
    else
    {
        this->sin6 = other.sin6;
    }
}

bool IpAddr::operator==(const IpAddr &other)
{
    if (this->is_v4 != other.is_v4)
        return false;

    if (this->is_v4)
    {
        return this->V4 == other.V4;
    }
    else
    {
        return this->V6 == other.V6;
    }
}

bool IpAddr::operator==(const Ipv4Addr &other)
{
    if (this->is_ipv6())
        return false;

    return this->sin4 == other;
}

bool IpAddr::operator==(const Ipv6Addr &other)
{
    if (this->is_ipv4())
        return false;

    return this->sin6 == other;
}

bool IpAddr::is_unspecified()
{
    if (this->is_v4)
    {
        return this->sin4.is_unspecified();
    }
    else
    {
        return this->sin6.is_unspecified();
    }
}

bool IpAddr::is_loopback()
{
    if (this->is_v4)
    {
        return this->sin4.is_loopback();
    }
    else
    {
        return this->sin6.is_loopback();
    }
}

bool IpAddr::is_global()
{
    if (this->is_v4)
    {
        return this->sin4.is_global();
    }
    else
    {
        return this->sin6.is_global();
    }
}

bool IpAddr::is_multicast()
{
    if (this->is_v4)
    {
        return this->sin4.is_multicast();
    }
    else
    {
        return this->sin6.is_multicast();
    }
}

bool IpAddr::is_documentation()
{
    if (this->is_v4)
    {
        return this->sin4.is_documentation();
    }
    else
    {
        return this->sin6.is_documentation();
    }
}

bool IpAddr::is_benchmarking()
{
    if (this->is_v4)
    {
        return this->sin4.is_benchmarking();
    }
    else
    {
        return this->sin6.is_benchmarking();
    }
}

bool IpAddr::is_ipv4()
{
    return this->is_v4;
}

bool IpAddr::is_ipv6()
{
    return !this->is_v4;
}

IpAddr IpAddr::to_canonical()
{
    if (this->is_v4)
    {
        return this->sin4;
    }
    else
    {
        return this->sin6.to_canonical();
    }
}

std::string IpAddr::to_string()
{
    if (this->is_v4)
    {
        return this->sin4.to_string();
    }
    else
    {
        return this->sin6.to_string();
    }
}


// #[stable(feature = "i128", since = "1.26.0")]
// impl From<Ipv6Addr> for u128 {
//     /// Convert an `Ipv6Addr` into a host byte order `u128`.
//     ///
//     /// # Examples
//     ///
//     /// ```
//     /// use std::net::Ipv6Addr;
//     ///
//     /// let addr = Ipv6Addr::new(
//     ///     0x1020, 0x3040, 0x5060, 0x7080,
//     ///     0x90A0, 0xB0C0, 0xD0E0, 0xF00D,
//     /// );
//     /// assert_eq!(0x102030405060708090A0B0C0D0E0F00D_u128, u128::from(addr));
//     /// ```
//     #[inline]
//     fn from(ip: Ipv6Addr) -> u128 {
//         u128::from_be_bytes(ip.octets)
//     }
// }
// #[stable(feature = "i128", since = "1.26.0")]
// impl From<u128> for Ipv6Addr {
//     /// Convert a host byte order `u128` into an `Ipv6Addr`.
//     ///
//     /// # Examples
//     ///
//     /// ```
//     /// use std::net::Ipv6Addr;
//     ///
//     /// let addr = Ipv6Addr::from(0x102030405060708090A0B0C0D0E0F00D_u128);
//     /// assert_eq!(
//     ///     Ipv6Addr::new(
//     ///         0x1020, 0x3040, 0x5060, 0x7080,
//     ///         0x90A0, 0xB0C0, 0xD0E0, 0xF00D,
//     ///     ),
//     ///     addr);
//     /// ```
//     #[inline]
//     fn from(ip: u128) -> Ipv6Addr {
//         Ipv6Addr::from(ip.to_be_bytes())
//     }
// }
