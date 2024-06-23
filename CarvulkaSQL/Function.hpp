#pragma once

template<typename R, typename Arg>
class Function
{
private:
    struct Callable
    {
        virtual ~Callable() = default;
        virtual R operator()(Arg arg) const = 0;
        virtual Callable* clone() const = 0;
    };

    template<typename F>
    struct CallableImpl : public Callable
    {
        F function;

        CallableImpl(const F& function) : function(function) {}

        R operator()(Arg arg) const override
        {
            return function(arg);
        }

        Callable* clone() const override
        {
            return new CallableImpl<F>(function);
        }
    };

    Callable* callable;
public:
    template<typename F>
    Function(const F& function) : callable(new CallableImpl<F>(function)) {}

    Function(const Function<R, Arg>& other) : callable(other.callable->clone()) {}

    Function(Function<R, Arg>&& other) noexcept : callable(other.callable)
    {
        other.callable = nullptr;
    }

    ~Function()
    {
        delete callable;
    }

    Function<R, Arg>& operator = (const Function<R, Arg>& other)
    {
        if (this != &other)
        {
            delete callable;
            callable = other.callable->clone();
        }
        return *this;
    }

    Function<R, Arg>& operator = (Function<R, Arg>&& other) noexcept
    {
        if (this != &other)
        {
            delete callable;
            callable = other.callable;
            other.callable = nullptr;
        }
        return *this;
    }

    R operator()(Arg arg) const
    {
        return (*callable)(arg);
    }
};