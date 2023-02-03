#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>


using namespace std;

/**
 * @brief 设置窗口大小
 * @param screen_x
 * @param screen_y
 *
 * @return 返回说明
 *     -<em>false</em> 失败
 *     -<em>true</em> 成功
 */
bool Set_Windows_size_without_scrollbar(short screen_x, short screen_y) {
    COORD size = {screen_x, screen_y};
    SMALL_RECT rc = {0, 0, short(size.X - 1), short(size.Y - 1)};
    HANDLE hConsole_out = GetStdHandle(STD_OUTPUT_HANDLE);
    bool success2 = SetConsoleScreenBufferSize(hConsole_out, {screen_x, screen_y});
    bool success1 = SetConsoleWindowInfo(hConsole_out, 1, &rc);
    if (success1 & success2) return 1;
    else {
        cout << "Set_window_size" << success1 << endl;
        cout << "Set_buffer_size" << success2 << endl;
        cout << "Error code" << GetLastError() << endl;
        return 0;
    }
}

/**
 * @brief 设置窗口字体
 * @param size_x
 * @param size_y
 *
 * @return 返回说明
 *     -<em>false</em> 失败
 *     -<em>true</em> 成功
 */
bool Set_Windows_font(short size_x, short size_y) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    cfi.dwFontSize.X = size_x;
    cfi.dwFontSize.Y = size_y;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_DONTCARE;
    wcscpy_s(cfi.FaceName, L"新宋体"); // 更改字体名
    return SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

/**
 * @brief 启用虚拟终端
 *
 * @return 返回说明
 *     -<em>false</em> 失败
 *     -<em>true</em> 成功
 */
bool EnableVTMode() {
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {

    Set_Windows_font(8, 16);
    SetConsoleTitle("pix_player_v3");
    SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    EnableVTMode();
    //cout << "\033[38;2;255;255;255m"; // << "\033[0m";

    ifstream codes_result;
    string codes;
    codes_result.open("C:\\Users\\Normal\\PycharmProjects\\zi_fu_hua_v3\\code_sources_v3.txt");
    getline(codes_result, codes);
    codes_result.close();
    bool wchar = false;
    bool color = true;
    for (char i: codes) {
        wchar = !wchar;
        if (wchar) {
            color = !color;
            if (color) cout << "\033[38;2;255;255;255;48;2;12;12;12m";
            else cout << "\033[48;2;255;255;255;38;2;12;12;12m";
        }
        cout << i;
    }
    codes_result.open("C:\\Users\\Normal\\PycharmProjects\\zi_fu_hua_v3\\code_sources_v3.txt");
    getline(codes_result, codes);
    codes_result.close();
    codes_result.close();
    wchar = false;
    color = false;
    for (char i: codes) {
        wchar = !wchar;
        if (wchar) {
            color = !color;
            if (color) cout << "\033[38;2;255;255;255;48;2;12;12;12m";
            else cout << "\033[48;2;255;255;255;38;2;12;12;12m";
        }
        cout << i;
    }
    cout << "\033[0m";
    codes_result.open("color.txt");
    getline(codes_result, codes);
    codes_result.close();
    cout << codes;
// 禁止修改窗口大小
    SetWindowLongPtrA(
            GetConsoleWindow(),
            GWL_STYLE,
            GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE)
            & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX
    );
    getchar();
}
