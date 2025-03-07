#include <cstring>
#include <fileapi.h>
#include <handleapi.h>
#include <iostream>
#include <minwindef.h>
#include <random>
#include <Windows.h>
#include <string>
#include <winnt.h>

std::string file_name = "data.dat";

int count = 30;
int min = 10;
int max = 100;

int random_integer(int min, int max)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dis(min, max);
    return dis(rd);
}

int main()
{
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
