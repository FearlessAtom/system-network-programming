#include <iomanip>
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>
#include <string>
#include <windows.h>
#include <Psapi.h>
#include "./interface/interface.h"

typedef ICalculator* (*create_calculator_function)();
typedef void (*external_function)(HWND, LPCSTR, LPCSTR, UINT);

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

void display_memory_usage(std::string message="")
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

int main()
{
    std::string calculator_library_path = "./calculator/calculator.dll";

    int length = 1000000, min = -10, max = 10, number_of_threads = 5;
    int* array = new int[length];

    for (int i = 0; i < length; i++) array[i] = get_random_integer(min, max);

    display_memory_usage("Memory usage before loading calculator.dll");

    HMODULE module = GetModuleHandle(calculator_library_path.c_str());
    if (!module) module = LoadLibrary(calculator_library_path.c_str());

    display_memory_usage("Memory usage after loading calculator.dll");

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

    display_memory_usage("Memory usage after creating an instance of Calculator");

    auto start = std::chrono::high_resolution_clock::now();

    calculator->sort_array(array, length, number_of_threads);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << " seconds" << std::endl;

    display_memory_usage("Memory usage after sorting is complete");

    delete calculator;

    display_memory_usage("Memory usage after the Calculator instance is deleted");

    FreeLibrary(module);

    display_memory_usage("Memory usage after calculator.dll is freed and before loading hello_world.dll");

    std::string external_module_name = "user32.dll";

    HMODULE external_module = LoadLibrary(external_module_name.c_str());

    if (!external_module)
    {
        std::cerr << std::endl << "Loading " << external_module_name <<
            " failed with error code: " << GetLastError() << std::endl;

        return 1;
    }

    display_memory_usage("Memory usage after loading " + external_module_name);

    external_function external = (external_function)GetProcAddress(external_module, "MessageBoxA");

    if (!external)
    {
        std::cerr << std::endl << "Could not find the function in " <<
            external_module_name << "." << " Error code: " << GetLastError() << std::endl;

        return 1;
    }

    external(NULL, "user32.dll", "user32.dll", MB_OK);

    display_memory_usage("Memory usage after the function of " + external_module_name);

    FreeLibrary(external_module);

    display_memory_usage("Memory usage after " + external_module_name + " is freed");

    return 0;
}
