# UPL: Unified Pointer Library

Библиотека унифицированных указателей (UPL) содержит [концепты](Reference.md#Концепты-указателей) и реализации умных [указателей](Reference.md#Указатели), которые предназначены для управления временем жизни объектов. Предоставляет указатели `upl::unique` и `upl::shared` для уникального и совместного владения объектами, слабые ссылки для них `upl::weak`, и добавляет унифицированный тип владения `upl::unified`. Публичный интерфейс унифицированных указателей схож с интерфейсом умных указателей стандартной библиотеки C++.

UPL предоставляет расширенную семантику для концепций [владения](TheoreticalBasis.md#Владение) и управления временем жизни объектов, по сравнению с умными указателями стандартной библиотеки C++. Больше внимания уделяет указателям с уникальным владением (слабые ссылки для них, возможность передачи в функторы, которые требуют копирование аргументов (std::function)), обеспечивает продление времени жизни объекта в заданной области видимости, добавляет указатели с одинарной [кратностью](TheoreticalBasis.md#Кратность), которые всегда ссылаются на один объект.

Указатели UPL не являются заменой умных указателей стандартной библиотеки C++ и могут использоваться совместно с ними (конечно, одновременно объект может находиться под управлением только одной библиотеки). [UPL предназначена](Reference.md#Область-применения) для случаев, когда не хватает функциональности умных указателей стандартной библиотеки C++ и требуются дополнительные возможности для организации связей между объектами в многопоточной среде.

# Ключевые особенности

* Возможность организации [ассоциативных связей](TheoreticalBasis.md#Ассоциативные-связи) между объектами в соответствии с UML.
* Определены [концепты](Reference.md#Концепты-указателей), которые в compile-time позволяют гибко определять тип указателей UPL в обобщённых алгоритмах ([пример ConceptPrinter](https://gitlab.com/UnifiedPointers/Example/ConceptPrinter/blob/master/src/main.cpp)).
* Указатель `upl::weak` может ссылаться на объект, который находится под управлением `upl::unique` ([пример](#Пример-использования)).
* Указатель `upl::unified` позволяет передать уникальное владение объектом в цепочке, где может выполняться копирование ([пример TransferUnique](https://gitlab.com/UnifiedPointers/Example/TransferUnique/blob/master/src/main.cpp)).
* С помощью `upl::unified` можно временно продлить время жизни объекта в заданной области видимости, что позволяет корректно завершить работу с ним, даже когда все остальные указатели на этот объект удалены ([пример](#Пример-использования)).
* Добавлены указатели с одинарной [кратностью](TheoreticalBasis.md#Кратность), которые не могут быть пустыми и всегда ссылаются на один объект.

# Пример использования

Этот пример демонстрирует возможности уникального указателя `upl::unique`, слабой ссылки `upl::weak` для него, а также использование унифицированного указателя `upl::unified` для временного продления времени жизни объекта.

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

# Документация

* [Справка](Reference.md)
* [Теоретическая основа](TheoreticalBasis.md)
* [Примеры](https://gitlab.com/UnifiedPointers/Complex/Example)

# Использование библиотеки

UPL состоит только из заголовочных файлов (header-only).

Для использования библиотеки необходимо скопировать папку [include/upl](include/upl) в доступное для компилятора место, добавить полученный к `include` путь в *include paths* компилятора, и в файле исходного кода подключить заголовочный файл: `#include <upl/pointer.h>`.

# Внешние зависимости

Отсутствуют.

# Требования

Компилятор с поддержкой стандарта C++17.

# Поддерживаемые компиляторы

Работа библиотеки проверялась со следующими компиляторами:
* GCC 7.3.1
* Clang 6.0.0

# Статус разработки

Альфа-версия, проверка концепции. API библиотеки и реализация может сильно меняться.
