#include <errhandlingapi.h>
#include <fileapi.h>
#include <minwindef.h>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <windef.h>
#include <iostream>
#include <wingdi.h>
#include <winuser.h>

std::wstring space_indent = L"        ";

std::wstring get_drive_type_wstring(int type)
{
    switch (type)
    {
        case 0: return L"Unknown";
        case 1: return L"Invalid";
        case 2: return L"Removable";
        case 3: return L"Fixed";
        case 4: return L"Remote";
        case 5: return L"CD-ROM";
    }

    throw std::invalid_argument("Invalid drive type!");
}

void render(HDC window_handle, std::vector<std::wstring> lines)
{
    int gap_indent = 20;

    for (int i = 0; i < lines.size(); i++)
    {
        TextOutW(window_handle, 0, gap_indent * i, lines[i].c_str(), lines[i].length());
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT paint_struct;

            HDC panting_handle = BeginPaint(hwnd, &paint_struct);

            DWORD drives = GetLogicalDrives();

            std::vector<std::wstring> lines = std::vector<std::wstring>();

            for (int i = 0; i < 26; i++)
            {
                if (drives & (1 << i))
                {
                    char disk_letter = char('A' + i);

                    std::wstring volume_root = std::wstring(1, disk_letter) + L":\\";

                    int drive_type = GetDriveTypeW(volume_root.c_str());

                    lines.push_back(std::wstring(1, disk_letter) + L":\\ - " + get_drive_type_wstring(drive_type));

                    wchar_t volume_name_buffer[MAX_PATH + 1] = {  };
                    DWORD volume_serial_number;
                    wchar_t file_system_name_buffer[MAX_PATH + 1] = {  };

                    if (GetVolumeInformationW(volume_root.c_str(), volume_name_buffer, sizeof(volume_name_buffer), &volume_serial_number, NULL, NULL, file_system_name_buffer, sizeof(file_system_name_buffer)))
                    {
                        lines.push_back(space_indent + L"Volume name: " +
                            std::wstring((volume_name_buffer[0] ? volume_name_buffer : L"<NONE>"))+ L", File system: " +
                            std::wstring((file_system_name_buffer[0] ? file_system_name_buffer : L"<NONE>")) +
                            L", Serial number: " + std::to_wstring(volume_serial_number));
                    }
                }
            }

            render(panting_handle,  lines);

            EndPaint(hwnd, &paint_struct);

            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
    std::string class_name = "MainWindow";

    HINSTANCE handle = GetModuleHandle(NULL);

    WNDCLASS window_class = {  };

    window_class.lpfnWndProc = WindowProcedure;
    window_class.hInstance= handle;
    window_class.lpszClassName = class_name.c_str();
    window_class.hCursor = LoadCursor(handle, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&window_class))
    {
        std::cerr << "Error registering the \"MainWindow\" window class" << std::endl;
        return 1;
    }

    HWND window_handle = CreateWindowEx(0, 
        class_name.c_str(),
        "Lab 3",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, handle, NULL);

    if (window_handle == NULL)
    {
        std::cerr << "Error creating the window!" << std::endl;
        return 1;
    }

    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    MSG message;

    while(GetMessageW(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}
