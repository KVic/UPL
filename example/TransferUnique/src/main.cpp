/*
 * Copyright (c) 2018 Viktor Kireev
 * Distributed under the MIT License
 */

#include <upl/pointer.h>

#include <memory>
#include <thread>
#include <iostream>

#define THREAD_ID "[" << std::this_thread::get_id() << "] "

struct Engine
{
    Engine(std::string model) : model{model} {}

    Engine(const Engine& other) = delete;
    Engine&     operator=(const Engine& other) = delete;

    std::string model;
};

class StdCar
{
public:
    StdCar(std::unique_ptr<Engine> engine)
        : m_engine{std::move(engine)}
    {}

    void replaceEngine(std::unique_ptr<Engine> engine)
    {
        using namespace std;

        string old_model = (*m_engine).model;
        m_engine = move(engine);
        cout << THREAD_ID << "StdCar replace engine " << old_model
             << " -> " << (*m_engine).model
             << endl;
    }

private:
    std::unique_ptr<Engine> m_engine;
};

class UplCar
{
public:
    UplCar(upl::unique<Engine> engine)
        : m_engine{std::move(engine)}
    {}

    void replaceEngine(upl::unique<Engine> engine)
    {
        using namespace std;

        string old_model = (*m_engine).model;
        m_engine = move(engine);
        cout << THREAD_ID << "UplCar replace engine " << old_model
             << " -> " << (*m_engine).model
             << endl;
    }

private:
    upl::unique<Engine> m_engine;
};

std::thread lambdaTransfer(std::shared_ptr<StdCar> car)
{
    using namespace std;

    cout << THREAD_ID << "Std lambda transfer" << endl;

    auto new_engine = make_unique<Engine>("V-8 Lambda");

    auto replace_engine =
        [engine = move(new_engine), car]() mutable {
            this_thread::sleep_for(chrono::milliseconds(200));
            (*car).replaceEngine(move(engine));
        };

    return thread{move(replace_engine)};
}

std::thread lambdaTransfer(upl::shared<UplCar> car)
{
    using namespace std;
    using namespace upl;

    cout << THREAD_ID << "Upl lambda transfer" << endl;

    upl::unique<Engine> new_engine{in_place, "V-8 Lambda"};

    auto replace_engine =
        [engine = move(new_engine), car]() mutable {
            this_thread::sleep_for(chrono::milliseconds(400));
            (*car).replaceEngine(move(engine));
        };

    return thread{move(replace_engine)};
}

std::thread functionLambdaTransfer(std::shared_ptr<StdCar> car[[maybe_unused]])
{
    using namespace std;

    cout << THREAD_ID << "Std function lambda transfer" << endl;

    auto new_engine = make_unique<Engine>("V-8 Function Lambda");

    #if 0
    // Error
    function<void()> replace_engine =
        [engine = move(new_engine), car]() mutable {
            this_thread::sleep_for(chrono::milliseconds(600));
            (*car).replaceEngine(move(engine));
        };
    #else
    function<void()> replace_engine =
        [] {
            this_thread::sleep_for(chrono::milliseconds(600));
            cout << THREAD_ID << "Unable std function lambda transfer" << endl;
        };
    #endif

    return thread{move(replace_engine)};
}

std::thread functionLambdaTransfer(upl::shared<UplCar> car)
{
    using namespace std;
    using namespace upl;

    cout << THREAD_ID << "Upl function lambda transfer" << endl;

    upl::unique<Engine> new_engine{in_place, "V-8 Function Lambda"};

    // Use an upl::unified to transfer the unique new_engine.
    unified<Engine> engine = move(new_engine);

    function<void()> replace_engine =
        [engine = move(engine), car]() mutable {
            this_thread::sleep_for(chrono::milliseconds(800));
            (*car).replaceEngine(move(engine));
        };

    return thread{move(replace_engine)};
}

std::thread functionBindTransfer(std::shared_ptr<StdCar> car[[maybe_unused]])
{
    using namespace std;

    cout << THREAD_ID << "Std function bind transfer" << endl;

    auto new_engine = make_unique<Engine>("V-8 Function Bind");

    #if 0
    // Error
    function<void()> replace_engine =
        std::bind(&StdCar::replaceEngine, car,
                  move(new_engine));
    #else
    function<void()> replace_engine =
        [] {
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << THREAD_ID << "Unable std function bind transfer" << endl;
        };
    #endif

    return thread{move(replace_engine)};
}

std::thread functionBindTransfer(upl::shared<UplCar> car)
{
    using namespace std;
    using namespace upl;

    cout << THREAD_ID << "Upl function bind transfer" << endl;

    upl::unique<Engine> new_engine{in_place, "V-8 Function Bind"};

    function<void()> replace_engine =
        std::bind(&UplCar::replaceEngine, car,
                  unified<Engine>(move(new_engine)));

    return thread{move(replace_engine)};
}

int main()
{
    using namespace std;
    using namespace upl;

    auto std_car = make_shared<StdCar>(make_unique<Engine>("V-6 Init"));
    (*std_car).replaceEngine(make_unique<Engine>("V-6 Direct"));

    shared<UplCar> upl_car{in_place, upl::unique<Engine>{in_place, "V-6 Init"}};
    (*upl_car).replaceEngine(upl::unique<Engine>{in_place, "V-6 Direct"});

    thread std_lambda = lambdaTransfer(std_car);
    thread upl_lambda = lambdaTransfer(upl_car);
    thread std_function_lambda = functionLambdaTransfer(std_car);
    thread upl_function_lambda = functionLambdaTransfer(upl_car);
    thread std_function_bind   = functionBindTransfer(std_car);
    thread upl_function_bind   = functionBindTransfer(upl_car);

    std_lambda.join();
    upl_lambda.join();
    std_function_lambda.join();
    upl_function_lambda.join();
    std_function_bind.join();
    upl_function_bind.join();

    return 0;
}
