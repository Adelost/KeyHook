#include "KeyHook.h"

using namespace std;
using namespace kh;

class MyKeyHook : public KeyHook {
protected:
    void script() {
        static bool enable = true;
        on(ArrowDown, MouseScrollDown);
        on(ArrowUp, MouseScrollUp);
        on(ArrowLeft, MouseTiltLeft);
        on(ArrowRight, MouseTiltRight);
    }
};

int main() {
    MyKeyHook hook;
    hook.start();
    return 0;
}
