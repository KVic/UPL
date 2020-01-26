The Unified Pointer Library is obsolete, please migrate to the [C++ Object Token Library](https://gitlab.com/CppObjectTokens/Module/Library/CppOtl).

# UPL: Unified Pointer Library

The Unified Pointer Library (UPL) contains concepts and implementations of smart pointers that are designed to manage the object lifetime. The library provides pointers for unique and shared ownership of objects, weak reference for them, adds a unified type of ownership. The public interface of the pointers from the UPL is similar to the interface of the smart pointers from the C++ Standard Library.

UPL provides advanced semantics for concepts of ownership and management of the object lifetime, compared to smart pointers from the C++ Standard Library. More attention is paid to objects with unique ownership (weak references to them, extending the lifetime in a given scope, the ability to transfer to functors that require copying arguments (lambda, std::function)), pointers with a multiplicity of 1 are added (exactly contain one object), and other features.

Pointers from UPL are not a replacement for smart pointers from the C++ Standard Library and can be used together with them. The current realization of the UPL pointers is implemented as wrappers over the std :: shared_ptr and std :: weak_ptr pointers. This means that the performance of UPL pointers is the same as std :: shared_ptr/weak_ptr, and it is also possible to integrate them. UPL pointers can be created from pointers of the standard C ++ library. In the opposite direction, only std::shared_ptr can be created from upl::shared. UPL is intended for cases when there is not enough functionality of smart pointers from the C++ Standard Library and additional capabilities are required.

UPL is a header-only library.

# Quick example

This example demonstrates the capabilities of a unique object pointer, a weak reference to it, and the use of a unified pointer to temporarily prolong the lifetime of an object.

```
#include <upl/pointer.h>

#include <iostream>

void consumeUnique(upl::unique<std::string> unique_string)
{
    using namespace std;

    if (unique_string)
    {
        cout << "Consume the unique string: " << *unique_string << endl;
        unique_string.reset();
    }
    else
    {
        cout << "The unique string is empty" << endl;
    }

    assert(!unique_string);
}

int main()
{
    using namespace std;
    using namespace upl;

    upl::unique<string> hello_unique{in_place, "Hello, World!"};
    weak<string>        hello_weak = hello_unique;

    cout << "Initial unique string: " << *hello_unique << endl;

    {
        // Prolong the lifetime of the string from the hello_unique in this scope.
        unified<string> hello_unified = hello_weak;

        consumeUnique(std::move(hello_unique));

        assert(!hello_unique);
        assert(hello_unified);
        assert(!hello_weak.expired());

        cout << "The unique string is empty" << endl;
        cout << "But the weak string is still alive: "
             << *hello_weak.lock() << endl;
    }

    assert(hello_weak.expired());
    cout << "And here the weak string has expired" << endl;

    return 0;
}
```

# Current state

Alpha version, proof of concept.
