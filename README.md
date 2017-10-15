# KeyHook

![KeyHook captain hook](/Doc/Images/cptnhook.png)

Provides low level rebinding of both keyboard keys and mouse keys, as well as creating macros. Especially useful for both games and applications which do not support rebinding of keys. The bindings are built and run as a normal C++ program.

If this sounds intriguing to you, please continue reading...

## Examples

**Basic setup**

```c++
#include "KeyHook.h"

using namespace kh;

class MyKeyHook : public KeyHook {
protected:
    void script() {
        // Script code goes here (executed every key press)
    }
};

int main() {
    MyKeyHook hook;
    hook.start();
    return EXIT_SUCCESS;
}
```

**Basic usage**

Rebind `A` to `B`.
```c++
on(A, B);
```
Keys can be disabled.
```c++
mute(Win);
```
Modifier keys work as expected and allows greater flexibility.
```c++
on(Ctrl + C, Ctrl + V);
on(Ctrl + V, Ctrl + C);
on(Ctrl + Alt + Delete, Ctrl + S);
on(LShift, RShift);
```
Rebind `mouse scroll` and `mouse tilt` to `arrow keys` and vice versa, just to spice things up a bit.
```c++
// Rebind mouse keys
on(MouseScrollDown, ArrowDown);
on(MouseScrollUp, ArrowUp);
on(MouseTiltLeft, ArrowLeft);
on(MouseTiltRight, ArrowRight);

// Rebind arrow keys
on(ArrowDown, MouseScrollDown);
on(ArrowUp, MouseScrollUp);
on(ArrowLeft, MouseTiltLeft);
on(ArrowRight, MouseTiltRight);
```
`Action`-objects can be used for greater customisation. Lambda capture (`&`) is required.
```c++
on(Ctrl + W, Action([&] {
    // Press Ctrl + A
    send(Ctrl + A);
    
    // Press Ctrl + A
    send(Ctrl, true);
    send(A);
    send(Ctrl, false);

    // Press keys in sequence to spell out text
    send("Write this text...");
}));
```

`on` can be nested.
```c++
static bool enable = true;
on(Enter, enable); // Toggle boolean
on(enable, Action([&] {
    on(A, Action([&] {
        // Code...
    });
}));
```

**Important**: avoid using `if` statements in favor of `on` in `script()`. This ensures that rebinded keys are sent and released correctly when a condition is enabled/disabled.
```c++
// if
on(condition, Action([&] {
    // true
}));

// if/else
on(condition, Action([&] {
    // true
}), Action([&] {
    // false
}));
```

**Advanced usage**

Move steering controls from `arrow keys` to `WASD`, where `Enter` brings up an in game chat window. This is an useful bindings in many strategy games, e.g. Age of Empires, Company of Heroes. 
```c++
void script() {
    static bool enable = true;
    
    // Only apply when specified window is active
    on(isWindow("Some strategy game"), Action([&] {
        // Disable/Enable bindings when pressing "Enter" without silencing the key. 
        // This is useful to allow typing when "Enter" brings up the chat window. 
        // The flag "Keys::NoMute" prevents "Enter" from being "silenced"
        on(Enter - Keys::NoMute, enable);
        
        // Disable/Enable bindings when pressing Ctrl + Enter, if you somehow 
        // need to manually correct.
        on(Ctrl + Enter, enable);
        
        on(enable, Action([&] {
            // Rebind WASD to arrow keys
            on(W, ArrowUp);
            on(A, ArrowLeft);
            on(S, ArrowDown);
            on(D, ArrowRight);
            
            // Allow WASD by Ctrl-clicking
            on(Ctrl + W, W);
            on(Ctrl + A, A);
            on(Ctrl + S, S);
            on(Ctrl + D, D);
        }));
    }));
}
```
Execute something during regular intervals:
```c++
void script() {
    // Script code goes here (executed every key press)
}
void init() {
    // Do something every 100 millisecond
    every(100, [&] {
        // Keep sending A if A is pressed
        if(isPressed(A)) {
            send(A);
        }
    });
}
```
**Tips**

Feel free to experiement with the API. Most of the safe to use public methods are documented with Doxygen-style annotations.

## Scan codes

Please note that rebinding of keys are based on scan code. The predefined keys are based on a US keyboard layout. Please consult the following chart to make modifications if unsure:

![Keyboard scan codes](/Doc/Images/scancodes.jpg)

## Possible TODOs

* Expose a scripting API using Lua
* Reading and sending of mouse movement
* Screen grabbing/reading
* Support different keyboard layouts

## Portability

Only usable on Windows currently, but most of the logic is platform independent and can be compiled under both Linux and Windows. Feel free to help out if you have the time.

## Third parties

Low level keyboard reading are handled with [Interception](https://github.com/oblitum/Interception) by Oblitum.


## License
Licensed under CC BY 4.0. Third party libraries are licensed under own respective licenses. 


See [LICENSE.md](LICENSE.md) for more information.