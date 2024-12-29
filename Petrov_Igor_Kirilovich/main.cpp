#include <iostream>
#include <functional>
#include <future>
#include <thread>
#include <vector>

#include "tests.h"

static std::vector<std::function<bool()>> tests;

void InitTests()
{
    tests.push_back(TestBodyContains);
    tests.push_back(TestIsContains);
    tests.push_back(TestGetShadows);
    tests.push_back(TestGetBody);
    tests.push_back(TestIsRed);
    tests.push_back(TestIsGreen);
}

int LaunchTests()
{
    int total = tests.size();
    int passed = 0;

    std::vector<std::future<bool>> futures;

    for (const auto& test : tests)
    {
        futures.push_back(std::async(std::launch::async, test));
    }

    for (size_t i = 0; i < futures.size(); ++i)
    {
        std::cout << "test #" << (i + 1);
        if (futures[i].get())
        {
            passed += 1;
            std::cout << " passed\n";
        }
        else
        {
            std::cout << " failed\n";
        }
    }

    std::cout << "\ntests " << passed << "/" << total << " passed!" << std::endl;

    // 0 = success
    return total - passed;
}

int main()
{
    InitTests();
    return LaunchTests();
}
