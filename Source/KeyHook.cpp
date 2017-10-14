#ifdef _WIN32_

#include <Windows.h>
#include <interception.h>

#endif

#include "KeyHook.h"
#include "OSWrap.h"
#include "MouseKeyMapper.h"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <exception>
#include <memory>
#include <unordered_map>


namespace kh {

namespace Exceptions {
struct UnkownReading {
};
}


#ifdef _WIN32_
InterceptionStroke s_stroke;
InterceptionContext s_context;
InterceptionDevice s_device;
std::unique_ptr<InterceptionDevice> s_mouseDevice;
std::unique_ptr<InterceptionDevice> s_keyboardDevice;
MouseKeyMapper s_mouseKeyMapper;


int interceptionGetCurrentKey(InterceptionKeyStroke stroke) {
    int code = stroke.code;
    int state = stroke.state;
    if (state > 1) {
        code += 128;
    }
    return code;
}
bool interceptionGetIsPressed(InterceptionKeyStroke stroke) {
    int state = stroke.state;
    if (state > 1) {
        state -= 2;
    }
    return state == 0;
}

void interceptionGetMouseStroke(InterceptionMouseStroke* mouseStroke, Key* key, bool* isPressed) {
    int state = mouseStroke->state;
    int roll = mouseStroke->rolling;
    if (state == 0 || mouseStroke->flags != 0 || mouseStroke->information != 0) {
        throw Exceptions::UnkownReading();
    }
    if (s_mouseKeyMapper.hasScrollCodeFor(state, roll)) {
        *key = s_mouseKeyMapper.getScrollCode(state, roll);
        key->setEvent(true);
        *isPressed = true;
        std::cout << "scroll reading " << std::endl;
    } else {
        s_mouseKeyMapper.getCode(state, key, isPressed);
    }
}

bool isMouseKey(Key key) {
    int value = (int) key.getCode();
    return value > 1000;
}

bool isKeyboardKey(Key key) {
    int value = (int) key.getCode();
    return value < 1000;
}

void interceptionSend(Key key, bool pressed) {
    if (isMouseKey(key)) {
        if (s_mouseDevice) {
            InterceptionMouseStroke stroke = {0};
            s_mouseKeyMapper.getState(key.getCode(), pressed, &stroke);
            interception_send(s_context, *s_mouseDevice, (InterceptionStroke const*) &stroke, 1);
        } else {
            std::cout << "No mouse device yet detected" << std::endl;
        }
    }
    if (isKeyboardKey(key)) {
        if (s_keyboardDevice) {
            int code = (int) key.getCode();
            int state = !pressed;
            if (code >= 128) {
                code -= 128;
                state += 2;
            }
            InterceptionKeyStroke stroke = {0};
            stroke.code = (unsigned short) code;
            stroke.state = (unsigned short) state;
            interception_send(s_context, *s_keyboardDevice, (InterceptionStroke const*) &stroke, 1);
        } else{
            std::cout << "No keyboard device yet detected" << std::endl;
        }
    }
}
#endif

ScanCode charToScanCode(char c) {
#ifdef _WIN32_
    SHORT virtualKey = VkKeyScan(c);
    UINT scanCode = MapVirtualKey((UINT) virtualKey, 0);
    return (ScanCode) scanCode;
#else
    return (ScanCode)c;
#endif
}

static KeyHook* s_hook;
static std::mutex s_mutex;
static std::vector<std::thread*> s_threads;

bool startsWith(std::string str, std::string prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    for (int i = 0; i < prefix.length(); i += 1) {
        if (str[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

void KeyHook::sendBlind(Key key, bool pressed) {
    m_sendBuffer.push_back({key, pressed, true});
}

void KeyHook::send(Key key, bool pressed) {
    m_sendBuffer.push_back({key, pressed, false});
}


std::set<Key> KeyHook::extractModKeys() {
    std::set<Key> mods;
    for (Key key : m_hardwareKeys) {
        if (isModKey(key) && !isCurrentKey(key)) {
            mods.insert(key);
        }
    }
    // Shift acts as a soft modifier
    containsKey(mods, Shift);
    if (mods.size() == 1 && containsKey(mods, Shift)) {
        eraseKey(mods, Shift);
    }
    for (Key mod : mods) {
        rawSend(mods, false);
    }
    return mods;
}

bool KeyHook::isValidMods(Keys keys) {
    std::set<Key> extraMods = getModKeys();
    // Shift special case
    if (extraMods.size() == 1 && containsKey(extraMods, Shift)) {
        return true;
    }
    for (Key key: keys.list) {
        eraseKey(extraMods, key);
    }
    return extraMods.empty();
}

std::set<Key> KeyHook::getModKeys() {
    std::set<Key> mods;
    for (Key key : m_hardwareKeys) {
        if (isModKey(key)) {
            mods.insert(key);
        }
    }
    return mods;
}

void KeyHook::rawSend(std::set<Key> keys, bool pressed) {
    for (auto key : keys) {
        rawSend(key, pressed);
    }
}

void KeyHook::start() {
#ifdef _WIN32_
    s_context = interception_create_context();
    interception_set_filter(s_context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(s_context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);
    while (interception_receive(s_context, s_device = interception_wait(s_context), (InterceptionStroke*) &s_stroke,
                                1) > 0) {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (m_exiting) {
            break;
        }
        initHook();
        try {
            readInput();
            sense.init();
            runScript();
            sense.clear();
            if (m_exiting) {
                break;
            }
        }
        catch (Exceptions::UnkownReading e) {
            interception_send(s_context, s_device, (InterceptionStroke const*) &s_stroke, 1);
        }
    }
    releaseAllKeys();
    exitThreads();
    std::cout << "exit main thread" << std::endl;
    interception_destroy_context(s_context);
#endif
}
void KeyHook::runScript() {
    preScript();
    script();
    postScript();
    if (m_currentKey.isEvent()) {
        m_pressed = false;
        preScript();
        script();
        postScript();
    }
}
void KeyHook::initHook() {
    s_hook = this;
    if (!s_hook->m_inited) {
        s_hook->init();
        m_inited = true;
    }
}
void KeyHook::readInput() const {
#ifdef _WIN32_
    if (interception_is_mouse(s_device)) {
        if (!s_mouseDevice) {
            s_mouseDevice = std::make_unique<InterceptionDevice>(s_device);
            std::cout << "Mouse device detected" << std::endl;
        }
        interceptionGetMouseStroke((InterceptionMouseStroke*) &s_stroke, &s_hook->m_currentKey, &s_hook->m_pressed);
    } else if (interception_is_keyboard(s_device)) {
        if (!s_keyboardDevice) {
            s_keyboardDevice = std::make_unique<InterceptionDevice>(s_device);
            std::cout << "Keyboard device detected" << std::endl;
        }
        InterceptionKeyStroke& keyStroke = *(InterceptionKeyStroke*) &s_stroke;
        s_hook->m_currentKey = (ScanCode) interceptionGetCurrentKey(keyStroke);
        s_hook->m_pressed = interceptionGetIsPressed(keyStroke);
    }
    s_hook->m_window = getActiveWindow();
#endif
}

void KeyHook::exitThreads() const {
    for (std::thread* thread : s_threads) {
        std::cout << "exit thread " << thread->get_id() << std::endl;
        thread->join();
        delete thread;
    }
}

void KeyHook::spoof(Key key, bool pressed) {
    m_debug = true;
    s_hook = this;
    m_currentKey = key;
    m_pressed = pressed;
    m_window = "Spoof";
    runScript();
    m_debug = false;
}


void KeyHook::preScript() {
    m_callPath = "";
    if (m_pressed) {
        m_hardwareKeys.insert(m_currentKey);
    } else {
        registerReleased(m_currentKey);
    }
}

void KeyHook::registerReleased(Key key) {
    m_hardwareKeys.erase(key);
    if (containsKey(m_modStash, key)) {
        eraseKey(m_modStash, key);
        mute(key);
    }
}

void KeyHook::insertKeys(std::set<Key> keys) {
    for (Key key : keys) {
        m_hardwareKeys.insert(key);
        rawSend(key, true);
    }
}

void KeyHook::unmute(Key key) {
//    std::cout << "unmuted: " << key.toStr() << std::endl;
    eraseKey(m_mutedKeys, key);
}
bool KeyHook::isMuted(Key key) {
    return containsKey(m_mutedKeys, key);
}

void KeyHook::postScript() {
    bool unmuted = false;
    if (!isMuted(m_currentKey)) {
        if (isStashedMods()) {
            unstashMods();
        }
        std::cout << "=";
        rawSend(currentKey(), isPressed());
    } else {
        if (!isPressed(m_currentKey)) {
            unmute(m_currentKey);
            unmuted = true;
        }
    }
    sendBuffer();
    if (unmuted && isStashedMods()) {
        unstashMods();
    }

}

void KeyHook::sendBuffer() {
    for (QueuedKey q: m_sendBuffer) {
        if (q.blind) {
            if (isStashedMods()) {
                unstashMods();
            }
            rawSend(q.key, q.pressed);
        } else {
            if (!isStashedMods()) {
                stashMods();
            }
            rawSend(q.key, q.pressed);
        }
    }
    m_sendBuffer.clear();
}

void KeyHook::rawSend(Key key, bool pressed) {
    if (!m_debug) {
#ifdef _WIN32_
        interceptionSend(key, pressed);
#endif
    }
    std::cout << "=> " << key.toStr() << " " << pressed << std::endl;
}
void KeyHook::on(Condition given, Action then, Action otherwise) {
    std::string path = m_callPath;
    int callCode = given.call();
    if (callCode == 1) {
        if (given.hasFlag(Keys::Mute)) {
            mute(currentKey());
        }
        unTrack(path + "2");
        m_callPath += "1";
        then.call(m_callPath);
    } else if (callCode == 0) {
        unTrack(path + "1");
        m_callPath += "2";
        otherwise.call(m_callPath);
    }
    m_callPath = path + "0";
}
bool KeyHook::isWindow(std::string windowName) {
    return currentWindow() == windowName;
}

bool KeyHook::isPressed(Key key) {
    return containsKey(m_hardwareKeys, key);
}

bool KeyHook::isPressed(Keys keys) {
    if (keys.list.empty()) {
        return true;
    }
    for (Key key : keys.list) {
        if (!s_hook->isPressed(key)) {
            return false;
        }
    }
    return true;
}
void KeyHook::mute(Key key) {
//    std::cout << "muted: " << key.toStr() << std::endl;
    m_mutedKeys.insert(key);
}
void KeyHook::send(std::string text) {
    for (char c : text) {
        bool upper = isupper(c);
        if (upper) {
            c = (char) tolower(c);
        };
        ScanCode code = charToScanCode(c);
        if (upper) {
            sendBlind(LShift, true);
        }
        send(code, true);
        send(code, false);
        if (upper) {
            sendBlind(LShift, false);
        }
    }
}


void KeyHook::every(int ms, std::function<void()> callback) {
    std::thread* thread = new std::thread([&, ms, callback] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            std::lock_guard<std::mutex> lock(s_mutex);
            if (m_exiting) {
                break;
            }
            sense.init();
            callback();
            sendBuffer();
            sense.clear();
            if (m_exiting) {
                break;
            }

//            std::cout << "task1 says:s " <<  << std::endl;
        }
    });
    s_threads.push_back(thread);
    std::cout << "thread created" << std::endl;
}
void KeyHook::releaseAllKeys() {
    for (Key key : m_hardwareKeys) {
        rawSend(key, false);
    }
    m_hardwareKeys.clear();
}


int Condition::call() {
    return m_pressCode;
}

Condition::Condition(Keys keys) : Condition() {
    if (!keys.hasFlag(Keys::NoMute)) {
        m_flags.insert(Keys::Mute);
    }
    m_pressCode = s_hook->getPressCode(keys);
}

void Action::call(std::string path) {
    m_path = path;
    if (!m_noActivate) {
        m_activate();
    }
}

Action::Action(Keys keys) {
    m_activate = [=] {
        s_hook->send(keys, true);
        s_hook->track([=] {
            s_hook->send(keys, false);
        });
    };
    m_noActivate = false;
}
Action::Action(bool& toggle) {
    m_activate = [&] {
        toggle = !toggle;
    };
    m_noActivate = false;
}
Action::Action(std::string text) {
    m_activate = [=] {
        s_hook->send(text);
    };
    m_noActivate = false;
}

void PathTracker::unTrack(std::string path) {
    for (int i = 0; i < m_tracked.size();) {
        Tracked& tracked = m_tracked[i];
        if (withinPath(tracked, path)) {
            tracked.callback();
            m_tracked.erase(m_tracked.begin() + i);
//            std::cout << "untrack: " << path << std::endl;
            continue;
        }
        i += 1;
    }
}
bool PathTracker::withinPath(PathTracker::Tracked& tracked, std::string path) {
    return startsWith(tracked.path, path);

}
void PathTracker::track(std::string path, std::function<void()> callback) {
    if (!isTracked(path)) {
        m_tracked.push_back({path, callback});
//        std::cout << "track: " << path << std::endl;
    }
}

}