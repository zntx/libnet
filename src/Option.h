#pragma once

#include <cstdlib>
#include <cstring>
#include <utility>
#include <iostream>
#include <tuple>
#include <functional>

namespace types
{
    template <typename T>
    class Some
    {
    public:
        template <typename U>
        friend class Option;
        Some(const T &val) : val_(val) {}
        Some(T &&val) : val_(std::move(val)) {}

    //private:
        T val_;
    };

    class None
    {
    };

    namespace impl
    {
        //// 用于获取可调用对象（函数或函数对象）的特征信息的模板    
        template <typename Func>
        struct callable_trait : public callable_trait<decltype(&Func::operator())>
        {
        };

        template <typename Class, typename Ret, typename... Args>
        struct callable_trait<Ret (Class::*)(Args...) const>
        {
            // 参数个数的常量表达式
            static constexpr size_t Arity = sizeof...(Args);
            // 数类型的元组类型
            typedef std::tuple<Args...> ArgsType;
            typedef Ret ReturnType;

            template <size_t Index>
            struct Arg
            {
                static_assert(Index < Arity, "Invalid index");
                typedef typename std::tuple_element<Index, ArgsType>::type Type;
            };
        };
    }

    template <typename Func,
              bool IsBindExp = std::is_bind_expression<Func>::value> /*用于检查Func是否是一个std::bind表达式（或者可调用对象）的结果*/
    struct callable_trait;

    /* std::bind returns an unspecified type which contains several overloads
     * of operator(). Thus, decltype(&operator()) does not compile since operator()
     * is overloaded. To bypass that, we only define the ReturnType for bind
     * expressions
     */
    template <typename Func>
    struct callable_trait<Func, true>
    {
        typedef typename Func::result_type ReturnType;
    };

    template <typename Func>
    struct callable_trait<Func, false> : public impl::callable_trait<Func>
    {
    };

    //    // 检查给定类型是否具有以指定参数构造对象的构造函数，并且该构造函数是否被声明为noexcept。
    template <typename T>
    struct is_nothrow_move_constructible : std::is_nothrow_constructible<T, typename std::add_rvalue_reference<T>::type>
    {
    };

    template <class T>
    struct is_move_constructible : std::is_constructible<T, typename std::add_rvalue_reference<T>::type>
    {
    };

}

inline types::None None()
{
    return types::None();
}

//#define None None()

template <typename T,
          typename CleanT = typename std::remove_reference<T>::type>
inline types::Some<CleanT> Some(T &&value)
{
    return types::Some<CleanT>(std::forward<T>(value));
}

template <typename T>
class Option
{
public:
    Option()
    {
        initialized = false;
    }

    // TODO: SFINAE-out if T is not trivially_copyable
    Option(const Option<T> &other)
    {
        if (!other.is_empty())
        {
            ::new (data()) T(*other.data());
            initialized = true;
        }
        else
        {
            initialized = false;
        }
    }

    Option(Option<T> &&other) noexcept(types::is_nothrow_move_constructible<T>::value)
    {
        *this = std::move(other);
    }

    template <typename U>
    Option(types::Some<U> some)
    {
        static_assert(std::is_same<T, U>::value || std::is_convertible<T, U>::value,
                      "Types mismatch");
        from_some_helper(std::move(some), types::is_move_constructible<U>());
    }
    Option(types::None) { initialized = false; }

    template <typename U>
    Option<T> &operator=(types::Some<U> some)
    {
        static_assert(std::is_same<T, U>::value || std::is_convertible<T, U>::value,
                      "Types mismatch");
        if (!is_empty())
        {
            data()->~T();
        }
        from_some_helper(std::move(some), types::is_move_constructible<U>());
        return *this;
    }

    Option<T> &operator=(types::None)
    {
        if (!is_empty())
        {
            data()->~T();
        }
        initialized = false;
        return *this;
    }

    // TODO: SFINAE-out if T is not trivially_copyable
    Option<T> &operator=(const Option<T> &other)
    {
        if (!other.is_empty())
        {
            if (!is_empty())
            {
                data()->~T();
            }
            ::new (data()) T(*other.data());
            initialized = true;
        }
        else
        {
            if (!is_empty())
            {
                data()->~T();
            }
            initialized = false;
        }
    }

    Option<T> &operator=(Option<T> &&other) noexcept(types::is_nothrow_move_constructible<T>::value)
    {
        if (other.data())
        {
            move_helper(std::move(other), types::is_move_constructible<T>());
            other.initialized = false;
        }
        else
        {
            initialized = false;
        }

        return *this;
    }

    bool is_empty() const
    {
        return initialized == false;
    }

    T unwrap( )
    {
        if (!is_empty())
        {
            initialized = false;
            return std::move(*constData());
        }

        std::fprintf(stderr, "%s:%d Attempting to unwrap an error Result\n", __FILE__, __LINE__);
        std::terminate();
    }

    T unwrap_or(const T &defaultValue)
    {
        if (!is_empty())
        {
            initialized = false;
            return std::move(*constData());
        }

        return defaultValue;
    }

    const T &get_or_else(const T &defaultValue)
    {
        if (!is_empty())
        {
            initialized = false;
            return std::move(*constData());
        }

        return defaultValue;
    }

    template <typename Func>
    void unwrap_or_else(Func func) const
    {
        if (is_empty())
        {
            func();
        }
    }

    T get()
    {
        return *constData();
    }

    const T &get() const
    {
        return *constData();
    }

    T &unsafeGet() const
    {
        return *data();
    }

    ~Option()
    {
        if (!is_empty())
        {
            data()->~T();
        }
    }

private:
    T *const constData() const
    {
        return const_cast<T *const>(reinterpret_cast<const T *const>(bytes));
    }

    /*将一个指向字节的指针转换为指向类型T的指针，并移除const限定。*/
    T *data() const
    {
        return const_cast<T *>(reinterpret_cast<const T *>(bytes));
    }

    void move_helper(Option<T> &&other, std::true_type)
    {
        ::new (data()) T(std::move(*other.data()));
        initialized = true;
    }

    void move_helper(Option<T> &&other, std::false_type)
    {
        ::new (data()) T(*other.data());
        initialized = true;
    }

    template <typename U>
    void from_some_helper(types::Some<U> some, std::true_type)
    {
        ::new (data()) T(std::move(some.val_));
        initialized = true;
    }

    template <typename U>
    void from_some_helper(types::Some<U> some, std::false_type)
    {
        ::new (data()) T(some.val_);
        initialized = true;
    }

    uint8_t bytes[sizeof(T)];
    bool initialized;
};

namespace details
{
    template <typename T>
    struct RemoveOptional
    {
        typedef T Type;
    };

    template <typename T>
    struct RemoveOptional<Option<T>>
    {
        typedef T Type;
    };

    template <typename T, typename Func>
    void do_static_checks(std::false_type)
    {
        static_assert(types::callable_trait<Func>::Arity == 1,
                      "The function must take exactly 1 argument");

        typedef typename types::callable_trait<Func>::template Arg<0>::Type ArgType;
        typedef typename std::remove_cv<
            typename std::remove_reference<ArgType>::type>::type CleanArgType;

        static_assert(std::is_same<CleanArgType, T>::value || std::is_convertible<CleanArgType, T>::value,
                      "Function parameter type mismatch");
    }

    template <typename T, typename Func>
    void do_static_checks(std::true_type)
    {
    }

    template <typename T, typename Func>
    void static_checks()
    {
        do_static_checks<T, Func>(std::is_bind_expression<Func>());
    }

    template <typename Func>
    struct IsArgMovable : public IsArgMovable<decltype(&Func::operator())>
    {
    };

    template <typename R, typename Class, typename Arg>
    struct IsArgMovable<R (Class::*)(Arg) const> : public std::is_rvalue_reference<Arg>
    {
    };

    template <typename Func, typename Arg>
    typename std::conditional<
        IsArgMovable<Func>::value,
        Arg &&,
        const Arg &>::type
    tryMove(Arg &arg)
    {
        return std::move(arg);
    }
}

template <typename T, typename Func>
const Option<T> &
option_do(const Option<T> &option, Func func)
{
    details::static_checks<T, Func>();
    static_assert(std::is_same<typename types::callable_trait<Func>::ReturnType, void>::value,
                  "Use optionally_map if you want to return a value");
    if (!option.is_empty())
    {
        func(details::tryMove<Func>(option.unsafeGet()));
    }

    return option;
}

template <typename T, typename Func>
auto option_map(const Option<T> &option, Func func)
    -> Option<typename types::callable_trait<Func>::ReturnType>
{
    details::static_checks<T, Func>();
    if (!option.is_empty())
    {
        return Some(func(details::tryMove<Func>(option.unsafeGet())));
    }

    return None;
}

template <typename T, typename Func>
auto option_fmap(const Option<T> &option, Func func)
    -> Option<typename details::RemoveOptional<typename types::callable_trait<Func>::ReturnType>::Type>
{
    details::static_checks<T, Func>();
    if (!option.is_empty())
    {
        const auto &ret = func(details::tryMove<Func>(option.unsafeGet()));
        if (!ret.is_empty())
        {
            return Some(ret.get());
        }
    }

    return None;
}

template <typename T, typename Func>
Option<T> option_filter(const Option<T> &option, Func func)
{
    details::static_checks<T, Func>();
    typedef typename types::callable_trait<Func>::ReturnType ReturnType;
    static_assert(std::is_same<ReturnType, bool>::value ||
                      std::is_convertible<ReturnType, bool>::value,
                  "The predicate must return a boolean value");
    if (!option.is_empty() && func(option.get()))
    {
        return Some(option.get());
    }

    return None;
}

