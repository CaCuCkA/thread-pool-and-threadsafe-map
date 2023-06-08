#include <vector>
#include <iostream>

#include "Utils.h"
#include "CrossType.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }

    int result = 0;
    int boundNumber = std::stoi(argv[1]);
    std::vector<std::future<int>> futures;
    cross_type::thread_pool threadPool;
    auto startTime = getCurrentTime();
    for (int i = 1; i <= boundNumber; ++i)
    {
        for (int j = 1; j <= boundNumber; ++j)
        {
            futures.emplace_back(
                    threadPool.Submit([=]()
                    {
                        return Multiply(i, j);
                    }));

        }
    }

    for (auto & future : futures)
    {
        result += future.get();
    }
    auto endTime = getCurrentTime();

    std::cout << "Total time: " << toUs(endTime - startTime) << std::endl;
}
