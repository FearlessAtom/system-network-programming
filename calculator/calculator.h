#pragma once

#include "../interface/interface.h"

class __declspec(dllexport) Calculator : public ICalculator
{
public:
    Calculator();
    ~Calculator();
    static void sort_chunk(int* array, size_t start, size_t end);
    void sort_array(int* array, int length, int number_of_threads);
    static int compare(const void* a, const void* b);
};

extern "C" __declspec(dllexport) ICalculator* CreateCalculator();
