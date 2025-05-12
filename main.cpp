#include <iostream>
#include <windows.h>
#include <winerror.h>
#include <vector>
#include <winreg.h>
#include <lmcons.h>

struct value_pair
{
    std::string name;
    std::string value;
};

std::vector<value_pair> read_key(HKEY h_key, std::string sub_key)
{
    HKEY key;

    LONG error_code = RegCreateKeyA(h_key, sub_key.c_str(), &key);

    if (error_code != ERROR_SUCCESS)
    {
        std::cerr << "Error creating a key. Error code: " << error_code << std::endl;
    }

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

void output_key(HKEY h_key, std::string sub_key, std::string sign=" = ")
{
    std::vector<value_pair> keys = read_key(h_key, sub_key);

    for (int i = 0; i < keys.size(); i++)
    {
        std::cout << keys[i].name << sign << keys[i].value << std::endl;
    }
}

int main()
{
    char user_name[UNLEN + 1];
    DWORD user_name_size = sizeof(user_name);

    GetUserNameA(user_name, &user_name_size);

    std::cout << "Startup applications for " << user_name << ":" << std::endl;
    output_key(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    
    char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computer_name_size = sizeof(computer_name);

    GetComputerNameA(computer_name, &computer_name_size);

    std::cout << std::endl << "Startup applications for " << computer_name << ":" << std::endl;
    output_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

    return 0;
}
