// Types for testing
enum E : int { e_zero, e_one };
enum class EC : int { zero, one };
union U {
    int i;
};
struct C {
    int i;
    auto memberFunction(int) -> int {
        return ++i;
    }
    auto constMemberFunction(int) const -> int {
        return i;
    }
};
struct CFrom {
    operator C() {
        return C{};
    }
};
struct CFromExplicit {
    explicit operator C() {
        return C{};
    }
};
using MemberObject           = int C::*;
using ConstMemberObject      = const int C::*;
using MemberFunctionPtr      = int (C::*)(int);
using ConstMemberFunctionPtr = int (C::*)(int) const;
using Array                  = int[10];
using ArrayRef               = int (&)[10];
using ConstArrayRef          = const int (&)[10];
using ArrayPtr               = int (*)[10];
using ConstArrayPtr          = const int (*)[10];
using FunctionRef            = int (&)(int);
using FunctionPtr            = int (*)(int);


template<typename Signature>
struct DummyFunctor;

template<typename Ret, typename... Args>
struct DummyFunctor<Ret(Args...)> {
    auto operator()(Args...) {
        return Ret{};
    }
};

template<typename... Args>
struct DummyFunctor<void(Args...)> {
    void operator()(Args...) {
    }
};
