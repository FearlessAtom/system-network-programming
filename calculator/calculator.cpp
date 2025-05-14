#include "./calculator.h"
#include <iostream>

Calculator::Calculator()
{
    std::cout << "Calculator instance constructed." << std::endl;
}

Calculator::~Calculator()
{
    std::cout << "Calculator instance deconstructed!" << std::endl;
}

void Calculator::sort_array(int* array, int length)
{
    std::cout << "Sorting array..." << std::endl;

    qsort(array, length, sizeof(int), compare);
}

int Calculator::compare(const void* a, const void* b)
{
    int int_a = *((const int*)a);
    int int_b = *((const int*)b);

    if (int_a > int_b) return 1;
    if (int_a < int_b) return -1;
    return 0;
}

extern "C" __declspec(dllexport) ICalculator* CreateCalculator()
{
    return new Calculator();
}
