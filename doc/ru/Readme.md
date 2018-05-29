# UPL: Unified Pointer Library

Библиотека унифицированных указателей (UPL) содержит концепты и реализации умных указателей, которые предназначены для управления временем жизни объектов. Предоставляет указатели для уникального и совместного владения объектами, слабые ссылки для них, добавляет унифицированный тип владения. Публичный интерфейс унифицированных указателей схож с интерфейсом умных указателей из стандартной библиотеки C++.

UPL предоставляет расширенную семантику для концепций владения и управления временем жизни объектов, по сравнению с умными указателями из стандартной библиотеки C++. Больше внимания уделяется объектам с уникальным владением (слабые ссылки на них; продление времени жизни в заданной области видимости; возможность передачи в функторы, которые требуют копирование аргументов(лямбды, std::function)), добавлены указатели с кратностью 1 (точно содержат один объект), и другие возможности.

Указатели из UPL не являются заменой умных указателей из стандартной библиотеки C++ и могут использоваться совместно с ними (конечно, одновременно объект может находиться под управлением только одной библиотеки). UPL предназначена для случаев, когда не хватает функциональности умных указателей из стандартной библиотеки C++ и требуются дополнительные возможности.

UPL состоит только из заголовочных файлов (header-only).

# Быстрый пример

Этот пример демонстрирует возможности уникального указателя на объект, слабой ссылки на него, а также использование унифицированного указателя для временного продления времени жизни объекта.

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

# Текущее состояние

Альфа-версия, проверка концепции.
