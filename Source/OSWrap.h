#pragma once

#include <sstream>

#ifdef _WIN32_
#include "Windows.h"
#endif

namespace kh {

struct Color {
    int r;
    int g;
    int b;
    std::string toHex() {
        int number = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
        std::stringstream stream;
        stream << std::hex << number;
        return "#" + stream.str();
    }
};

struct Point {
    int x;
    int y;
};

inline Point operator+(Point& left, Point& right) {
    return {left.x + right.x, left.y + right.y};
}

inline Point operator-(Point& left, Point& right) {
    return {left.x - right.x, left.y - right.y};
}


std::string getActiveWindow();

#ifdef _WIN32_
Point getActiveWindowPos();
Point getCursorPos();
Point mapToWindow(Point& globalPoint, Point& windowPos);
Point mapToGlobal(Point& windowPoint, Point& windowPos);
#endif

class Sense {
public:
    Sense();
    ~Sense();
    void init();
    void clear();
    Color pixel(Point point);
    Point window();
    Point cursor();

private:
    bool m_inited = false;
#ifdef _WIN32_
    HWND m_hwnd;
    HDC m_dc;
#endif
    Point m_cursor;
    Point m_window;

};

}