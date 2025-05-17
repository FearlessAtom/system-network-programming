#include "./calculator.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

Calculator::Calculator()
{
    std::cout << "Calculator instance constructed." << std::endl;
}

Calculator::~Calculator()
{
    std::cout << "Calculator instance deconstructed!" << std::endl;
}

void Calculator::sort_chunk(int* array, size_t start, size_t end)
{
    qsort(array + start, end - start, sizeof(int), compare);
}

int Calculator::compare(const void* a, const void* b)
{
    return (*((const int*)a) - *((const int*)b));
}

void Calculator::sort_array(int* array, int length, int number_of_threads)
{
    std::cout << "Sorting array..." << std::endl;

    std::vector<std::thread> threads;
    size_t chunk_size = length / number_of_threads;

    for (int i = 0; i < number_of_threads; i++)
    {
        size_t start = i * chunk_size;
        size_t end = (i == chunk_size - 1) ? length : start + chunk_size;

        threads.emplace_back(sort_chunk, array, start, end);
    }

    for (int i = 0; i < threads.size(); i++) threads[i].join();

    size_t current_chunk_size = chunk_size;

    for (int i = 1; i < number_of_threads; ++i)
    {
        size_t middle = i * chunk_size;
        size_t end = (i == number_of_threads - 1) ? length : (i + 1) * chunk_size;
        std::inplace_merge(array, array + middle - chunk_size, array + end);
    }
}

extern "C" __declspec(dllexport) ICalculator* CreateCalculator()
{
    return new Calculator();
}
