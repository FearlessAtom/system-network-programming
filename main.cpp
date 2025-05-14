#include <iomanip>
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>
#include <string>
#include <windows.h>
#include <Psapi.h>
#include "./interface/interface.h"

int get_memory_usage()
{
    PROCESS_MEMORY_COUNTERS process_memory;
    GetProcessMemoryInfo(GetCurrentProcess(), &process_memory, sizeof(process_memory));

    return process_memory.WorkingSetSize;
}

std::string get_human_readable_bytes(int bytes)
{
    const char* sizes[] = { "B", "KB", "MB", "GB", "TB", "PB" };
    int order = 0;
    double readable_size = static_cast<double>(bytes);

    while (readable_size >= 1024 && order < 5)
    {
        order++;
        readable_size /= 1024;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << readable_size << " " << sizes[order];

    return oss.str();
}

void display_memory_info(std::string message="")
{
    std::string default_message = "Memory usage";

    if(message == "") message = default_message;

    std::cout << message << ": " <<
        get_human_readable_bytes(get_memory_usage()) << std::endl;
}

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

    display_memory_info("Memory usage before loading calculator.dll");

    HMODULE module = GetModuleHandle(calculator_library_path.c_str());
    if (!module) module = LoadLibrary(calculator_library_path.c_str());

    std::cout << "Memory usage after loading calculator.dll: " <<
        get_human_readable_bytes(get_memory_usage()) << std::endl;;

    if (!module)
    {
        std::cerr << "Loading calculator library failed with error code: " <<
            GetLastError() << std::endl;

        return 1;
    }

    create_calculator_function create_calculator =
        (create_calculator_function)GetProcAddress(module, "CreateCalculator");

    if (!create_calculator)
    {
        std::cerr << std::endl << "Failed to get a create calculator function!" <<
            " Error code: " << GetLastError() << std::endl;

        return 1;
    }

    ICalculator* calculator = create_calculator();

    display_memory_info("Memory usage after creating an instance of Calculator");

    auto start = std::chrono::high_resolution_clock::now();

    calculator->sort_array(array, length);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << " seconds" << std::endl;

    display_memory_info("Memory usage after sorting is complete");

    delete calculator;

    display_memory_info("Memory usage after the Calculator instance is deleted");

    FreeLibrary(module);

    display_memory_info("Memory usage after calculator.dll is freed");

    display_memory_info("Memory usage before loading hello_world.dll");

    HMODULE hello_module = LoadLibrary("./hello_world.dll");

    if (!hello_module)
    {
        std::cerr << std::endl << "Loading hello library failed with error code: " <<
            GetLastError() << std::endl;

        return 1;
    }

    display_memory_info("Memory usage after loading hello_world.dll");

    hello_function hello = (hello_function)GetProcAddress(hello_module, NULL);

    hello();

    display_memory_info("Memory usage after creating an instance hello_world.dll");

    FreeLibrary(hello_module);

    display_memory_info("Memory usage after hello_world.dll is freed");

    return 0;
}
