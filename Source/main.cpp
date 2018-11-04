#include "KeyHook.h"

using namespace std;
using namespace kh;

class MyKeyHook : public KeyHook {
protected:
    void script() override {
		on(ArrowDown, MouseScrollDown);
		on(ArrowUp, MouseScrollUp);
		on(ArrowLeft, MouseScrollLeft);
		on(ArrowRight, MouseScrollRight);
        //on(Num1, Num2);
    }
};

int main() {
    MyKeyHook hook;
    hook.start();
    return EXIT_SUCCESS;
}
