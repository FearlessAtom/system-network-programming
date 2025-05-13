#include <iostream>
#include <windows.h>
#include <winerror.h>
#include <vector>
#include <winnt.h>
#include <winreg.h>
#include <lmcons.h>

std::string word_value = "C:\\Program Files (x86)\\Microsoft Office\\Office16\\winword.exe";
std::string word_name = "Microsoft Word";

struct value_pair
{
    std::string name;
    std::string value;
};

std::vector<value_pair> read_key(HKEY key)
{
    char name[150];
    char value[150];

    DWORD name_size;
    DWORD value_size;

    std::vector<value_pair> result = std::vector<value_pair>();

    for (int i = 0; ; i++)
    {
        name_size = sizeof(name);
        value_size = sizeof(value);

        LONG error_code = RegEnumValueA(key, i, name, &name_size, 0, NULL, (LPBYTE)value, &value_size);

        if (error_code != ERROR_SUCCESS)
        {
            break;
        }

        result.push_back({name, value});
    }

    return result;
}

void output_key_values(HKEY key, std::string sign=" = ")
{
    std::vector<value_pair> keys = read_key(key);


    for (int i = 0; i < keys.size(); i++)
    {
        std::cout << keys[i].name << sign << keys[i].value << std::endl;
    }
}

DWORD add_value(HKEY key, std::string name, std::string value)
{
    DWORD value_size = value.size() + 1;

    return RegSetValueExA(key, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value_size);
}

std::vector<std::string> read_subkeys(HKEY key)
{
    char name[150];

    DWORD name_size;

    std::vector<std::string> result = std::vector<std::string>();

    for (int i = 0; ; i++)
    {
        name_size = sizeof(name);

        LONG error_code = RegEnumKeyEx(key, i, name, &name_size, NULL, NULL, NULL, NULL);

        if (error_code == ERROR_NO_MORE_ITEMS)
        {
            break;
        }

        if (error_code != ERROR_SUCCESS)
        {
            std::cerr << "Error code: " << error_code << std::endl;
            break;
        }

        result.push_back(name);
    }

    return result;
}

void output_subkeys(HKEY key)
{
    std::vector<std::string> tasks = read_subkeys(key);

    if (tasks.empty())
    {
        std::cout << "No subkeys." << std::endl;
    }

    for (int i = 0; i < tasks.size(); i++)
    {
        std::cout << tasks[i] << std::endl;
    }
}

int main()
{
    LONG error_code;

    HKEY user_startup_key;

    std::string startup_sub_key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    std::string tasks_sub_key = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Schedule\\TaskCache\\Tree";

    error_code = RegCreateKeyA(HKEY_CURRENT_USER, startup_sub_key.c_str(), &user_startup_key);

    if (error_code != ERROR_SUCCESS)
    {
        std::cerr << "Error creating a current user key. Error code: " << error_code << std::endl;
        return 1;
    }

    HKEY computer_key;

    error_code = RegCreateKeyA(HKEY_LOCAL_MACHINE, startup_sub_key.c_str(), &computer_key);

    if (error_code != ERROR_SUCCESS)
    {
        std::cerr << "Error creating a local machine key. Error code: " << error_code << std::endl;
        RegCloseKey(user_startup_key);
        return 1;
    }

    char user_name[UNLEN + 1];
    DWORD user_name_size = sizeof(user_name);

    GetUserNameA(user_name, &user_name_size);

    std::cout << "Startup applications for user " << user_name << ":" << std::endl;
    output_key_values(user_startup_key);
    
    char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computer_name_size = sizeof(computer_name);

    GetComputerNameA(computer_name, &computer_name_size);

    std::cout << std::endl << "Startup applications for computer " << computer_name << ":" << std::endl;
    output_key_values(computer_key);

    error_code = add_value(user_startup_key, word_name, word_value);

    if (error_code != ERROR_SUCCESS)
    {
        std::cerr << "Error adding Microsoft Word to startup. Error code: " << error_code << std::endl;
    }

    std::cout << std::endl << "Startup applications for " << user_name << " after adding Microsoft Word to startup:" << std::endl;
    output_key_values(user_startup_key);

    HKEY user_tasks_key;
    HKEY computer_tasks_key;

    HKEY opened_user_tasks_key;
    HKEY opened_computer_tasks_key;

    error_code = RegCreateKeyA(HKEY_CURRENT_USER, tasks_sub_key.c_str(), &user_tasks_key);
    error_code = RegOpenKeyExW(user_tasks_key, NULL, KEY_ENUMERATE_SUB_KEYS, KEY_READ, &opened_user_tasks_key);

    error_code = RegCreateKeyA(HKEY_LOCAL_MACHINE, tasks_sub_key.c_str(), &computer_tasks_key);
    error_code = RegOpenKeyExW(computer_tasks_key, NULL, KEY_ENUMERATE_SUB_KEYS, KEY_READ, &opened_computer_tasks_key);

    if (error_code != ERROR_SUCCESS)
    {
        std::cerr << "Error code: " << error_code << std::endl;
    }

    std::cout << std::endl << "Registered tasks for user " << user_name << ":" << std::endl;
    output_subkeys(opened_user_tasks_key);


    std::cout << std::endl << "Registered tasks for computer " << computer_name << ":" << std::endl;
    output_subkeys(opened_computer_tasks_key);

    return 0;
}
