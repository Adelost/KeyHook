#include <iostream>
#include "OSWrap.h"

namespace kh {
std::string getActiveWindow() {
#ifdef _WIN32_
    char wnd_title[1024];
    HWND hwnd = GetForegroundWindow();
    GetWindowTextA(hwnd, wnd_title, sizeof(wnd_title));
    std::string path(wnd_title);
    std::string title = path.substr(path.find_last_of("\\/") + 1, std::string::npos);
    return title;
#else
    return "";
#endif
}

Point mapToWindow(Point& globalPoint, Point& windowPos) {
    return globalPoint - windowPos;
}
Point mapToGlobal(Point& windowPoint, Point& windowPos) {
    return windowPoint + windowPos;
}

Point getActiveWindowPos() {
#ifdef _WIN32_
    RECT rect;
    GetWindowRect(GetForegroundWindow(), &rect);
    return {rect.left, rect.top};
#else
    return {0, 0};
#endif
}
Point getCursorPos() {
#ifdef _WIN32_
    POINT point;
    GetCursorPos(&point);
    return {point.x, point.y};
#else
    return {0, 0};
#endif
}

Sense::Sense() {
}
Sense::~Sense() {
    if (m_inited) {
        clear();
    }
}
Color Sense::pixel(Point point) {
#ifdef _WIN32_
    std::cout << "Point: " << point.x << " " << point.y << std::endl;
    point = mapToGlobal(point, m_window);

    COLORREF color = GetPixel(m_dc, point.x, point.y);
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    std::cout << "Beforehex: " << r << " " << g << " " << b << std::endl;
    return {r, g, b};
#else
    return {0, 0, 0};
#endif
}
Point Sense::window() {
    return m_window;
}
Point Sense::cursor() {
    return m_cursor;
}
void Sense::init() {
    m_inited = true;
#ifdef _WIN32_
    m_hwnd = GetForegroundWindow();
    m_dc = GetDC(nullptr);
#endif
    m_window = getActiveWindowPos();
    auto cursorGlobal = getCursorPos();
    m_cursor = mapToWindow(cursorGlobal, m_window);
}
void Sense::clear() {
#ifdef _WIN32_
    ReleaseDC(NULL, m_dc);
#endif
    m_inited = false;
}

}