# C++ delegates

> **☑️COMPLETE**  
> This project is considered complete, and no further work will be done until requested otherwise.  
> Please [open an issue on GitLab](https://gitlab.com/rmettler/cpp_delegates/-/issues) if you miss a feature or found a bug.

[![pipeline status](https://gitlab.com/rmettler/cpp_delegates/badges/master/pipeline.svg)](https://gitlab.com/rmettler/cpp_delegates/-/commits/master)
[![coverage report](https://gitlab.com/rmettler/cpp_delegates/badges/master/coverage.svg)](https://gitlab.com/rmettler/cpp_delegates/-/commits/master)
[![latest release](https://gitlab.com/rmettler/cpp_delegates/-/badges/release.svg)](https://gitlab.com/rmettler/cpp_delegates/-/releases)

- [Introduction and goals](#introduction-and-goals)
- [Library content](#library-content)
  - [`rome::delegate`](#romedelegate)
  - [`rome::fwd_delegate`](#romefwd_delegate)
  - [`rome::event_delegate`](#romeevent_delegate)
  - [`rome::command_delegate`](#romecommand_delegate)
- [Documentation](#documentation)
- [Integration](#integration)
- [Tests](#tests)
  - [Configure CMake](#configure-cmake)
  - [Run tests](#run-tests)
- [Examples](#examples)
  - [Usage of `rome::delegate`](#usage-of-romedelegate)
  - [Usage of `rome::command_delegate` and `rome::event_delegate`](#usage-of-romecommand_delegate-and-romeevent_delegate)
- [Comparison with other solutions](#comparison-with-other-solutions)
- [Design decisions](#design-decisions)
- [Motive for this library](#motive-for-this-library)
- [Feedback](#feedback)
- [Thanks](#thanks)
- [References](#references)

## Introduction and goals

The _C++ delegates_ library provides lightweight function wrappers able to store and invoke any callable target without the caller having to know the details of the callee. Examples of such callable targets are functions, member functions, function objects or lambda expressions. They just need to support the function call signature specified with the delegate.

This library is single header, written in C++14, and following the design goals:

1. Safe to use:
   - No undefined behavior, no matter how the delegates are used.
   - Use compile-time checks instead of runtime-checks whenever possible.
2. Efficiency:
   - Ready to be used in memory and performance restricted software, e.g. on microcontrollers.
3. Easy to understand, what the caller expects from the callee.
4. Easy to use.
5. Configurable behavior for calls when no target was assigned.

The library is tested to be compatible and free of compiler warnings with:

- C++14, C++23
- Clang: 4.0, 17.0, 18.1
- GCC: 5.3, 13.2, 14.1
- MSVC: 19.40

## Library content

### `rome::delegate`

```cpp
delegate<bool(int)> d_exp;            // same as `delegate<bool(int), target_is_expected>`
d_exp(1);                             // throws exception, no target was assigned
d_exp = [](int i) { return i > 0; };
assert(d_exp(2) == true);             // ok

delegate<bool(int), target_is_optional> d_opt1;  // does not compile, non-void return
delegate<void(int), target_is_optional> d_opt2;  // ok
d_opt2(3);                                       // does nothing

delegate<bool(int), target_is_mandatory> d_mand1;  // does not compile, no target assigned
delegate<bool(int), target_is_mandatory> d_mand2 = [](int i) { return i > 0; };  // ok
assert(d_mand2(4) == true);                        // ok
```

The basic delegate which supports any callable target of any function signature. On call, calls a previously assigned target. By default, throws an exception when called without assigned target. This behavior is configurable by the following options:

- `rome::target_is_expected` _(default)_:  
  If no target was assigned before the `rome::delegate` is called, an exception is thrown.
- `rome::target_is_optional`:  
  If no target was assigned before the `rome::delegate` is called, it directly returns without doing anything (only works if the function call signature has `void` return).
- `rome::target_is_mandatory`  
  Ensures by design that there is always a target assigned to `rome::delegate`. For this, the default constructor is deleted and there is no possibility to drop an assigned target.

_See also the detailed documentation of [`rome::delegate`](doc/delegate.md) in [doc/delegate.md](doc/delegate.md)._

### `rome::fwd_delegate`

```cpp
fwd_delegate<bool()> d_nonvoid;       // does not compile, non-void return
fwd_delegate<void(int&)> d_mut;       // does not compile, mutable argument
fwd_delegate<void(const int&)> d_ok;  // ok
```

Provides the same functionality as `rome::delegate`, but with the restriction that data can only be **_forwarded_**. To ensure this, only function signatures with `void` return and arguments of immutable type are allowed. E.g. the signature `void(const std::string&)` would work, while `void(int*)` or `bool()` would produce a compile error.

 _See also the detailed documentation of [`rome::fwd_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

### `rome::event_delegate`

```cpp
event_delegate<void(int)> d;
d(1);                               // does nothing
d = [](int i) { std::cout << i; };
d(2);                               // prints "2"
```

A [`rome::fwd_delegate`](doc/fwd_delegate.md) that ignores calls if no target was assigned.

Designed for event or message-driven architectures, to notify about happened events. Thus, it is optional whether someone wants to listen to the event or not.

_See also the detailed documentation of [`rome::event_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

### `rome::command_delegate`

```cpp
command_delegate<void(int)> d_err;  // does not compile, no target assigned
command_delegate<void(int)> d_ok = [](int i) { std::cout << i; };  // ok
```

A [`rome::fwd_delegate`](doc/fwd_delegate.md) that ensures that always a target is assigned.

Designed for event or message-driven architectures to command an action that shall happen. Because the execution of the command is mandatory, a target must be assigned during construction of the delegate and can only be overridden by another target afterwards.

_See also the detailed documentation of [`rome::command_delegate`](doc/fwd_delegate.md) in [doc/fwd_delegate.md](doc/fwd_delegate.md)._

## Documentation

Please see the documentation in the folder `./doc`. Especially the following markdown files:

- [doc/delegate.md](doc/delegate.md)
- [doc/fwd_delegate.md](doc/fwd_delegate.md)

## Integration

- Add the folder `./include` to the include paths of your compiler or copy `./include/rome/delegate.hpp`.
- Include the delegates:
  
  ```cpp
  #include <rome/delegate.hpp>
  ```

If exceptions are disabled, the delegates will call `std::terminate()` instead (see also [doc/delegate.md](doc/delegate.md)).

The delegates depend on the following headers of the C++ standard library:

- `<algorithm>`
- `<cstddef>`
- `<exception>`
- `<new>`
- `<type_traits>`
- `<utility>`

## Tests

The tests can be found in [./test](./test).

### Configure CMake

```bash
cmake --preset clang-cpp14-instr -B build
```

Setting `ROME_DELEGATES_BUILD_TESTS=ON` configures the test targets. They are hidden otherwise.

Setting `ROME_DELEGATES_INSTRUMENT=ON` enables instrumentation for code coverage, address sanitizer (ASan) and undefined behavior sanitizer (UBSan). Instrumentation only works with Clang.

Both options are enabled with the CMake presets `clang-cpp14-instr` and `clang-cpp23-instr`. See also the [`CMakePresets.json`](./CMakePresets.json).

### Run tests

`cd build`

- `ninja run_unittest`  
  Test functionality and constraints of the delegates. The unit tests are built with `-Wall -Wextra -pedantic -Werror` or `/W4 /WX` for MSVC. Uses the unit test framework [doctest][doctest] and the mocking framework [Trompeloeil].  
  If `ROME_DELEGATES_INSTRUMENT` is enabled:
  - Prints errors of address sanitizer and undefined behavior sanitizer to stderr.
  - Creates coverage data.

- `ninja coverage`:  
  Build and run the unit tests, collect coverage results, print the results to console, and create coverage reports in `build/test/coverage`.

- `ninja run_compile_error_tests`:  
  Test delegates for expected compile errors.

- `ninja run_example_tests`:  
  Test that the provided examples are working.

- `ninja clang_tidy`:  
  Run clang-tidy code analysis over the delegates and the unit tests.

## Examples

### Usage of `rome::delegate`

Basic usage examples for all three types of `Behavior` and the three target types function, member function and function object.

_See the code in [examples/basic_examples.cpp](../examples/basic_examples.cpp)._  
_Or explore it online at [Compiler Explorer](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGIMxqkrgAyeAyYAHI%2BAEaYxCAAzKQADqgKhE4MHt6%2B/oGp6Y4CoeFRLLHxSXaYDplCBEzEBNk%2BfgG2mPZFDPWNBCWRMXGJtg1NLbntCmP9YYPlwwkAlLaoXsTI7BzmCWHI3lgA1CYJbggEBMkKIAD0N8RMAO4AdMCECF7RXkobsowEzzQLHubAu9GIN2QyWSAH0sPRgEwCJgFDcWExpnEbnsDph7qg2Dd4ZhEcjnghoSdsCYNABBHY4rxHE5uKheBi1ARiKk0%2BlmXYc3HHU5OabETCsHl03kAN1QeHQh2SxDCBAgqsOeCWxwA7FY6YdDYdpugQCg1gRhW4WZqrTawNsAKxuBgOk76%2Bk6gAivN5GuS3gUXA0GnVgk12pMet5RsO4oI6wYtush2DGndvu9vrpYq8DkOABVGsBMM0DAoFLqPbGNTKxF5MMKvYd0wkPTGjXKFcbS2HLVqqx3Y4a695Gydm3gMwajVGfdKs23s7Tc/nsKpWAHx9GZ4biAT2CBiaTMCzVX2VnGD2axiWCDC8AoYeiGOgkSQAJ5Uw4CACyhjfIhiA/CJUAIbAWGSAgvyXXcrzYM1jyRU8RUEC9DhuAAqUh4MPW9SwfJ9MFUZIamRdBMJub8BHXUiHEwdBQPAyDoPdQ07kOCAuCWIcMJuXDEM6ElkLPNDVUvfcEJAfD70fGFUCgzJuQSbAfwYf8P1iCCoJgixh2HDiIDMbV0FQFFDgYMDDiBZI6FPODJMPJDkRZLt0AvHDHJvYsCLkhTumU1S/yYTScBY3Tl1jddN3oCAsH2aDSIgYLX3fYCmO06CeLMAA2cwcsOF90G1EA1P/VKgJAsDMq/PUTTNFhUBlTAICKnjvSrdj%2BIgZZeLnTMfVg%2Bk6Q1dEwggSMd1pWMi2IO83HLStUGibQoysRdqyNaLIPoQ5VDW%2BqQHCE8WR5aMsw2/SrsM3rpTgw60C8S1TtOY4zDMNwxFoSsmCoZFiE1BgMjEPAAC8kUyEATGdBhzDMadpqNZVz1UZ4UsAz8MvCrilh4tsrr444co0Q7jpE04eWJw5HgxQ4MXSYBwnQXiCGAwc4NjVGaJIsiGKxnSjLxvSCaNDjRFoehFUwcK%2BqzODRAIZAEE4tAGGmASQGiJh0DhISTxhcXaHyw5MEmzb9Iei07VezBnkeBAkQm623EON0YbdIbY36uCufUkKtOx3r8ZFkOuusr6GJNmW7sRw1Lae523vhmHPolytHneQ4qHFBRlfpvBGcjmTrmhl04YR2MOPzxms/ZTkk2W7QyN433yox9LqvCptjkdKxHS9PsI06%2BNE2TSxU1bPT%2Bs90X%2BOrpM2Q5bofxWsjqYeaEwmAOna6XzIW7RhhaN5xjO50w0Jx7vuB41Ac1rjUtR4Db403VIXdUG83Cfn3f65Xpv8yPA3rZIwO82ALABoveuB9gqhRqgTS%2B0MrC5UbtDG%2B4Y756n/s8JQaotRsWnl/ZGaFW4ATSlVZiAtJC4wrkjFUJDD7H3oqfSh0EICOhoTPQ0xC1SN2eKOBsQsYF%2BzgdjHKQi4I8OSitfh9ZTYI14vHZ6FNXrlxTl9H6f04iHHQPuTeoDi5QxhuXLhhNSEVUxmfaC3cGBeAllBYgbFK7dWoTosylZLKWhsnZYRTDyL82sTYuxtAHG0MNKQ0R58rqX1sfY1mYSlT0LVOY9uFCaoQB1Jw4W%2BkOJQOXo3Ne0w6C0DphWAuTMWZszWrxTmjCebMICR%2BCAAAOd%2BocxYRyltHWOH9eIKyVirAQ6svKa21rrBEyEDZfWNqbdmPTYxKMTrbe2jtIwqJdm7F0Hsv7ex6REgOAsACcbTQ4iw6RLSO0sdK8SkXwgR8ihq7I4CsWgnBHS8D8NwXgqBODWksCmBQawNjjn5DwUgBBNDPJWAAaxAJISQzwuC5UdBoYmZhJAaEOTlBIzScr6E4JID5kLSA/I4Lwa4gQIUcC0CsOAsAYCIHNJBOyZAKAQG8eCFABgjBcGacGPgdB/rXAgNEYl0QwiNA/JwMF4rmDAQAPKrwcNK3gQI2CCHlQwWgUrqW8CwF8YAqdvoqtIFgF8wBxC6tNXgcUtRmrXCtcRGoT0thaHIIIToxLaB4GiA8YCHgsDEtZngFgJrmrEGiGkTAXppaGAtVvSFKwqAGGAAoAAangTAjx5WkU%2BWC/gggRBiHYFIGQghFAqHUFa3QXB9BxpQNYaw%2BgfXXEgCsfymQHUAFp5UJG%2BeGlUWBW0TQ6F0SGEBXATD8LWkIcwygVD0AUDIAgp2LrSMuhgAx53DFrdUeuvRxieFaHoPd3QD2zFKEMeIu6ZirpvX0LdV6JArEBesTYz78UcHeaQT5brSV7VxV2nKkhrLcu3ryxFzwNCcVwIQEgb0EjcV4FSmlKwEASiwPEEdryOCEtIKGx0jpnjYskIcgIJGyOoukL%2B75nByUgEpYm0gdLGUWmSE9cglAOVxAiKwLYjQWAyh1F2pgoG42pmaZBt1DE4ODr0AW4Q4sS3SAUxWtQxKa2kCAUwZIKqXlvKJVa0l8qnrsctKgKgAGcpAZA/scTEGuBQc4h4Zl4IENIfBYm/TuHeChvhc8BIOUUU6kOYF4LoW8U0ZJXR2wDHPO6txqQWFhHnjNJ1AkUjCRDk6kdIcjQgXS04b7T%2B4lpLkNec/WYQzf6YsoahaQcN6RnCSCAA%3D%3D)._

```cpp
#include <functional>
#include <iostream>
#include <rome/delegate.hpp>

void print(int i) {
    std::cout << i << '\n';
}

int plus100(int i) {
    return i + 100;
}

struct TargetClass {
    int value = 0;

    void set(int i) {
        value = i;
    }
};

struct Example {
    rome::delegate<int(int), rome::target_is_mandatory> onMandatoryNotEmpty;
    rome::delegate<int(int) /*, rome::target_is_expected*/> onExpectedNotEmpty;  // (1)
    // rome::delegate<int(int), rome::target_is_optional> onMaybeEmpty;          // (2) does not compile
    rome::delegate<void(int), rome::target_is_optional> onMaybeEmpty;

    Example(decltype(onMandatoryNotEmpty)&& mand) : onMandatoryNotEmpty{std::move(mand)} {  // (3)
    }
};

int main() {
    TargetClass obj{};
    Example x{std::negate<>{}};             // (3)

    std::cout << "Calls after initialization:\n";
    print(x.onMandatoryNotEmpty(1));        // `std::negate<>` was assigned
    try {
        x.onExpectedNotEmpty(2);            // called empty
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(3);                      // called empty

    std::cout << "\nCalls with fresh assigned targets:\n";
    // assign function object
    x.onMandatoryNotEmpty = [](int i) { return i + 10; };
    // assign function object wrapping a function
    x.onExpectedNotEmpty  = [](int i) { return plus100(i); };
    // assign function object wrapping a member function
    x.onMaybeEmpty        = [&obj](int i) { obj.set(i); };
    print(x.onMandatoryNotEmpty(4));
    print(x.onExpectedNotEmpty(5));
    print(obj.value);
    x.onMaybeEmpty(6);
    print(obj.value);

    std::cout << "\nCalls after dropping targets:\n";
    // x.onMandatoryNotEmpty = nullptr;     // (4) does not compile
    x.onExpectedNotEmpty  = nullptr;
    x.onMaybeEmpty        = nullptr;
    print(x.onMandatoryNotEmpty(7));        // function object still assigned
    try {
        x.onExpectedNotEmpty(8);            // called empty
    }
    catch (const rome::bad_delegate_call& e) {
        std::cout << e.what() << '\n';
    }
    x.onMaybeEmpty(9);                      // called empty
    print(obj.value);
}
```

- **(1)** - second template parameter is `rome::target_is_expected` by default
- **(2)** - `rome::delegate` with `rome::target_is_optional` must have void return
- **(3)** -  `rome::delegate` with `rome::target_is_mandatory` has deleted default constructor, a _target_ must be assigned during construction
- **(4)** - `rome::delegate` with `rome::target_is_mandatory` does not allow to drop _targets_

Output:

> Calls after initialization:  
> -1  
> rome::bad_delegate_call  
>
> Calls with fresh assigned targets:  
> 14  
> 105  
> 0  
> 6  
>
> Calls after dropping targets:  
> 17  
> rome::bad_delegate_call  
> 6

### Usage of `rome::command_delegate` and `rome::event_delegate`

Model of an extremely simplified cruise control system. The four classes _Engine_, _BrakingSystem_, _SpeedSensor_ and _CruiseControl_ are atomic, i.e., are free from dependencies to other classes. _Integration_ integrates all four.

_See the code in [examples/cruise_control.cpp](../examples/cruise_control.cpp)._  
_Or explore it online at [Compiler Explorer](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGIAMwAbKSuADJ4DJgAcj4ARpjEEsEADqgKhE4MHt6%2BASlpGQLhkTEs8YlcwXaYDplCBEzEBNk%2BfkG2mPaOAvWNBMXRcQlJtg1NLbntCmP9EYNlw5UAlLaoXsTI7Bzm/hHI3lgA1Cb%2BbggEBMkKIAD0N8RMAO4AdMCECF6xXkobsowEzzQLHubAu9GIN2QyWSAH0sPRgEwCJgFDcWExpgkbnsDph7qg2Dd4ZhEcjnghoSdsCYNABBHY4rxHE5uJzTYiYVhUmn0sy7Bj7JmYY6nLyOWiEACe3LpPPZXgch2wRjmxwA7FY6YdtYcAG6oPDoQ5MZCbcFIzAQJbqzW0nX2w7TdAgFBrAgitws45mMyMN6RY2mzoJJERYAmACsbgY5jMJ1t9pMaoAIjz7frDYcCOsGAB5KhUK02tMO7VOl1oMUer2xv2q7PESJG1AFyPR2PxkvapOp2UpzuyunyxUWB4AazDQklmJYxa1OozRobDFHTDHKNzDCLSYTpcdBGdrqrLJrPti45RhwEbZjPoHdp1Pa7eoNS5zq/XCnzhetO%2Bf9vLI93RPU5vTMc810vFsqBvDt/F3bt%2Bz7VN4J5OUCGIBV3SEZJMEwdAhEYBQSDnB9tTuQ5aQUdJgAYMNDmJUlhTwBQr2SbpmFoQ5%2BGIR1cPwx0iJIohDkeEgx2eZ9iAJdgQEwXV/jhYMmJZRdqFoVAkSWKkrwYHC8PcBBDGAfD7yfVDB1pfYMVYtxMJYzAPEEaSuL/edtSoDSkSzRoTOEfj0BhEVk0ODRng0AAxMz3MOCiqJouijEOWJUAIBAGOUi1WJYw4OQARy8PAOSNHjDmQeylDK2QXKzVBRPEySYuktgKwJdEGECxiLVU18rR0gQrLNENkTcRoYXve1mtkoF2s6zLkR6w0%2Bv8bBdKEDTHmTVBHiyMboofZJPglZAQGfRdDi8ZJ0AtQbgweDiIDQBhpm4rz3XQYg8F1ScAt/DV/x1PAqEOCAPq%2Bn6DI9HziD8/T8KCgAqULngATgjCK/oQvdtQGoNzRGsblqxxMkLIh1OkqoGQbB76jDho0dLGWGAsRw4uHC9HMYB0seg2radtG4gYSJ7n1V7Mmn0s9NX0EggABVfMwfyDMegQXs8zT3SZpX6a5mL7W15X4eC6HmYMibH1J587MKpQnIwzwICm1qWFmpSEW6041O0lbzECP3dNu/HHMaUhRed11XcMOaPYWr3ep96kzH95O1r57bduIbT9Z1EBA7x4aQ6FndAJYVAFIgXGhvuovtJTUg0%2B2/nM/GjVS/Ly1eabjPBbrkK3Ilq3%2Bws%2BkhwwrDDgASUEEl7syUj7XpwjnpIhQAuX4jiAt7UbYc%2B2avK23HOqzxt6VFUAzrSIz4/Sdp2RWcIM/fb7Wn5FgDngQ%2Bpz7U88PveT60AHtjRMEYLBpRYpGZM25/o/xAU/FEzxlwfg3AWEWcDsZX0wM8E01cLToLJtjHsYcMGPjARAhQUCYHExAdqLBSCczfgIbQ0sCCFAMMbCgr8W4fY0NLE%2BFMC89xrwMhvEgzwegBTOMZASJx%2B7kIQJAiM0CNbeRphDfCetCF7n/nbQBzxLrXWREHYamRQafVpsAXWZ8SYoSxpLekw9bQ8jfrPUMAhDgRHfp/GM/0nFoTpF4w46IIjUOfBRIQeAWBeAMMiKeuZDgMFSlVBgkQHACQiFmRRrFMCqFYMkegz4vFuI4oCCqx9nKeGeEoeWisjboAgGYCM4UMZn2KR/dxDBqnryEsQCRekpFGSMPhRpYVIq8KKTPDppSRH4TEX0yRBlpHDIaU0lpEyYrtJ8d00RvT%2Bn02WSZBp/gxmtJHg4jgKxaCcAjLwPw3BeCoE4J6Sw1hHRrA2MKHYPBSAEE0JclYY4QCSEkM8LgycIwaECBoMwkgNAo0CP4AAHMEa5HBJB3P%2BaQJ5HBeDXA0L8/5Kw4CwBgIgSOyQ6AJHIJQIElLwQoAMEYLgSKuAEpoLQZExBrgQFiFi2IERGiSk4D8gVzBiCSlzLEbQNQ/kPNIDNf4m5aDCo4FoUgWAvjAFGrQWg1x5VYHasAcQareD4A5LUBS%2Br1W5JqGKLY6rilovVRKCCEqPBYCxRhKJIreAKWIClJQyZMBGolEYIlfADDAAUAANTwJgR4uZcL3J%2BfwQQIgxDsCkDIQQigVDqFNaQXQXB9DGRQNYaw%2Bg8CxGuJAFYqB2KZH1QAWlzP4R5/rPpYFrVaDoXRMguA6hMPwJawhzFKOUPQqR0gcWHVOgoHEBgTsWH22VdQZhzpLdUWoPQZhLqGBUUYfRN1HqaPuhYFQVjEXWJsCQVybmYsLTiw4qgUXNsCJIMqTLgBsyRWC8KINcCEBIt8pYvA5VaCWCsBAnIsCJF7WijFpAWDArVM8JFEY1RSDMBhrDkgcPBHueqnFeKQAEog5c0gJLyVukOgQGlqsWD0oSFEVgWxGgsF1GqZtTAv3GV/f%2Bm1%2BAiBdr0Gm4QohxDZvE3mtQWLi2kEeA8ZIvr70cFuaQIjjzOC5jFHRq8wNX2BHfZ%2B6ySUWWCZBh4JjVLeKgfA0StTSGUMRjMGC/DuHsOeekFp7FnBSPkcc6QIFEZmlIrVP4SQKN/AozVBGFGGggjZrRe2zTWKSOEtNVB/QnAzCPuI/5zLkGVj%2BvSM4SQQA)._

```cpp
#include <iostream>
#include <utility>
#include <rome/delegate.hpp>

struct Engine {
    void accelerate() {
        std::cout << "engine accelerating\n";
    }
    void turnOff() {
        std::cout << "engine turned off\n";
    }
};

struct BrakingSystem {
    void turnBrakesOn() {
        std::cout << "brakes on\n";
    }
    void turnBrakesOff() {
        std::cout << "brakes off\n";
    }
};

struct SpeedSensor {
    // Assigning delegate is optional for speed sensor to work.
    rome::event_delegate<void(float)> onSpeedChanged;
};

class CruiseControl {
    float targetSpeed_ = 0.0F;

    // Assigning both delegates is required for cruise control to work.
    rome::command_delegate<void()> onAccelerateCar_;
    rome::command_delegate<void()> onSlowDownCar_;

  public:
    void updateAcceleration(const float drivingSpeed) {
        if (drivingSpeed < targetSpeed_ * 0.95F) {
            onAccelerateCar_();
        }
        else if (drivingSpeed > targetSpeed_ * 1.05F) {
            onSlowDownCar_();
        }
    }

    void setTargetSpeed(const float targetSpeed) {
        targetSpeed_ = targetSpeed;
    }

    CruiseControl(rome::command_delegate<void()>&& onAccelerateCar,
        rome::command_delegate<void()>&& onSlowDownCar)
        : onAccelerateCar_{std::move(onAccelerateCar)}, onSlowDownCar_{std::move(onSlowDownCar)} {
    }
};

struct Integration {
    SpeedSensor speedSensor;
    CruiseControl cruiseControl;
    Engine engine;
    BrakingSystem brakes;

    Integration()
        : cruiseControl{
            [this]() {
                brakes.turnBrakesOff();
                engine.accelerate();
            },
            [this]() {
                engine.turnOff();
                brakes.turnBrakesOn();
            }} {
        speedSensor.onSpeedChanged = [this](float drivingSpeed) {
            cruiseControl.updateAcceleration(drivingSpeed);
        };
    }
};

Integration integration{};

int main() {
    // Simulate IO not connected in this example
    integration.cruiseControl.setTargetSpeed(25.0F);
    integration.speedSensor.onSpeedChanged(20.0F);
    integration.speedSensor.onSpeedChanged(25.0F);
    integration.speedSensor.onSpeedChanged(30.0F);
}
```

Output:

> brakes off  
> engine accelerating  
> engine turned off  
> brakes on

## Comparison with other solutions

Similar C++ standard library counterparts in behavior and interface are:

- `std::move_only_function` (C++23)  
  Very similar by constraints and efficiency. However, it is undefined behavior when a `std::move_only_function` is called empty. For _C++ delegates_, this behavior is always defined and configurable.
- `std::function` (C++11)  
  Copyable but less efficient. Throws an exception when called empty.

## Design decisions

- **Why do delegates take ownership of assigned function objects?**  
  Let's take the example `delegate<void()> d = []() {};`. If the delegate would only reference the function object created by the lambda expression, it may easily happen, that the assigned function object is already destroyed when the delegate is called. To make safe usage of the delegate easy and unsafe usage hard, the delegate takes the ownership of the function object.  
  Note that it is still necessary to manage the lifetime of objects that the assigned function object captures by reference.

- **Why is the size of the delegate `sizeof(void*) + 2*sizof(void(*)())`?**
  
  - One function pointer stores the function that invokes the target.
  - One function pointer stores the function that destroys an assigned function object. This happens either when the delegate is destroyed or when a target is dropped.
  - The object pointer stores the address where the function object is stored. Or, with small object optimization, the function object is stored within the memory of the object pointer.
  
  Thus, the size is kept at the required minimum, with memory restricted devices in mind.

- **Why can't I copy delegates, why are they move-only?**
  
  - It would need another function pointer stored within the delegate that increases its size significantly.
  - Copying the delegate may only lead to a shallow copy of the target, but a deep copy might be needed. Because the delegate hides the assigned target, this issue is invisible.

  As a result, the _C++ delegates_ are move only. If you need multiple instances of the same delegate, just create multiple delegates and assign the same target.

- **Why can't I directly assign a function to a _C++ delegate_, as for example with `std::move_only_function`?**  
  Because it is less optimizable and may lead to less efficient code.

  - With direct assignment `delegate<void()> d = &function;`:  
    The address of the function or member function is passed to the assignment operator's function argument. Thus, it is only known during runtime.
  - When wrapped by a function object `delegate<void()> d = []() { function(); };`:  
    The function is now bound to the type of the function object. Therefore, it is known during compilation time.

- **Why is the size for small object optimization `sizeof(void*)`?**  
  When a big function object is assigned to a _C++ delegate_, the delegate needs to store the object in a dynamic allocated memory and remember that location with a pointer (`sizeof(void*)`). If, however, the function object is smaller or equal to the pointer, the space of the pointer can instead be used to store the function object in. This enables to small buffer optimize any lambda expression that only captures a reference or a pointer. Additional data may be accessed through that reference/pointer.  
  As a result, dynamic allocation should be avoidable in any use case without increasing the size of the delegate.

- **Why is the namespace called _rome_?**  
  It has nothing to do with the Italian capital, it's just the initials of my name.

## Motive for this library

The initial motivation was the wiring between independent modules.  
Let `A` and `B` be two modules. `A` is reactive and may produce the event `done`. When `A` produces the event `done`, `B` shall `start`.

```cpp
namespace module_a {
    struct A {
        /* something that produces the event `done` */
    };
}

namespace module_b {
    struct B {
        void start();
    };
}

void wire() {
    // connect them somehow
}
```

In OOP, this is commonly done by `A` having an association to an interface `I` and `B` realizing that interface. `B` is then injected into `A` by its base class `I`.  
It might look something like the following:

```cpp
namespace awkward {
    struct I {
        virtual void done() = 0;
    };
}

namespace module_a {
    struct A {
        awkward::I& done;
    };
}

namespace module_b {
    struct B : public awkward::I {
        void done() override;  // but should be called `start`
    };
}

void wire() {
    module_b::B b{};
    module_a::A a{b};
}
```

The consequences are:

- The code of `I` needs to be stored somewhere, either in module `A`, module `B`, or a third module.
  - If `I` is stored in `A` or `B`, one module depends on the other. They are not independent anymore.
  - If `I` is stored in a third module, that third module is created just for that purpose, while `A` and `B` still are not as independent anymore as without.
- By realizing `I` in `B`, `I` becomes a public interface of some class in `B`. However, the function names of `I` might be a bad fit for the public interface of `B`, so you need to add an extra class and hide it somehow, just to realize `I`.

A better solution is to use `std::function` for the event `done`:

```cpp
namespace module_a {
    struct A {
        std::function<void()> done;
    };
}

namespace b {
    struct B {
        void start();
    };
}

void wire() {
    module_a::A a{};
    module_b::B b{};
    a.done = [&b]() { b.start(); };
}
```

While already a lot better than the OOP interface headache, `std::function` isn't exactly lightweight. On memory restricted devices this might become a problem. Furthermore, `std::function` throws an exception if no target was assigned. This enforces to assign a target to any provided event, whether handling that event is of interest or not. And if the handling of an event is required, a compile error is preferable to a runtime exception.

So, a lightweight solution similar to `std::function` is needed, that enables marking the handling of an event as optional or mandatory, with unhandled optional events doing nothing and unhandled mandatory events raising a compile error.

That's where my journey started and the reason for the [`rome::event_delegate`](doc/fwd_delegate.md) and the [`rome::command_delegate`](doc/fwd_delegate.md).

## Feedback

Do you have any request, a question or found a bug? Please [open an issue on GitLab](https://gitlab.com/rmettler/cpp_delegates/-/issues/new) or write me an email. I am also happy to hear from any positive or negative experience you had with this library.

## Thanks

- Sergey Ryazanov for his incredible article [_The Impossibly Fast C++ Delegates_][impossDelegates]. He explains how you can use the fact that the address of any function can be passed as non-type template argument to create highly optimizable function delegates.
- Lee David for help finding more comprehensible names.
- Matthias Deimbacher for the initial set up of the CI pipelines at GitLab.

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates, MIT.
2. Cppreference. [_std::move_only_function_][std_move_only_function], cppreference.com, 14 November 2023, https://en.cppreference.com/w/cpp/utility/functional/move_only_function, CC-BY-SA.
3. Cppreference. [_std::function_][std_function], cppreference.com, 14 November 2023, https://en.cppreference.com/w/cpp/utility/functional/function, CC-BY-SA.
4. Kirilov, Viktor. [_doctest_][doctest], 15 March 2023, https://github.com/onqtam/doctest, MIT.
5. Fahler, Björn. [_Trompeloeil_][trompeloeil], 21 July 2023, https://github.com/rollbear/trompeloeil, BSL-1.0.

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
[std_move_only_function]: https://en.cppreference.com/w/cpp/utility/functional/move_only_function
[std_function]: https://en.cppreference.com/w/cpp/utility/functional/function
[doctest]:  https://github.com/onqtam/doctest
[trompeloeil]: https://github.com/rollbear/trompeloeil
