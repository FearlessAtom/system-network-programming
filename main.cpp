#include <iostream>
#include <random>
#include <chrono>
#include <windows.h>
#include "./interface/interface.h"

void output_array(int* array, int length)
{
    for (int i = 0; i < length; i++)
    {
        std::cout << array[i];

        if (i + 1 != length) std::cout << " ";
    }

    std::cout << std::endl;
}

int get_random_integer(int min, int max)
{
    std::random_device rd;
    std::uniform_int_distribution<> dist(min, max);

    return dist(rd);
}

typedef ICalculator* (*create_calculator_function)();
typedef void (*hello_function)();

int main()
{
    std::string calculator_library_path= "./calculator/calculator.dll";

    int length = 1000000, min = -10, max = 10;
    int* array = new int[length];

    for (int i = 0; i < length; i++) array[i] = get_random_integer(min, max);

    HMODULE module = GetModuleHandle(calculator_library_path.c_str());
    if (!module) module = LoadLibrary(calculator_library_path.c_str());

    if (!module)
    {
        std::cerr << "Loading calculator library failed with error code: " << GetLastError() << std::endl;
        return 1;
    }

    create_calculator_function create_calculator = (create_calculator_function)GetProcAddress(module, "CreateCalculator");

    if (!create_calculator)
    {
        std::cerr << std::endl << "Failed to get a create calculator function!" << std::endl;
        return 1;
    }

    ICalculator* calculator = create_calculator();

    auto start = std::chrono::high_resolution_clock::now();

    calculator->sort_array(array, length);

    auto end = std::chrono::high_resolution_clock::now();

    delete calculator;
    FreeLibrary(module);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Time taken: " << duration.count() / 1000.0 << " seconds" << std::endl;

    HMODULE hello_module = LoadLibrary("./hello_world.dll");

    if (!hello_module)
    {
        std::cerr << std::endl << "Loading hello library failed with error code: " << GetLastError() << std::endl;
        return 1;
    }

    hello_function hello = (hello_function)GetProcAddress(hello_module, NULL);
    
    hello();

    FreeLibrary(hello_module);

    return 0;
}
