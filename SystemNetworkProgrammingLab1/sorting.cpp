#include <Windows.h>
#include <fileapi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <winnt.h>

std::pair<std::string*, int> string_split(char delimeter, std::string str)
{
    std::vector<std::string> words;
    std::istringstream stream(str);

    std::string token;

    while (std::getline(stream, token, delimeter))
    {
        words.push_back(token);
    }

    return std::make_pair(words.data(), words.size());
}

void selection_sort(int* array, int length)
{
    for (int i = 0; i < length - 1; i++)
    {
        int index = i;

        for (int j = i + 1; j < length; j++)
        {
            if (array[index] > array[j])
            {
                index = j;
            }
        }

        std::swap(array[index], array[i]);
    }
}

void output_array(int* array, int length)
{
    for (int i = 0; i < length; i++)
    {
        std::cout << array[i] << std::endl;
    }
}

int main()
{
    HANDLE h_file_reading = CreateFile("data.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (h_file_reading == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening the file!" << std::endl;
        return 1;
    }

    const DWORD buffer_size = 1024;
    char buffer[buffer_size] = {0};

    DWORD bytes_read = 0;


    BOOL reading_success = ReadFile(h_file_reading, buffer, buffer_size, &bytes_read, NULL);

    if (!reading_success)
    {
        std::cerr << "Error reading the file!" << std::endl;
        CloseHandle(h_file_reading);
        return 1;
    }

    CloseHandle(h_file_reading);

    char* str(buffer);

    std::pair<std::string*, int> words_entry = string_split(' ', str);

    std::string* words =  words_entry.first;
    int length = words_entry.second;

    std::vector<int> numbers_vector = {};

    for (int i = 0; i < length; i++)
    {
        numbers_vector.push_back(std::stoi(words[i]));
    }

    int* numbers = numbers_vector.data();

    selection_sort(numbers, length);

    std::string result = "";

    for (int i = 0; i < length; i++)
    {
        result = result + std::to_string(numbers[i]);

        if (i + 1 != length)
        {
            result = result + " ";
        }
    }

    DWORD bytes_written;

    HANDLE h_file_writing = CreateFile("data.dat", GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


    BOOL writing_success = WriteFile(h_file_writing, result.c_str(), (DWORD)strlen(result.c_str()),
            &bytes_written, NULL);

    if (!writing_success)
    {
        std::cerr << "Error writing to the file!" << std::endl;
        return 1;
    }

    CloseHandle(h_file_writing);

    return 0;
}
