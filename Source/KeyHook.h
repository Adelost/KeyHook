#pragma once

#include <iostream>
#include <functional>
#include <set>
#include <vector>
#include "Keys.h"
#include "OSWrap.h"

namespace kh {


class KeyHook;


class Condition {
public:
    Condition() : Condition(true) {}
    Condition(bool active) {
        m_pressCode = active ? 1 : 0;
    }
    Condition(int pressCode, std::set<Keys::Flags> flags = {}) {
        m_flags = flags;
        m_pressCode = pressCode;
    }
    Condition(std::function<int()> callback) : Condition(callback()) {}
    Condition(std::function<bool()> callback) : Condition(callback()) {}
    Condition(Keys keys);
    Condition(Key key) : Condition(Keys(key)) {}
    bool hasFlag(Keys::Flags flag) {
        return m_flags.count(flag) > 0;
    };
    int call();

private:
    int m_pressCode;
    std::set<Keys::Flags> m_flags;
};

class Action {
public:
    Action() {}
    Action(bool& toggle);
    Action(std::string text);

    Action(Key key) : Action(Keys(key)) {}
    Action(Keys keys);
    Action(std::function<void()> activate) {
        m_activate = activate;
        m_noActivate = false;
    }
    Action(std::function<void()> activate, std::function<void()> deactivate) {
        m_activate = activate;
        m_noActivate = false;
    }
    void call(std::string path);
    std::string path() const { return m_path; }

private:
    std::function<void()> m_activate;
    bool m_noActivate = true;
    std::string m_path = "";
};

struct QueuedKey {
    Key key;
    bool pressed;
    bool blind;
};

class PathTracker {
public:
    struct Tracked {
        std::string path;
        std::function<void()> callback;
    };
    void track(std::string path, std::function<void()> callback);
    void unTrack(std::string path);
    bool isTracked(std::string path) {
        for (Tracked tracked: m_tracked) {
            if (tracked.path == path) {
                return true;
            }
        }
        return false;
    }

private:
    bool withinPath(Tracked& tracked, std::string path);
    std::vector<Tracked> m_tracked;
};


class KeyHook : public KeysInherit {
public:
    KeyHook() {}
    virtual ~KeyHook() {}

    /// Starts the execution.
    void start();
    /// Simulates a key event. Useful for debugging.
    void spoof(Key key, bool pressed);
    void spoof(Key key) {
        spoof(key, true);
        spoof(key, false);
    };
    /// If else condition based on either bool or pressCode.
    /// Activates action "then" if condition is true or pressCode is 1 (activated),
    /// or activates "otherwise" (optional) if false or pressCode is 0 (disabled).
    /// Does nothing if press code is 2 (active).
    /// N.B. on(A, B) is equivalent to on(getPressCode(A), B).
    /// E.g. on(isPressed(A), B), sends A pressed (if key is not muted elsewhere) and sends B.
    /// E.g. on(getPressCode(A), B), sends key B if A is currently pressed and mutes A.
    void on(Condition given, Action then, Action otherwise = Action());

    /// Returns true if key is pressed.
    bool isPressed(Key key);
    /// Returns true if all keys are pressed.
    bool isPressed(Keys keys);
    /// Similar to isPressed() but does not allow any additional keys
    /// to be pressed.
    bool isExactly(Keys keys) {
        return isPressed(keys) && m_hardwareKeys.size() == keys.list.size();
    }
    /// Returns true if active and the last key in keys
    /// is the key currently pressed.
    bool isActivated(Keys keys) {
        if (!isActive(keys)) {
            return false;
        }
        return isCurrentKey(keys);
    }
    /// Returns true if all keys are pressed and modifiers are valid.
    /// E.g. Ctrl + A is active when Ctrl and A is pressed.
    /// Mod keys cannot be combined with other mod keys, normal keys
    /// can be combined, e.g. both A and A + B are active when A
    /// and B is pressed.
    /// A is not active when Ctrl + A is pressed.
    /// Ctrl + A is not active when Ctrl + Shift + A is pressed.
    bool isActive(Keys keys) {
        if (!isPressed(keys)) {
            return false;
        }
        return isValidMods(keys);
    }
    /// Returns 1 if key combo is activated: all keys are pressed and the
    /// last key is the key currently pressed. This also mutes the key.
    /// Returns 2 if active: all keys are pressed, but the last key is not actively pressed.
    /// Returns 0 otherwise.
    int getPressCode(Keys keys) {
        if (!isPressed(keys)) {
            return 0;
        }
        if (!isValidMods(keys)) {
            return 0;
        }
        if (isActivated(keys)) {
            return 1;
        }
        return 2;
    }
    /// Returns true if all modifiers in keys (if any) is pressed.
    /// Returns false if any additional modifiers are pressed not part of keys.
    /// Shift is a special case, if Shift is the only additional modifier returns true
    /// E.g. If Ctrl and A is pressed, Ctrl + A is true.
    /// E.g. If Shift, Ctrl and A is pressed, Ctrl + A is not true since additional
    /// modifiers are pressed.
    bool isValidMods(Keys keys);
    /// Returns the name of the currently active window.
    std::string currentWindow() { return m_window; }
    /// Returns true if the name of the currently active window is windowName.
    bool isWindow(std::string windowName);
    /// Returns the currently pressed key.
    Key currentKey() { return m_currentKey; }
    /// Returns true if the currently sent key is pressed.
    bool isPressed() { return m_pressed; }
    /// Sends key with modifiers filtered out.
    /// E.g. if Ctrl is pressed while sending B, Ctrl is release before
    /// sending and then pressed again.
    void send(Key key, bool pressed);
    /// Sends keys.
    void send(Keys keys, bool pressed) {
        for (Key key : keys.list) {
            send(key, pressed);
        }
    }
    void send(Keys keys) {
        send(keys, true);
        send(keys, false);
    }
    /// Sends text
    void send(std::string text);
    /// Send key without releasing modifiers first.
    void sendBlind(Key key, bool pressed);
    /// Returns any currently pressed modifier keys.
    std::set<Key> getModKeys();
    /// Prevents the key from being sent when pressed.
    void mute(Key key);;
    /// Mutes multiple keys.
    void mute(std::initializer_list<Key> keys) {
        for (Key key : keys) {
            mute(key);
        }
    }
    /// Unmutes a key,
    void unmute(Key key);
    /// Returns true if key has been muted
    bool isMuted(Key key);
    /// Returns true if key is the currently pressed key.
    bool isCurrentKey(Key key) {
        return key.contains(currentKey().getCode());
    }
    /// Returns true if the last key in keys is the currently pressed key or if
    /// keys are empty.
    bool isCurrentKey(Keys keys) {
        if (keys.list.empty()) {
            return true;
        }
        return isCurrentKey(keys.list.back());
    }
    /// Returns true for key not a modifier key, e.g. A, B, 1, 2, Return, Enter
    bool isNormalKey(Key key) {
        return !isModKey(key);
    }
    /// Returns if key is a modifier key, i.e. Shift, Ctrl or Alt.
    bool isModKey(Key key) {
        switch (key.getCode()) {
            case ScanCode::LShift: return true;
            case ScanCode::RShift: return true;
            case ScanCode::LCtrl: return true;
            case ScanCode::RCtrl: return true;
            case ScanCode::LAlt: return true;
            case ScanCode::RAlt: return true;
            default: break;
        }
        return false;
    }

    void track(std::function<void()> action) {
        m_tracker.track(m_callPath, action);
    }

    void unTrack(std::string path) {
        m_tracker.unTrack(path);
    }
    void exit() {
        m_exiting = true;
    }
    void releaseAllKeys();
    void every(int ms, std::function<void()> callback);
    Sense sense;

    Action Exit = Action([&] { exit(); });

private:
//    std::string callPath() { return m_callPath; }
    void preScript();
    void postScript();
    std::set<Key> extractModKeys();
    void rawSend(std::set<Key> keys, bool pressed);
    bool isStashedMods() {
        return !m_modStash.empty();
    }
    void stashMods() {
        m_modStash = extractModKeys();
    }

    void unstashMods() {
        insertKeys(m_modStash);
        m_modStash.clear();
    }
    void sendBuffer();
    void exitThreads() const;

    bool m_pressed;
    std::string m_window;
    Key m_currentKey;
    std::string m_callPath = "";
    std::set<Key> m_hardwareKeys;
    std::set<Key> m_mutedKeys;
    PathTracker m_tracker;
    bool m_debug = false;
    std::vector<QueuedKey> m_sendBuffer;
    std::set<Key> m_modStash;
    bool m_inited = false;
    bool m_exiting = false;

protected:
    virtual void script() = 0;
    virtual void init() {};

    void insertKeys(std::set<Key> keys);
    void rawSend(Key key, bool pressed);

    void readInput() const;
    void initHook();
    void registerReleased(Key key);
    void runScript();
};

}