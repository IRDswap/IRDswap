
#ifndef BEAST_TEST_FAIL_COUNTER_HPP
#define BEAST_TEST_FAIL_COUNTER_HPP

#include <beast/core/error.hpp>
#include <boost/throw_exception.hpp>

namespace beast {
namespace test {

enum class error
{
    fail_error = 1
};

namespace detail {

class fail_error_category : public boost::system::error_category
{
public:
    const char*
    name() const noexcept override
    {
        return "test";
    }

    std::string
    message(int ev) const override
    {
        switch(static_cast<error>(ev))
        {
        default:
        case error::fail_error:
            return "test error";
        }
    }

    boost::system::error_condition
    default_error_condition(int ev) const noexcept override
    {
        return boost::system::error_condition{ev, *this};
    }

    bool
    equivalent(int ev,
        boost::system::error_condition const& condition
            ) const noexcept override
    {
        return condition.value() == ev &&
            &condition.category() == this;
    }

    bool
    equivalent(error_code const& error, int ev) const noexcept override
    {
        return error.value() == ev &&
            &error.category() == this;
    }
};

inline
boost::system::error_category const&
get_error_category()
{
    static fail_error_category const cat{};
    return cat;
}

} 

inline
error_code
make_error_code(error ev)
{
    return error_code{
        static_cast<std::underlying_type<error>::type>(ev),
            detail::get_error_category()};
}


struct fail_error_code : error_code
{
    fail_error_code()
        : error_code(make_error_code(error::fail_error))
    {
    }

    template<class Arg0, class... ArgN>
    fail_error_code(Arg0&& arg0, ArgN&&... argn)
        : error_code(arg0, std::forward<ArgN>(argn)...)
    {
    }
};


class fail_counter
{
    std::size_t n_;
    error_code ec_;

public:
    fail_counter(fail_counter&&) = default;

    
    explicit
    fail_counter(std::size_t n,
            error_code ev = make_error_code(error::fail_error))
        : n_(n)
        , ec_(ev)
    {
    }

    void
    fail()
    {
        if(n_ > 0)
            --n_;
        if(! n_)
            BOOST_THROW_EXCEPTION(system_error{ec_});
    }

    bool
    fail(error_code& ec)
    {
        if(n_ > 0)
            --n_;
        if(! n_)
        {
            ec = ec_;
            return true;
        }
        ec.assign(0, ec.category());
        return false;
    }
};

} 
} 

namespace boost {
namespace system {
template<>
struct is_error_code_enum<beast::test::error>
{
    static bool const value = true;
};
} 
} 

#endif






