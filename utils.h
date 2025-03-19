#include <sstream>
#include <fileapi.h>
#include <handleapi.h>
#include <iostream>
#include <minwindef.h>
#include <string>
#include <vector>
#include <winnt.h>
#include <random>

bool read_file(HANDLE h_file, std::string& content)
{
    const DWORD buffer_size = 1024;
    char buffer[buffer_size] = {0};
    DWORD bytes_read = 0;

    bool success = ReadFile(h_file, buffer, buffer_size, &bytes_read, NULL);

    content = buffer;

    return success;
}

bool file_exists(std::string file_name)
{
    DWORD attributes = GetFileAttributes(file_name.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

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

int file_to_numbers(HANDLE h_file, int*& numbers, int& length)
{
    std::string content;

    bool reading_success = read_file(h_file, content);
    
    if (!reading_success)
    {
        std::cerr << "Error reading the file!" << std::endl;
        CloseHandle(h_file);
        return 1;
    }

    std::pair<std::string*, int> words_entry = string_split(' ', content);

    std::string* words =  words_entry.first;
    length = words_entry.second;

    std::vector<int> numbers_vector = {};

    for (int i = 0; i < length; i++)
    {
        try
        {
            numbers_vector.push_back(std::stoi(words[i]));
        }

        catch (std::exception ex)
        {
            std::cerr << "Invalid file content!" << std::endl;
            return 1;
        }
    }

    numbers = numbers_vector.data();

    return 0;
}

std::string numbers_to_string(int* numbers, int length)
{
    std::string result = "";

    for (int i = 0; i < length; i++)
    {
        result = result + std::to_string(numbers[i]);

        if (i + 1 != length)
        {
            result = result + " ";
        }
    }
    
    return result;
}

