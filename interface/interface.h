#pragma once

class __declspec(dllexport) ICalculator
{
public:
    virtual void sort_array(int* array, int length, int number_of_threads) = 0;
    virtual ~ICalculator() {}
};
