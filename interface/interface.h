#pragma once

class __declspec(dllexport) ICalculator
{
public:
    virtual void sort_array(int* array, int length) = 0;
    virtual ~ICalculator() {}
};
