#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <string>
#include <winnt.h>
#include <conio.h>
#include "utils.h"

std::string default_file_name = "data.dat";


int sort_file(std::string file_name)
{
    HANDLE h_file_reading = CreateFile(file_name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h_file_reading == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening the file!" << std::endl;
        return 1;
    }

    int* numbers;
    int length;

    file_to_numbers(h_file_reading, numbers, length);

    CloseHandle(h_file_reading);

    selection_sort(numbers, length);

    std::string sorted_numbers_string = numbers_to_string(numbers,  length);

    DWORD bytes_written;

    HANDLE h_file_writing = CreateFile(file_name.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    BOOL writing_success = WriteFile(h_file_writing, sorted_numbers_string.c_str(),
            (DWORD)strlen(sorted_numbers_string.c_str()),
            &bytes_written, NULL);

    if (!writing_success)
    {
        std::cerr << "Error writing to the file!" << std::endl;
        return 1;
    }

    CloseHandle(h_file_writing);

    return 0;
}

int main(int argc, char* argv[])
{
    std::string file_name = default_file_name;

    if (argc > 2)
    {
        std::cerr << "Usage: sorting.exe [file_name]" << std::endl;
        return 1;
    }

    if (argc == 2)
    {
        file_name = argv[1];
    }

    if (!file_exists(file_name))
    {
        std::cerr << "File " + file_name + " does not exist!" << std::endl;
        return 1;
    }

    while (true)
    {
        char key = getch();

        if (key == 32)
        {
            sort_file(file_name);
        }

        else if (key == 'q')
        {
            break;
        }
    }

    return 0;
}
