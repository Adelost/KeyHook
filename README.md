# KeyHook

![KeyHook captain hook](/Doc/Images/cptnhook.png)

KeyHook provides low level rebinding of both keyboard keys and mouse keys, as well as creation of macros. It is especially useful for both games and applications which do no support rebinding of keys. The bindings are built and run as a normal C++ program.

If you ever wanted to rebind your WASD-keys to your scroll wheel (just to spice things up a bit), look no further!

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
Rebind `mouse scrolling` to `arrow keys`, and vice versa.
```c++
// Rebind mouse scroll
on(MouseScrollDown, ArrowDown);
on(MouseScrollUp, ArrowUp);
on(MouseScrollLeft, ArrowLeft);
on(MouseScrollRight, ArrowRight);

// Rebind arrow keys
on(ArrowDown, MouseScrollDown);
on(ArrowUp, MouseScrollUp);
on(ArrowLeft, MouseScrollLeft);
on(ArrowRight, MouseScrollRight);
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

**Important**: avoid using `if` statements in favor of `on` in `script()`. This ensures that rebound keys are sent and released correctly when a condition is enabled/disabled.
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
        // Disable/Enable bindings when pressing "Enter" without silencing the key, 
        // which is useful to allow typing when "Enter" brings up the chat window. 
        // The flag "Send" (passed  using the `-` operator) prevents "Enter" from
        // being silenced.
        on(Enter - Send, enable);
        
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

Feel free to experiement with the API. Most of the safe to use [public methods](https://github.com/Adelost/KeyHook/blob/master/Source/KeyHook.h) are documented with Doxygen-style annotations.



## Scan codes

Please note that rebinding of keys are based on scan code. The predefined keys are based on a US keyboard layout. Please consult the following chart to make modifications if unsure:

![Keyboard scan codes](/Doc/Images/scancodes.jpg)

## Portability

Only usable on Windows currently, but most of the logic is platform independent and can be compiled under both Linux and Windows. Feel free to help out if you have the time.

## Build and Installation on Windows

KeyHook requires installing the third-party lib [Interception](https://github.com/oblitum/Interception).

1. Download [Interception.zip ](https://github.com/oblitum/Interception/releases) v.1.0.1
2. Install Interception drivers by running `install-interception.exe` as administrator
3. IMPORTANT: Restart computer for drivers to work

The preferred way of building KeyHook is with [Cmake](https://cmake.org/install/).

CMake gives you multiple options of building the executable.

If you have CMake added to you PATH, and have "Visual Studio 15 2017" or 
similar installed, here is one way it can be done:

With Git Bash installed you can simply execute the BUILD.sh for a default build. The binaries are found in `KeyHook\build\Release`.

You can also perform the steps manually by executing the following commands in almost any terminal:

```
# From root directory of KeyHook open any terminal and perform the following commands

# For building x86
mkdir build32
cd build32
cmake -G "Visual Studio 15 2017" ..
cmake --build . --config Release
cd ..

# For building x64
mkdir build64
cd build64
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
```

## Possible TODOs

* Expose a scripting API using Lua
* Reading and sending of mouse movement
* Screen grabbing/reading
* Support different keyboard layouts

## Third parties

Low level keyboard reading on Windows are handled with [Interception](https://github.com/oblitum/Interception) by Oblitum. Note: KeyHook was originally based on WinApi LowLevelKeyboardProc, however this method does not work for some games, especially older games based on DirectInput.

## License
Licensed under CC BY 4.0. Third party libraries are licensed under own respective licenses. 

See [LICENSE.md](LICENSE.md) for more information.
