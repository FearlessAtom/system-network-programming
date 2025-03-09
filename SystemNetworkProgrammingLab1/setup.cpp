#include <iostream>
#include <Windows.h>
#include <string>
#include "utils.h"

int count = 30;
int min = 10;
int max = 100;

std::string default_file_name = "data.dat";

int main(int argc, char* argv[])
{
    std::string file_name = default_file_name;

    if (argc > 2)
    {
        std::cerr << "Usage: setup.exe [file_name]" << std::endl;
        return 1;
    }

    if (argc == 2)
    {
        file_name = argv[1];
    }

    HANDLE h_file = CreateFile(file_name.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (h_file == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error creating the file!" << std::endl;
        return 1;
    }

    DWORD bytes_written;
    std::string result = "";

    for (int i = 0; i < count; i++)
    {
        result = result + std::to_string(random_integer(min, max)) + " ";
    }

    BOOL success = WriteFile(h_file, result.c_str(), (DWORD)strlen(result.c_str()) , &bytes_written, NULL);

    if (!success)
    {
        std::cerr << "Error writing to the file!" << std::endl;
        return 1;
    }

    CloseHandle(h_file);

    return 0;
}
