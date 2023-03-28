#include "StaticThreadPool.h"

#include <iostream>
#include <vector>
#include <functional>


void printNum(int number)
{
    std::cout << "Number: " << number << std::endl;
}

int main()
{
    StaticThreadPool staticThreadPool;
    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < 8; ++i)
    {
        futures.emplace_back(staticThreadPool.Submit([capture = static_cast<int>(i)] ()
        {
            printNum(capture);
        }));
    }

    for (auto& result : futures)
    {
        result.get();
    }
    return 0;
}