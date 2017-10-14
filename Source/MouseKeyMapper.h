#pragma once

#include <unordered_map>
#include <interception.h>
#include "ScanCodes.h"
#include "ScanCodes.h"
#include "Keys.h"
#include "EnumClassHash.h"

namespace kh {
class MouseKeyMapper {
    struct ScrollState {
        int state;
        int roll;
    };
    std::unordered_map<int, ScanCode> m_pressCodes;
    std::unordered_map<int, ScanCode> m_releaseCodes;
    std::unordered_map<ScanCode, int, EnumClassHash> m_pressStates;
    std::unordered_map<ScanCode, int, EnumClassHash> m_releaseStates;
    std::unordered_map<int, std::unordered_map<int, ScanCode>> m_scrollRolls;
    std::unordered_map<ScanCode, ScrollState, EnumClassHash> m_scrollStates;

public:
    MouseKeyMapper();
    void add(ScanCode code, int pressState, int releaseState);
    void addScroll(ScanCode code, int state, int roll);
    bool hasScrollCodeFor(int state, int roll) {
        auto& rolls = m_scrollRolls;
        if(rolls.count(state)){
            auto& codes = rolls[state];
            if(codes.count(roll)) {
                return true;
            }
        }
        return false;
    }
    ScanCode getScrollCode(int state, int roll) {
        return m_scrollRolls[state][roll];
    }
    bool hasScrollStateFor(ScanCode code) {
        auto& states = m_scrollStates;
        return states.count(code) != 0;
    }
    ScrollState getScrollState(ScanCode code) {
        auto& states = m_scrollStates;
        return states[code];
    }
    bool hasCodeFor(int state);
    void getCode(int code, Key* key, bool* pressed);
    void getState(ScanCode code, bool pressed, InterceptionMouseStroke* stroke);
    bool hasStateFor(ScanCode code);
};
}