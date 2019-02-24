# C++ delegates

An enhanced solution for C++ delegates, based on C++14.

It is inspired by the approach Sergey Ryazanov describes in the article [_The Impossibly Fast C++ Delegates_][impossDelegates] but is completly rewritten for better usability and makes use of modern C++14 features.

## Features
- **delegate**: The original one.
- **event_delegate**: Only ´void´ is allowed as return type with the advantage that calls to unlinked delegates are simply ignored and don't lead to undefined behavior.
- **static_delegate**: An even faster solution, but the functions or methods need to be linked at compile time. Only the objects to call methods are still resolved during runtime.

## References

1. Ryazanov, Sergey. “The Impossibly Fast C++ Delegates.” [_The Impossibly Fast C++ Delegates_][impossDelegates], CodeProject, 18 July 2005, www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates. 

[impossDelegates]: https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates