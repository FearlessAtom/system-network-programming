#pragma once

#include "../interface/interface.h"

class __declspec(dllexport) Calculator : public ICalculator
{
public:
    Calculator();
    ~Calculator();
    void sort_array(int* array, int length);
    static int compare(const void* a, const void* b);
};

extern "C" __declspec(dllexport) ICalculator* CreateCalculator();
