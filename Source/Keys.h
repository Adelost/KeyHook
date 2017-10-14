#pragma once

#include <string>
#include <vector>
#include <set>
#include "ScanCodes.h"

namespace kh {

/// Holds
class Key {
public:
    Key() {
    }
    Key(ScanCode code) {
        m_code = code;
    }
    Key(ScanCode code, std::vector<ScanCode> alternatives) {
        m_code = code;
        m_alternatives = alternatives;
    }
    std::string toStr() const {
        return scanCodeToStr(m_code);
    }
    ScanCode getCode() const {
        return m_code;
    }
    void setEvent(bool state) {
        m_event = state;
    }
    bool isEvent() const {
        return m_event;
    }
    std::vector<ScanCode> getAlternatives() const {
        return m_alternatives;
    }
    bool contains(const ScanCode& code) const {
        if (getCode() == code) {
            return true;
        }
        for (ScanCode alt : m_alternatives) {
            if (alt == code) {
                return true;
            }
        }
        return false;
    }
    bool operator<(const Key& right) const {
        return m_code < right.m_code;
    }
    bool operator==(const Key& right) const {
        return m_code == right.m_code;
    }

private:
    bool m_event = false;
    ScanCode m_code;
    std::vector<ScanCode> m_alternatives;
};

class Keys {
public:
    enum Flags {
        Mute,
        NoMute
    };
    Keys() {};
    Keys(Key key) {
        list.push_back(key);
    };
    bool hasFlag(Flags flag) {
        return flags.count(flag) > 0;
    };
    std::vector<Key> list;
    std::set<Flags> flags;
};

inline Keys operator+(const Key& left, const Key& right) {
    Keys keys;
    keys.list.push_back(left);
    keys.list.push_back(right);
    return keys;
}

inline Keys operator+(const Keys& keys, const Key& key) {
    Keys copy = keys;
    copy.list.push_back(key);
    return copy;
}

inline Keys operator-(const Keys& keys, const Keys::Flags& flag) {
    Keys copy = keys;
    copy.flags.insert(flag);
    return copy;
}


bool containsKey(const std::set<Key>& set, Key key);

bool eraseKey(std::set<Key>& set, Key key);


class KeysInherit {
public:
    Key Enter = Key(ScanCode::Return, {ScanCode::NumpadEnter});
    Key Ctrl = Key(ScanCode::LCtrl, {ScanCode::RCtrl});
    Key Alt = Key(ScanCode::LAlt, {ScanCode::RAlt});
    Key Shift = Key(ScanCode::LShift, {ScanCode::RShift});
    Key Win = Key(ScanCode::LWin, {ScanCode::RWin});

    Key Esc = ScanCode::Esc;
    Key Num1 = ScanCode::Num1;
    Key Num2 = ScanCode::Num2;
    Key Num3 = ScanCode::Num3;
    Key Num4 = ScanCode::Num4;
    Key Num5 = ScanCode::Num5;
    Key Num6 = ScanCode::Num6;
    Key Num7 = ScanCode::Num7;
    Key Num8 = ScanCode::Num8;
    Key Num9 = ScanCode::Num9;
    Key Num0 = ScanCode::Num0;
    Key Minus = ScanCode::Minus;
    Key Plus = ScanCode::Plus;
    Key Backspace = ScanCode::Backspace;
    Key Tab = ScanCode::Tab;
    Key Q = ScanCode::Q;
    Key W = ScanCode::W;
    Key E = ScanCode::E;
    Key R = ScanCode::R;
    Key T = ScanCode::T;
    Key Y = ScanCode::Y;
    Key U = ScanCode::U;
    Key I = ScanCode::I;
    Key O = ScanCode::O;
    Key P = ScanCode::P;
    Key BracketLeft = ScanCode::BracketLeft;
    Key BracketRight = ScanCode::BracketRight;
    Key Return = ScanCode::Return;
    Key LCtrl = ScanCode::LCtrl;
    Key A = ScanCode::A;
    Key S = ScanCode::S;
    Key D = ScanCode::D;
    Key F = ScanCode::F;
    Key G = ScanCode::G;
    Key H = ScanCode::H;
    Key J = ScanCode::J;
    Key K = ScanCode::K;
    Key L = ScanCode::L;
    Key Semicolon = ScanCode::Semicolon;
    Key Quote = ScanCode::Quote;
    Key LShift = ScanCode::LShift;
    Key Backslash = ScanCode::Backslash;
    Key Z = ScanCode::Z;
    Key X = ScanCode::X;
    Key C = ScanCode::C;
    Key V = ScanCode::V;
    Key B = ScanCode::B;
    Key N = ScanCode::N;
    Key M = ScanCode::M;
    Key Colon = ScanCode::Colon;
    Key Period = ScanCode::Period;
    Key ForwardSlash = ScanCode::ForwardSlash;
    Key RShift = ScanCode::RShift;
    Key NumpadMultiply = ScanCode::NumpadMultiply;
    Key LAlt = ScanCode::LAlt;
    Key Space = ScanCode::Space;
    Key CapsLock = ScanCode::CapsLock;
    Key NumLock = ScanCode::NumLock;
    Key ScrollLock = ScanCode::ScrollLock;
    Key Numpad7 = ScanCode::Numpad7;
    Key Numpad8 = ScanCode::Numpad8;
    Key Numpad9 = ScanCode::Numpad9;
    Key NumpadMinus = ScanCode::NumpadMinus;
    Key Numpad4 = ScanCode::Numpad4;
    Key Numpad5 = ScanCode::Numpad5;
    Key Numpad6 = ScanCode::Numpad6;
    Key NumpadPlus = ScanCode::NumpadPlus;
    Key Numpad1 = ScanCode::Numpad1;
    Key Numpad2 = ScanCode::Numpad2;
    Key Numpad3 = ScanCode::Numpad3;
    Key Numpad0 = ScanCode::Numpad0;
    Key NumpadDecimalPoint = ScanCode::NumpadDecimalPoint;
    Key NumpadEnter = ScanCode::NumpadEnter;
    Key RCtrl = ScanCode::RCtrl;
    Key PrintScreen = ScanCode::PrintScreen;
    Key NumpadDivide = ScanCode::NumpadDivide;
    Key RAlt = ScanCode::RAlt;
    Key Home = ScanCode::Home;
    Key ArrowUp = ScanCode::ArrowUp;
    Key PageUp = ScanCode::PageUp;
    Key ArrowLeft = ScanCode::ArrowLeft;
    Key ArrowRight = ScanCode::ArrowRight;
    Key End = ScanCode::End;
    Key ArrowDown = ScanCode::ArrowDown;
    Key PageDown = ScanCode::PageDown;
    Key Insert = ScanCode::Insert;
    Key Delete = ScanCode::Delete;
    Key LWin = ScanCode::LWin;
    Key RWin = ScanCode::RWin;
    Key Menu = ScanCode::Menu;

    Key MouseLeft = ScanCode::MouseLeft;
    Key MouseRight = ScanCode::MouseRight;
    Key MouseMiddle = ScanCode::MouseMiddle;
    Key MouseForward = ScanCode::MouseForward;
    Key MouseBack = ScanCode::MouseBack;

    Key MouseScrollDown = ScanCode::MouseScrollDown;
    Key MouseScrollUp = ScanCode::MouseScrollUp;
    Key MouseTiltLeft = ScanCode::MouseTiltLeft;
    Key MouseTiltRight = ScanCode::MouseTiltRight;
};
}