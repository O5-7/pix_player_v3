#include <cstring>
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <Mmsystem.h>
#include <iomanip>


using namespace std::chrono;
using namespace std;
#define clock high_resolution_clock

/**
 * @brief string转 wchar_t *
 * @param pKey string
 * @return wchar_t*
 * */
wchar_t *MultiByte_To_WideChar(const string &pKey) {
    const char *pCStrKey = pKey.c_str();
    //第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t *pWCStrKey = new wchar_t[pSize];
    //第二次调用将单字节字符串转换成双字节字符串
    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
    return pWCStrKey;
}

/**
 * @brief 显示/隐藏 光标
 * @param visible 是否可见
 * @return 返回说明
 *     -<em>false</em> 失败
 *     -<em>true</em> 成功
 * */
bool Show_Cursor(bool visible) { //显示或隐藏光标
    CONSOLE_CURSOR_INFO cursor_info = {100, visible};
    return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}


/**
 * @brief 设置窗口位置
 * @param screen_x
 * @param screen_y
 *
 * @return 返回说明
 *     -<em>false</em> 失败
 *     -<em>true</em> 成功
 */
bool Set_Windows_Console_Pos(HANDLE hout, short x, short y) {
    bool success = SetConsoleCursorPosition(hout, {x, y});
    return success;
}

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
    success2 = SetConsoleScreenBufferSize(hConsole_out, {screen_x, screen_y});
    success1 = SetConsoleWindowInfo(hConsole_out, 1, &rc);
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
bool Set_Windows_font(short size_x, short size_y, const wchar_t *font_type) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    cfi.dwFontSize.X = size_x;
    cfi.dwFontSize.Y = size_y;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_DONTCARE;
    wcscpy_s(cfi.FaceName, font_type); // 更改字体名
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

//   初始设置
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    ios::sync_with_stdio(false);
    EnableVTMode();
    Show_Cursor(false);
    SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

//  读取元数据
    ifstream codes_result;
    string codes;

//  元数据
    string font_type;
    int font_size;
    int str_size_X;
    int str_size_Y;
    bool loop_play = false;
    bool audio = false;

    codes_result.open(string(argv[1]) + "\\result.zfh");
//  读取元数据 直到END_META_DATA
    while (getline(codes_result, codes)) {
        if (codes.substr(0, 2) == "//") continue; // 忽略 //
        if (codes == "END_META_DATA") {
            Set_Windows_font(
                    short(font_size / 2),
                    short(font_size),
                    MultiByte_To_WideChar(font_type)
            );
            Set_Windows_size_without_scrollbar(short(str_size_X * 2 + 1), short(str_size_Y + 3));
            // 禁止修改窗口大小
            SetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE, GetWindowLongPtrA(GetConsoleWindow(), GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX);
            break;
        }
        int index = int(codes.find('='));
        if (codes.substr(0, index) == "media_name") {
//          media_name
            string file_name = codes.substr(index + 1, codes.length() - index - 1);
            SetConsoleTitle(("pix_player_v3: " + file_name).data());
        }
        if (codes.substr(0, index) == "font_type") {
//          字体名字
            font_type = codes.substr(index + 1, codes.length() - index - 1);
        }
        if (codes.substr(0, index) == "font_size") {
//           字体大小
            font_size = stoi(codes.substr(index + 1, codes.length() - index - 1));
        }
        if (codes.substr(0, index) == "str_size") {
//           屏幕大小
            string _str_size = codes.substr(index + 1, codes.length() - index - 1);
            int tuple_index = int(_str_size.find(','));
            str_size_X = stoi(_str_size.substr(0, tuple_index));
            str_size_Y = short(stoi(_str_size.substr(tuple_index + 1, _str_size.length() - tuple_index - 1)));
        }
        if (codes.substr(0, index) == "loop_play") {
//           是否循环播放
            string _loop_play = codes.substr(index + 1, codes.length() - index - 1);
            loop_play = _loop_play == "True";
        }
        if (codes.substr(0, index) == "audio") {
//           是否播放音频
            string _audio = codes.substr(index + 1, codes.length() - index - 1);
            audio = _audio == "True";
        }
    }

    codes_result.close();
    SetWindowPos(GetConsoleWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    while (true) {
        codes_result.open(string(argv[1]) + "\\result.zfh");
        while (getline(codes_result, codes)) {
            if (codes == "END_META_DATA") break;
        }

        if (audio) {
            bool audio_success = PlaySound(TEXT((string(argv[1]) + "\\result.wav").data()), nullptr, SND_FILENAME | SND_ASYNC);
            if (!audio_success) {
                cout << """""""""""""";
                getchar();
                return 0;
            }
        }

//    cout << "\033[38;2;255;255;255m"; // << "\033[0m";
        int line = 0;
        float delta_start = 0.0;
        auto play_start_time = clock::now(); // 播放的开始时间
        auto frame_start_time = clock::now(); // 一帧的开始时间
        int frame_count = 0;
        string multi_line;
        float duration = 0;
        float frame_print_time = 0.0; // 打印一帧的所有时间
        while (getline(codes_result, codes)) {
            frame_start_time = clock::now();
            /**
             * 循环读取
             * 打印实时参数
             */
            if (codes.substr(0, 2) == "//") continue;
            if (codes.substr(0, 9) == "duration=") {
                duration = stof(codes.substr(9, codes.length() - 9));
                delta_start += duration;
                continue;
            }
            multi_line += codes + '\n' + "\u001B[0m";
            line++;
            if (line % str_size_Y == 0) {

                WriteConsole(hOut, multi_line.c_str(), multi_line.length(), nullptr, nullptr);
                multi_line = "";
//                printf(multi_line.c_str());
//                cout << multi_line;
                if (frame_print_time > duration) {
                    cout << "\u001B[38;2;255;0;0;48;2;0;0;0m"
                         << setw(6) << right << int(frame_print_time * 100) / 100.0
                         << "\u001B[0m";
                } else {
                    cout << setw(6) << right << int(frame_print_time * 100) / 100.0;
                }
                cout << '/' << int(duration * 100) / 100.0 << "ms   ";
                cout << 1000.0 / duration << "hz";
                cout << setw(6) << right << frame_count++;
                cout << "frames    time(ms):";

                while (true) {
                    // 循环检测下一帧是否开始
                    chrono::duration delta_time_now = clock::now() - play_start_time;
                    long long delta_time_now_ms = duration_cast<milliseconds>(delta_time_now).count();
                    if (float(delta_time_now_ms) > delta_start) {
                        cout << setw(7) << right << (float(delta_time_now_ms)) << " / " << setw(7) << left << int(delta_start);
                        cout << "delta_time(ms) =" << setw(7) << right << int((float(delta_time_now_ms) - delta_start) * 100) / 100.0;
                        Set_Windows_Console_Pos(hOut, 0, 0);
                        frame_print_time = (float) duration_cast<microseconds>(clock::now() - frame_start_time).count() / 1000;
//                        frame_start_time = clock::now();
                        break;
                    }
                }
            }
        }
        codes_result.close();
        if (!loop_play) break;
    }


    cout << endl;
    getchar();
}
