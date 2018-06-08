/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

#include <upl/pointer.h>

#include <iostream>

#if defined (__cpp_concepts)
template <class P>
void printPointer(const P& strong_pointer)
requires upl::StrongPointer<P>
{
    using namespace std;

    if constexpr (upl::OptionalPointer<P>)
    {
        if (strong_pointer)
            cout << "Strong optional pointer value = " << *strong_pointer << endl;
        else
            cout << "Strong optional pointer is empty" << endl;
    }
    else
    {
        cout << "Strong single pointer value = " << *strong_pointer << endl;
    }
}

void printPointer(const upl::WeakPointer& weak_pointer)
{
    using namespace std;

    auto locker = weak_pointer.lock();

    if (locker)
        cout << "Weak pointer value = " << *locker << endl;
    else
        cout << "Weak pointer is empty" << endl;
}

void printAnyPointer(const upl::Pointer& any_pointer)
{
    using namespace std;

    cout << "any printer:\n    ";

    printPointer(any_pointer);
}

void printPointerByType(const upl::Pointer<std::string>& string_pointer)
{
    using namespace std;

    cout << "std::string printer:\n    ";
    printPointer(string_pointer);
}

void printPointerByType(const upl::Pointer<double>& double_pointer)
{
    using namespace std;

    cout << "double printer:\n    ";
    printPointer(double_pointer);
}
#else // defined (__cpp_concepts)
template <class P, UPL_CONCEPT_REQUIRES_(upl::StrongPointer<P>)>
void printPointer(const P& strong_pointer)
{
    using namespace std;

    if constexpr (upl::OptionalPointer<P>)
    {
        if (strong_pointer)
            cout << "Strong optional pointer value = " << *strong_pointer << endl;
        else
            cout << "Strong optional pointer is empty" << endl;
    }
    else
    {
        cout << "Strong single pointer value = " << *strong_pointer << endl;
    }
}

template <class P, UPL_CONCEPT_REQUIRES_(upl::WeakPointer<P>)>
void printPointer(const P& weak_pointer)
{
    using namespace std;

    auto locker = weak_pointer.lock();

    if (locker)
        cout << "Weak pointer value = " << *locker << endl;
    else
        cout << "Weak pointer is empty" << endl;
}

template <class P, UPL_CONCEPT_REQUIRES_(upl::Pointer<P>)>
void printAnyPointer(const P& any_pointer)
{
    using namespace std;

    cout << "any printer:\n    ";

    printPointer(any_pointer);
}

template <class P,
          class = std::enable_if_t<upl::Pointer<P, std::string>>>
void printPointerByType(const P& string_pointer)
{
    using namespace std;

    cout << "std::string printer:\n    ";
    printPointer(string_pointer);
}

template <class P>
std::enable_if_t<upl::Pointer<P, double>>
printPointerByType(const P& double_pointer)
{
    using namespace std;

    cout << "double printer:\n    ";
    printPointer(double_pointer);
}
#endif // defined (__cpp_concepts)

int main()
{
    using namespace std;
    using namespace upl;

    upl::unique<string> unique_string{in_place, "Hello, optional unique!"};
    weak<string>        weak_string{unique_string};

    shared<double>        shared_1{in_place, 42.43};
    shared_single<double> shared_2{shared_1};
    unified<double>       unified_3{shared_2};

    printAnyPointer(unique_string);
    printAnyPointer(weak_string);
    printAnyPointer(shared_1);
    printAnyPointer(shared_2);
    printAnyPointer(unified_3);

    printPointerByType(unique_string);
    printPointerByType(weak_string);
    printPointerByType(shared_1);
    printPointerByType(shared_2);
    printPointerByType(unified_3);

    unique_string.reset();

    printAnyPointer(unique_string);
    printAnyPointer(weak_string);

    return 0;
}
