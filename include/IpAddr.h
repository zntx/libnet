/*
https://doc.rust-lang.org/std/net/enum.IpAddr.html

1.0.0 · source · [−]
*/

#ifndef IPADDR_H_H
#define IPADDR_H_H
#include <stdint.h>
#include "Slice.h"
#include "socket_include.h"


class Ipv4Addr;
class Ipv6Addr;
class IpAddr;

class Ipv4Addr{
public:  
    union {
        uint8_t	_octets[4];
        struct in_addr sin;
    };

    /// Creates a new IPv4 address from four eight-bit octets.
    ///
    /// The result will represent the IP address `a`.`b`.`c`.`d`.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// let addr = Ipv4Addr::new(127, 0, 0, 1);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_32", since = "1.32.0")]
    // #[stable(feature = "rust1", since = "1.0.0")]
    // #[must_use]
    // #[inline]

    static Ipv4Addr * Create(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

    static Ipv4Addr * Create(std::string ips);

    static Ipv4Addr * Create(Slice<const char> ips);

    Ipv4Addr( );
    Ipv4Addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    //Ipv4Addr(uint32_t d);
    Ipv4Addr(const uint8_t (&array)[4]);
    Ipv4Addr(struct in_addr _sin);
    Ipv4Addr(struct in_addr *_sin);
    Ipv4Addr( const Ipv4Addr &other);
    Ipv4Addr(Ipv4Addr &&other);
    Ipv4Addr& operator=(const Ipv4Addr& other);
    bool operator==(const Ipv4Addr &other);

    /// Returns the four eight-bit integers that make up this address.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// let addr = Ipv4Addr::new(127, 0, 0, 1);
    /// assert_eq!(addr.octets(), [127, 0, 0, 1]);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "rust1", since = "1.0.0")]
    // #[must_use]
    // #[inline]

    uint8_t (&octets())[4];

    /// Returns [`true`] for the special 'unspecified' address (`0.0.0.0`).
    ///
    /// This property is defined in _UNIX Network Programming, Second Edition_,
    /// W. Richard Stevens, p. 891; see also [ip7].
    ///
    /// [ip7]: https://man7.org/linux/man-pages/man7/ip.7.html
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(0, 0, 0, 0).is_unspecified(), true);
    /// assert_eq!(Ipv4Addr::new(45, 22, 13, 197).is_unspecified(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_32", since = "1.32.0")]
    // #[stable(feature = "ip_shared", since = "1.12.0")]
    // #[must_use]
    // #[inline]
    bool is_unspecified();

    /// Returns [`true`] if this is a loopback address (`127.0.0.0/8`).
    ///
    /// This property is defined by [IETF RFC 1122].
    ///
    /// [IETF RFC 1122]: https://tools.ietf.org/html/rfc1122
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(127, 0, 0, 1).is_loopback(), true);
    /// assert_eq!(Ipv4Addr::new(45, 22, 13, 197).is_loopback(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    // #[inline]
    bool is_loopback();

    /// Returns [`true`] if this is a private address.
    ///
    /// The private address ranges are defined in [IETF RFC 1918] and include:
    ///
    ///  - `10.0.0.0/8`
    ///  - `172.16.0.0/12`
    ///  - `192.168.0.0/16`
    ///
    /// [IETF RFC 1918]: https://tools.ietf.org/html/rfc1918
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(10, 0, 0, 1).is_private(), true);
    /// assert_eq!(Ipv4Addr::new(10, 10, 10, 10).is_private(), true);
    /// assert_eq!(Ipv4Addr::new(172, 16, 10, 10).is_private(), true);
    /// assert_eq!(Ipv4Addr::new(172, 29, 45, 14).is_private(), true);
    /// assert_eq!(Ipv4Addr::new(172, 32, 0, 2).is_private(), false);
    /// assert_eq!(Ipv4Addr::new(192, 168, 0, 2).is_private(), true);
    /// assert_eq!(Ipv4Addr::new(192, 169, 0, 2).is_private(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    bool is_private();

    /// Returns [`true`] if the address is link-local (`169.254.0.0/16`).
    ///
    /// This property is defined by [IETF RFC 3927].
    ///
    /// [IETF RFC 3927]: https://tools.ietf.org/html/rfc3927
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(169, 254, 0, 0).is_link_local(), true);
    /// assert_eq!(Ipv4Addr::new(169, 254, 10, 65).is_link_local(), true);
    /// assert_eq!(Ipv4Addr::new(16, 89, 10, 65).is_link_local(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    bool is_link_local();

    /// Returns [`true`] if the address appears to be globally reachable
    /// as specified by the [IANA IPv4 Special-Purpose Address Registry].
    /// Whether or not an address is practically reachable will depend on your network configuration.
    ///
    /// Most IPv4 addresses are globally reachable;
    /// unless they are specifically defined as *not* globally reachable.
    ///
    /// Non-exhaustive list of notable addresses that are not globally reachable:
    ///
    /// - The [unspecified address] ([`is_unspecified`](Ipv4Addr::is_unspecified))
    /// - Addresses reserved for private use ([`is_private`](Ipv4Addr::is_private))
    /// - Addresses in the shared address space ([`is_shared`](Ipv4Addr::is_shared))
    /// - Loopback addresses ([`is_loopback`](Ipv4Addr::is_loopback))
    /// - Link-local addresses ([`is_link_local`](Ipv4Addr::is_link_local))
    /// - Addresses reserved for documentation ([`is_documentation`](Ipv4Addr::is_documentation))
    /// - Addresses reserved for benchmarking ([`is_benchmarking`](Ipv4Addr::is_benchmarking))
    /// - Reserved addresses ([`is_reserved`](Ipv4Addr::is_reserved))
    /// - The [broadcast address] ([`is_broadcast`](Ipv4Addr::is_broadcast))
    ///
    /// For the complete overview of which addresses are globally reachable, see the table at the [IANA IPv4 Special-Purpose Address Registry].
    ///
    /// [IANA IPv4 Special-Purpose Address Registry]: https://www.iana.org/assignments/iana-ipv4-special-registry/iana-ipv4-special-registry.xhtml
    /// [unspecified address]: Ipv4Addr::UNSPECIFIED
    /// [broadcast address]: Ipv4Addr::BROADCAST

    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv4Addr;
    ///
    /// // Most IPv4 addresses are globally reachable:
    /// assert_eq!(Ipv4Addr::new(80, 9, 12, 3).is_global(), true);
    ///
    /// // However some addresses have been assigned a special meaning
    /// // that makes them not globally reachable. Some examples are:
    ///
    /// // The unspecified address (`0.0.0.0`)
    /// assert_eq!(Ipv4Addr::UNSPECIFIED.is_global(), false);
    ///
    /// // Addresses reserved for private use (`10.0.0.0/8`, `172.16.0.0/12`, 192.168.0.0/16)
    /// assert_eq!(Ipv4Addr::new(10, 254, 0, 0).is_global(), false);
    /// assert_eq!(Ipv4Addr::new(192, 168, 10, 65).is_global(), false);
    /// assert_eq!(Ipv4Addr::new(172, 16, 10, 65).is_global(), false);
    ///
    /// // Addresses in the shared address space (`100.64.0.0/10`)
    /// assert_eq!(Ipv4Addr::new(100, 100, 0, 0).is_global(), false);
    ///
    /// // The loopback addresses (`127.0.0.0/8`)
    /// assert_eq!(Ipv4Addr::LOCALHOST.is_global(), false);
    ///
    /// // Link-local addresses (`169.254.0.0/16`)
    /// assert_eq!(Ipv4Addr::new(169, 254, 45, 1).is_global(), false);
    ///
    /// // Addresses reserved for documentation (`192.0.2.0/24`, `198.51.100.0/24`, `203.0.113.0/24`)
    /// assert_eq!(Ipv4Addr::new(192, 0, 2, 255).is_global(), false);
    /// assert_eq!(Ipv4Addr::new(198, 51, 100, 65).is_global(), false);
    /// assert_eq!(Ipv4Addr::new(203, 0, 113, 6).is_global(), false);
    ///
    /// // Addresses reserved for benchmarking (`198.18.0.0/15`)
    /// assert_eq!(Ipv4Addr::new(198, 18, 0, 0).is_global(), false);
    ///
    /// // Reserved addresses (`240.0.0.0/4`)
    /// assert_eq!(Ipv4Addr::new(250, 10, 20, 30).is_global(), false);
    ///
    /// // The broadcast address (`255.255.255.255`)
    /// assert_eq!(Ipv4Addr::BROADCAST.is_global(), false);
    ///
    /// // For a complete overview see the IANA IPv4 Special-Purpose Address Registry.
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv4", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    bool is_global();

    /// Returns [`true`] if this address is part of the Shared Address Space defined in
    /// [IETF RFC 6598] (`100.64.0.0/10`).
    ///
    /// [IETF RFC 6598]: https://tools.ietf.org/html/rfc6598
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(100, 64, 0, 0).is_shared(), true);
    /// assert_eq!(Ipv4Addr::new(100, 127, 255, 255).is_shared(), true);
    /// assert_eq!(Ipv4Addr::new(100, 128, 0, 0).is_shared(), false);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv4", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    bool is_shared();

    /// Returns [`true`] if this address part of the `198.18.0.0/15` range, which is reserved for
    /// network devices benchmarking. This range is defined in [IETF RFC 2544] as `192.18.0.0`
    /// through `198.19.255.255` but [errata 423] corrects it to `198.18.0.0/15`.
    ///
    /// [IETF RFC 2544]: https://tools.ietf.org/html/rfc2544
    /// [errata 423]: https://www.rfc-editor.org/errata/eid423
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(198, 17, 255, 255).is_benchmarking(), false);
    /// assert_eq!(Ipv4Addr::new(198, 18, 0, 0).is_benchmarking(), true);
    /// assert_eq!(Ipv4Addr::new(198, 19, 255, 255).is_benchmarking(), true);
    /// assert_eq!(Ipv4Addr::new(198, 20, 0, 0).is_benchmarking(), false);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv4", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // inline
    bool is_benchmarking();

    /// Returns [`true`] if this address is reserved by IANA for future use. [IETF RFC 1112]
    /// defines the block of reserved addresses as `240.0.0.0/4`. This range normally includes the
    /// broadcast address `255.255.255.255`, but this implementation explicitly excludes it, since
    /// it is obviously not reserved for future use.
    ///
    /// [IETF RFC 1112]: https://tools.ietf.org/html/rfc1112
    ///
    /// # Warning
    ///
    /// As IANA assigns new addresses, this method will be
    /// updated. This may result in non-reserved addresses being
    /// treated as reserved in code that relies on an outdated version
    /// of this method.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(240, 0, 0, 0).is_reserved(), true);
    /// assert_eq!(Ipv4Addr::new(255, 255, 255, 254).is_reserved(), true);
    ///
    /// assert_eq!(Ipv4Addr::new(239, 255, 255, 255).is_reserved(), false);
    /// // The broadcast address is not considered as reserved for future use by this implementation
    /// assert_eq!(Ipv4Addr::new(255, 255, 255, 255).is_reserved(), false);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv4", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    bool is_reserved();

    /// Returns [`true`] if this is a multicast address (`224.0.0.0/4`).
    ///
    /// Multicast addresses have a most significant octet between `224` and `239`,
    /// and is defined by [IETF RFC 5771].
    ///
    /// [IETF RFC 5771]: https://tools.ietf.org/html/rfc5771
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(224, 254, 0, 0).is_multicast(), true);
    /// assert_eq!(Ipv4Addr::new(236, 168, 10, 65).is_multicast(), true);
    /// assert_eq!(Ipv4Addr::new(172, 16, 10, 65).is_multicast(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    bool is_multicast();

    /// Returns [`true`] if this is a broadcast address (`255.255.255.255`).
    ///
    /// A broadcast address has all octets set to `255` as defined in [IETF RFC 919].
    ///
    /// [IETF RFC 919]: https://tools.ietf.org/html/rfc919
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(255, 255, 255, 255).is_broadcast(), true);
    /// assert_eq!(Ipv4Addr::new(236, 168, 10, 65).is_broadcast(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    bool is_broadcast();

    /// Returns [`true`] if this address is in a range designated for documentation.
    ///
    /// This is defined in [IETF RFC 5737]:
    ///
    /// - `192.0.2.0/24` (TEST-NET-1)
    /// - `198.51.100.0/24` (TEST-NET-2)
    /// - `203.0.113.0/24` (TEST-NET-3)
    ///
    /// [IETF RFC 5737]: https://tools.ietf.org/html/rfc5737
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv4Addr;
    ///
    /// assert_eq!(Ipv4Addr::new(192, 0, 2, 255).is_documentation(), true);
    /// assert_eq!(Ipv4Addr::new(198, 51, 100, 65).is_documentation(), true);
    /// assert_eq!(Ipv4Addr::new(203, 0, 113, 6).is_documentation(), true);
    /// assert_eq!(Ipv4Addr::new(193, 34, 17, 19).is_documentation(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    bool is_documentation();

    /// Converts this address to an [IPv4-compatible] [`IPv6` address].
    ///
    /// `a.b.c.d` becomes `::a.b.c.d`
    ///
    /// Note that IPv4-compatible addresses have been officially deprecated.
    /// If you don't explicitly need an IPv4-compatible address for legacy reasons, consider using `to_ipv6_mapped` instead.
    ///
    /// [IPv4-compatible]: Ipv6Addr#ipv4-compatible-ipv6-addresses
    /// [`IPv6` address]: Ipv6Addr
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(
    ///     Ipv4Addr::new(192, 0, 2, 255).to_ipv6_compatible(),
    ///     Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0xc000, 0x2ff)
    /// );
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "rust1", since = "1.0.0")]
    // #[must_use = "this returns the result of the operation, 
    //               without modifying the original"]
    Ipv6Addr to_ipv6_compatible();

    /// Converts this address to an [IPv4-mapped] [`IPv6` address].
    ///
    /// `a.b.c.d` becomes `::ffff:a.b.c.d`
    ///
    /// [IPv4-mapped]: Ipv6Addr#ipv4-mapped-ipv6-addresses
    /// [`IPv6` address]: Ipv6Addr
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(Ipv4Addr::new(192, 0, 2, 255).to_ipv6_mapped(),
    ///            Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc000, 0x2ff));
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "rust1", since = "1.0.0")]
    // #[must_use = "this returns the result of the operation, 
    //               without modifying the original"]
    // #[inline]
    Ipv6Addr to_ipv6_mapped();

    // #[stable(feature = "rust1", since = "1.0.0")]
    // impl fmt::Display for Ipv4Addr {
    // fn fmt(&self, fmt: &mut fmt::Formatter<'_'>) -> fmt::Result {
    std::string to_string();

    // #[stable(feature = "rust1", since = "1.0.0")]
    // impl fmt::Debug for Ipv4Addr {
    //  fn fmt(&self, fmt: &mut fmt::Formatter<'_'>) -> fmt::Result {
    //      fmt::Display::fmt(self, fmt)
    //  }
    //}
};

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
    extern Ipv4Addr LOCALHOST;

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
    extern Ipv4Addr UNSPECIFIED;

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
    extern Ipv4Addr BROADCAST;
}

class Ipv6Addr{
    enum  Ipv6MulticastScope
    {
        Null,
        /// Interface-Local scope.
        InterfaceLocal,
        /// Link-Local scope.
        LinkLocal,
        /// Realm-Local scope.
        RealmLocal,
        /// Admin-Local scope.
        AdminLocal,
        /// Site-Local scope.
        SiteLocal,
        /// Organization-Local scope.
        OrganizationLocal,
        /// Global scope.
        Global,
    };

    union {
        uint8_t	 addr8[16];
        uint16_t addr16[8];
        uint32_t addr32[4];
        struct in6_addr sin;
    };


public:
    static Ipv6Addr * Create(Slice<const char> ips);

    Ipv6Addr(struct in6_addr _sin);

    Ipv6Addr(const uint8_t(&arr)[16]);

    Ipv6Addr(const uint16_t(&arr)[8]);
    
    Ipv6Addr(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e, uint16_t f, uint16_t g, uint16_t h);

    Ipv6Addr(uint8_t a1, uint8_t a2, uint8_t b1, uint8_t b2,  uint8_t c1, uint8_t c2, uint8_t d1, uint8_t d2, uint8_t e1, uint8_t e2, uint8_t f1, uint8_t f2,  uint8_t g1, uint8_t g2, uint8_t h1, uint8_t h2 );
    
    
    bool operator==(const Ipv6Addr &other);

    /// Returns the eight 16-bit segments that make up this address.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).segments(),
    ///            [0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff]);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "rust1", since = "1.0.0")]
    // #[must_use]
    // #[inline]
    uint16_t segments(uint32_t index);

    /// Returns [`true`] for the special 'unspecified' address (`::`).
    ///
    /// This property is defined in [IETF RFC 4291].
    ///
    /// [IETF RFC 4291]: https://tools.ietf.org/html/rfc4291
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_unspecified(), false);
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 0).is_unspecified(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    // #[inline]
    bool is_unspecified();

    /// Returns [`true`] if this is the [loopback address] (`::1`),
    /// as defined in [IETF RFC 4291 section 2.5.3].
    ///
    /// Contrary to IPv4, in IPv6 there is only one loopback address.
    ///
    /// [loopback address]: Ipv6Addr::LOCALHOST
    /// [IETF RFC 4291 section 2.5.3]: https://tools.ietf.org/html/rfc4291#section-2.5.3
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_loopback(), false);
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 0x1).is_loopback(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    // #[inline]
    bool is_loopback();

    /// Returns [`true`] if the address appears to be globally reachable
    /// as specified by the [IANA IPv6 Special-Purpose Address Registry].
    /// Whether or not an address is practically reachable will depend on your network configuration.
    ///
    /// Most IPv6 addresses are globally reachable;
    /// unless they are specifically defined as *not* globally reachable.
    ///
    /// Non-exhaustive list of notable addresses that are not globally reachable:
    /// - The [unspecified address] ([`is_unspecified`](Ipv6Addr::is_unspecified))
    /// - The [loopback address] ([`is_loopback`](Ipv6Addr::is_loopback))
    /// - IPv4-mapped addresses
    /// - Addresses reserved for benchmarking
    /// - Addresses reserved for documentation ([`is_documentation`](Ipv6Addr::is_documentation))
    /// - Unique local addresses ([`is_unique_local`](Ipv6Addr::is_unique_local))
    /// - Unicast addresses with link-local scope ([`is_unicast_link_local`](Ipv6Addr::is_unicast_link_local))
    ///
    /// For the complete overview of which addresses are globally reachable, see the table at the [IANA IPv6 Special-Purpose Address Registry].
    ///
    /// Note that an address having global scope is not the same as being globally reachable,
    /// and there is no direct relation between the two concepts: There exist addresses with global scope
    /// that are not globally reachable (for example unique local addresses),
    /// and addresses that are globally reachable without having global scope
    /// (multicast addresses with non-global scope).
    ///
    /// [IANA IPv6 Special-Purpose Address Registry]: https://www.iana.org/assignments/iana-ipv6-special-registry/iana-ipv6-special-registry.xhtml
    /// [unspecified address]: Ipv6Addr::UNSPECIFIED
    /// [loopback address]: Ipv6Addr::LOCALHOST
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// // Most IPv6 addresses are globally reachable:
    /// assert_eq!(Ipv6Addr::new(0x26, 0, 0x1c9, 0, 0, 0xafc8, 0x10, 0x1).is_global(), true);
    ///
    /// // However some addresses have been assigned a special meaning
    /// // that makes them not globally reachable. Some examples are:
    ///
    /// // The unspecified address (`::`)
    /// assert_eq!(Ipv6Addr::UNSPECIFIED.is_global(), false);
    ///
    /// // The loopback address (`::1`)
    /// assert_eq!(Ipv6Addr::LOCALHOST.is_global(), false);
    ///
    /// // IPv4-mapped addresses (`::ffff:0:0/96`)
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_global(), false);
    ///
    /// // Addresses reserved for benchmarking (`2001:2::/48`)
    /// assert_eq!(Ipv6Addr::new(0x2001, 2, 0, 0, 0, 0, 0, 1,).is_global(), false);
    ///
    /// // Addresses reserved for documentation (`2001:db8::/32`)
    /// assert_eq!(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 1).is_global(), false);
    ///
    /// // Unique local addresses (`fc00::/7`)
    /// assert_eq!(Ipv6Addr::new(0xfc02, 0, 0, 0, 0, 0, 0, 1).is_global(), false);
    ///
    /// // Unicast addresses with link-local scope (`fe80::/10`)
    /// assert_eq!(Ipv6Addr::new(0xfe81, 0, 0, 0, 0, 0, 0, 1).is_global(), false);
    ///
    /// // For a complete overview see the IANA IPv6 Special-Purpose Address Registry.
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_global();

    /// Returns [`true`] if this is a unique local address (`fc00::/7`).
    ///
    /// This property is defined in [IETF RFC 4193].
    ///
    /// [IETF RFC 4193]: https://tools.ietf.org/html/rfc4193
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_unique_local(), false);
    /// assert_eq!(Ipv6Addr::new(0xfc02, 0, 0, 0, 0, 0, 0, 0).is_unique_local(), true);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_unique_local();

    /// Returns [`true`] if this is a unicast address, as defined by [IETF RFC 4291].
    /// Any address that is not a [multicast address] (`ff00::/8`) is unicast.
    ///
    /// [IETF RFC 4291]: https://tools.ietf.org/html/rfc4291
    /// [multicast address]: Ipv6Addr::is_multicast
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// // The unspecified and loopback addresses are unicast.
    /// assert_eq!(Ipv6Addr::UNSPECIFIED.is_unicast(), true);
    /// assert_eq!(Ipv6Addr::LOCALHOST.is_unicast(), true);
    ///
    /// // Any address that is not a multicast address (`ff00::/8`) is unicast.
    /// assert_eq!(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0).is_unicast(), true);
    /// assert_eq!(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0).is_unicast(), false);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_unicast();

    /// Returns `true` if the address is a unicast address with link-local scope,
    /// as defined in [RFC 4291].
    ///
    /// A unicast address has link-local scope if it has the prefix `fe80::/10`, as per [RFC 4291 section 2.4].
    /// Note that this encompasses more addresses than those defined in [RFC 4291 section 2.5.6],
    /// which describes "Link-Local IPv6 Unicast Addresses" as having the following stricter format:
    ///
    /// ```text
    /// | 10 bits  |         54 bits         |          64 bits           |
    /// +----------+-------------------------+----------------------------+
    /// |1111111010|           0             |       interface ID         |
    /// +----------+-------------------------+----------------------------+
    /// ```
    /// So while currently the only addresses with link-local scope an application will encounter are all in `fe80::/64`,
    /// this might change in the future with the publication of new standards. More addresses in `fe80::/10` could be allocated,
    /// and those addresses will have link-local scope.
    ///
    /// Also note that while [RFC 4291 section 2.5.3] mentions about the [loopback address] (`::1`) that "it is treated as having Link-Local scope",
    /// this does not mean that the loopback address actually has link-local scope and this method will return `false` on it.
    ///
    /// [RFC 4291]: https://tools.ietf.org/html/rfc4291
    /// [RFC 4291 section 2.4]: https://tools.ietf.org/html/rfc4291#section-2.4
    /// [RFC 4291 section 2.5.3]: https://tools.ietf.org/html/rfc4291#section-2.5.3
    /// [RFC 4291 section 2.5.6]: https://tools.ietf.org/html/rfc4291#section-2.5.6
    /// [loopback address]: Ipv6Addr::LOCALHOST
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// // The loopback address (`::1`) does not actually have link-local scope.
    /// assert_eq!(Ipv6Addr::LOCALHOST.is_unicast_link_local(), false);
    ///
    /// // Only addresses in `fe80::/10` have link-local scope.
    /// assert_eq!(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0).is_unicast_link_local(), false);
    /// assert_eq!(Ipv6Addr::new(0xfe80, 0, 0, 0, 0, 0, 0, 0).is_unicast_link_local(), true);
    ///
    /// // Addresses outside the stricter `fe80::/64` also have link-local scope.
    /// assert_eq!(Ipv6Addr::new(0xfe80, 0, 0, 1, 0, 0, 0, 0).is_unicast_link_local(), true);
    /// assert_eq!(Ipv6Addr::new(0xfe81, 0, 0, 0, 0, 0, 0, 0).is_unicast_link_local(), true);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_unicast_link_local();

    /// Returns [`true`] if this is an address reserved for documentation
    /// (`2001:db8::/32`).
    ///
    /// This property is defined in [IETF RFC 3849].
    ///
    /// [IETF RFC 3849]: https://tools.ietf.org/html/rfc3849
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_documentation(), false);
    /// assert_eq!(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0).is_documentation(), true);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_documentation();

    /// Returns [`true`] if this is an address reserved for benchmarking (`2001:2::/48`).
    ///
    /// This property is defined in [IETF RFC 5180], where it is mistakenly specified as covering the range `2001:0200::/48`.
    /// This is corrected in [IETF RFC Errata 1752] to `2001:0002::/48`.
    ///
    /// [IETF RFC 5180]: https://tools.ietf.org/html/rfc5180
    /// [IETF RFC Errata 1752]: https://www.rfc-editor.org/errata_search.php?eid=1752
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc613, 0x0).is_benchmarking(), false);
    /// assert_eq!(Ipv6Addr::new(0x2001, 0x2, 0, 0, 0, 0, 0, 0).is_benchmarking(), true);
    /// ```
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_benchmarking();

    /// Returns [`true`] if the address is a globally routable unicast address.
    ///
    /// The following return false:
    ///
    /// - the loopback address
    /// - the link-local addresses
    /// - unique local addresses
    /// - the unspecified address
    /// - the address range reserved for documentation
    ///
    /// This method returns [`true`] for site-local addresses as per [RFC 4291 section 2.5.7]
    ///
    /// ```no_rust
    /// The special behavior of [the site-local unicast] prefix defined in [RFC3513] must no longer
    /// be supported in new implementations (i.e., new implementations must treat this prefix as
    /// Global Unicast).
    /// ```
    ///
    /// [RFC 4291 section 2.5.7]: https://tools.ietf.org/html/rfc4291#section-2.5.7
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0).is_unicast_global(), false);
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_unicast_global(), true);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_unicast_global();

    /// Returns the address's multicast scope if the address is multicast.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::{Ipv6Addr, Ipv6MulticastScope};
    ///
    /// assert_eq!(
    ///     Ipv6Addr::new(0xff0e, 0, 0, 0, 0, 0, 0, 0).multicast_scope(),
    ///     Some(Ipv6MulticastScope::Global)
    /// );
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).multicast_scope(), None);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    Ipv6Addr::Ipv6MulticastScope multicast_scope();

    /// Returns [`true`] if this is a multicast address (`ff00::/8`).
    ///
    /// This property is defined by [IETF RFC 4291].
    ///
    /// [IETF RFC 4291]: https://tools.ietf.org/html/rfc4291
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::Ipv6Addr;
    ///
    /// assert_eq!(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0).is_multicast(), true);
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).is_multicast(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(since = "1.7.0", feature = "ip_17")]
    // #[must_use]
    // #[inline]
    bool is_multicast();

    /// Converts this address to an [`IPv4` address] if it's an [IPv4-mapped] address,
    /// as defined in [IETF RFC 4291 section 2.5.5.2], otherwise returns [`None`].
    ///
    /// `::ffff:a.b.c.d` becomes `a.b.c.d`.
    /// All addresses *not* starting with `::ffff` will return `None`.
    ///
    /// [`IPv4` address]: Ipv4Addr
    /// [IPv4-mapped]: Ipv6Addr
    /// [IETF RFC 4291 section 2.5.5.2]: https://tools.ietf.org/html/rfc4291#section-2.5.5.2
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0).to_ipv4_mapped(), None);
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0xc00a, 0x2ff).to_ipv4_mapped(),
    ///            Some(Ipv4Addr::new(192, 10, 2, 255)));
    /// assert_eq!(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 1).to_ipv4_mapped(), None);
    /// ```
    // #[rustc_const_unstable(feature = "const_ipv6", issue = "76205")]
    // #[stable(feature = "ipv6_to_ipv4_mapped", since = "1.63.0")]
    // #[must_use = "this returns the result of the operation, 
    //               without modifying the original"]
    // #[inline]
    Ipv4Addr * to_ipv4_mapped();

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
    // #[must_use = "this returns the result of the operation, 
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

    IpAddr to_canonical();

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
    uint8_t (&octets())[16];

    /// Write an Ipv6Addr, conforming to the canonical style described by
    /// [RFC 5952](https://tools.ietf.org/html/rfc5952).
    // #[stable(feature = "rust1", since = "1.0.0")]
    // impl fmt::Display for Ipv6Addr {
    std::string to_string();

};






class IpAddr
{
public:
    bool is_v4;
    union
    {
        Ipv4Addr sin4;
        Ipv6Addr sin6;
    };

    static IpAddr V4( const Ipv4Addr & ip);
    static IpAddr V6( const Ipv6Addr & ip);

    static IpAddr * create(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

    static IpAddr * Create(std::string host);

    static IpAddr * Create(Slice<const char> host);

    /// Returns [`true`] for the special 'unspecified' address.
    ///
    /// See the documentation for [`Ipv4Addr::is_unspecified()`] and
    /// [`Ipv6Addr::is_unspecifiedV()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0)).is_unspecified(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 0)).is_unspecified(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "ip_shared", since = "1.12.0")]
    // #[must_use]
    // #[inline]
    bool is_unspecified();

    /// Returns [`true`] if this is a loopback address.
    ///
    /// See the documentation for [`Ipv4Addr::is_loopback()`] and
    /// [`Ipv6Addr::is_loopback()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(127, 0, 0, 1)).is_loopback(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0, 0, 0, 0, 0, 0, 0, 0x1)).is_loopback(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "ip_shared", since = "1.12.0")]
    // #[must_use]
    // #[inline]
    bool is_loopback();

    /// Returns [`true`] if the address appears to be globally routable.
    ///
    /// See the documentation for [`Ipv4Addr::is_global()`] and
    /// [`Ipv6Addr::is_global()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(80, 9, 12, 3)).is_global(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0, 0, 0x1c9, 0, 0, 0xafc8, 0, 0x1)).is_global(), true);
    /// ```
    // #[rustc_const_unstable(feature = "const_ip", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_global();

    /// Returns [`true`] if this is a multicast address.
    ///
    /// See the documentation for [`Ipv4Addr::is_multicast()`] and
    /// [`Ipv6Addr::is_multicast()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(224, 254, 0, 0)).is_multicast(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0xff00, 0, 0, 0, 0, 0, 0, 0)).is_multicast(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "ip_shared", since = "1.12.0")]
    // #[must_use]
    // #[inline]
    bool is_multicast();

    /// Returns [`true`] if this address is in a range designated for documentation.
    ///
    /// See the documentation for [`Ipv4Addr::is_documentation()`] and
    /// [`Ipv6Addr::is_documentation()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(203, 0, 113, 6)).is_documentation(), true);
    /// assert_eq!(
    ///     IpAddr::V6(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0)).is_documentation(),
    ///     true
    /// );
    /// ```
    // #[rustc_const_unstable(feature = "const_ip", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_documentation();

    /// Returns [`true`] if this address is in a range designated for benchmarking.
    ///
    /// See the documentation for [`Ipv4Addr::is_benchmarking()`] and
    /// [`Ipv6Addr::is_benchmarking()`] for more details.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    ///
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(198, 19, 255, 255)).is_benchmarking(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0x2001, 0x2, 0, 0, 0, 0, 0, 0)).is_benchmarking(), true);
    /// ```
    // #[unstable(feature = "ip", issue = "27709")]
    // #[must_use]
    // #[inline]
    bool is_benchmarking();

    /// Returns [`true`] if this address is an [`IPv4` address], and [`false`]
    /// otherwise.
    ///
    /// [`IPv4` address]: IpAddr::V4
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(203, 0, 113, 6)).is_ipv4(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0)).is_ipv4(), false);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "ipaddr_checker", since = "1.16.0")]
    // #[must_use]
    // #[inline]
    bool is_ipv4();

    /// Returns [`true`] if this address is an [`IPv6` address], and [`false`]
    /// otherwise.
    ///
    /// [`IPv6` address]: IpAddr::V6
    ///
    /// # Examples
    ///
    /// ```
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(203, 0, 113, 6)).is_ipv6(), false);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0x2001, 0xdb8, 0, 0, 0, 0, 0, 0)).is_ipv6(), true);
    /// ```
    // #[rustc_const_stable(feature = "const_ip_50", since = "1.50.0")]
    // #[stable(feature = "ipaddr_checker", since = "1.16.0")]
    // #[must_use]
    // #[inline]
    bool is_ipv6();

    /// Converts this address to an `IpAddr::V4` if it is an IPv4-mapped IPv6 addresses, otherwise it
    /// return `self` as-is.
    ///
    /// # Examples
    ///
    /// ```
    /// #![feature(ip)]
    /// use std::net::{IpAddr, Ipv4Addr, Ipv6Addr};
    ///
    /// assert_eq!(IpAddr::V4(Ipv4Addr::new(127, 0, 0, 1)).to_canonical().is_loopback(), true);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0x7f00, 0x1)).is_loopback(), false);
    /// assert_eq!(IpAddr::V6(Ipv6Addr::new(0, 0, 0, 0, 0, 0xffff, 0x7f00, 0x1)).to_canonical().is_loopback(), true);
    /// ```
    // #[inline]
    // #[must_use = "this returns the result of the operation, 
    //               without modifying the original"]
    // #[rustc_const_unstable(feature = "const_ip", issue = "76205")]
    // #[unstable(feature = "ip", issue = "27709")]
    IpAddr to_canonical();

    std::string to_string();

    IpAddr();

    IpAddr(Ipv4Addr ipv4);

    IpAddr(Ipv6Addr ipv6);

    IpAddr(const IpAddr &ipv);
    IpAddr(IpAddr&& ip);

    IpAddr(const uint8_t (&arr)[4]);

    

    bool operator==(const IpAddr &other);

    bool operator==(const Ipv4Addr &other);

    bool operator==(const Ipv6Addr &other);

    IpAddr(const uint8_t (&arr)[16]);

    IpAddr(const uint16_t (&arr)[8]);
};

// impl IntoInner<c::in_addr> for Ipv4Addr {
//     #[inline]
//     fn into_inner(self) -> c::in_addr {
//         // `s_addr` is stored as BE on all machines and the array is in BE order.
//         // So the native endian conversion method is used so that it's never swapped.
//         c::in_addr { s_addr: u32::from_ne_bytes(self.octets) }
//     }
// }
// impl FromInner<c::in_addr> for Ipv4Addr {
//     fn from_inner(addr: c::in_addr) -> Ipv4Addr {
//         Ipv4Addr { octets: addr.s_addr.to_ne_bytes() }
//     }
// }

// impl IntoInner<c::in6_addr> for Ipv6Addr {
//     fn into_inner(self) -> c::in6_addr {
//         c::in6_addr { s6_addr: self.octets }
//     }
// }
// impl FromInner<c::in6_addr> for Ipv6Addr {
//     #[inline]
//     fn from_inner(addr: c::in6_addr) -> Ipv6Addr {
//         Ipv6Addr { octets: addr.s6_addr }
//     }
// }

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

#endif