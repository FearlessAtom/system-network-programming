#include <iostream>
#include <random>
#include <windows.h>
#include "./interface/interface.h"

int get_random_integer(int min, int max)
{
    std::random_device rd;
    std::uniform_int_distribution<> dist(min, max);

    return dist(rd);
}

typedef ICalculator* (*create_calculator_function)();

int main()
{
    std::string calculator_library_path= "./calculator/calculator.dll";

    int length = 5, min = -10, max = 10;
    int* array = new int[length];

    for (int i = 0; i < length; i++) array[i] = get_random_integer(min, max);

    for (int i = 0; i < length; i++) std::cout << array[i] << " ";

    HMODULE module = GetModuleHandle(calculator_library_path.c_str());
    if (!module) module = LoadLibrary(calculator_library_path.c_str());

    if (!module)
    {
        std::cerr << std::endl << "Failed to load the calculator module!" << std::endl;
        return 1;
    }

    create_calculator_function create_calculator = (create_calculator_function)GetProcAddress(module, "CreateCalculator");

    if (!create_calculator)
    {
        std::cerr << std::endl << "Failed to get a create calculator function!" << std::endl;
        return 1;
    }

    std::cout << std::endl;
    
    ICalculator* calculator = create_calculator();

    calculator->sort_array(array, length);

    delete calculator;
    FreeLibrary(module);

    for (int i = 0; i < length; i++) std::cout << array[i] << " ";

    return 0;
}
