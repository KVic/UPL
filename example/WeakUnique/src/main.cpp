/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

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
