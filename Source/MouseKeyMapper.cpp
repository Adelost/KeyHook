#include <interception.h>
#include "MouseKeyMapper.h"

namespace kh {


MouseKeyMapper::MouseKeyMapper() {
    add(ScanCode::MouseLeft, 1, 2);
    add(ScanCode::MouseRight, 4, 8);
    add(ScanCode::MouseMiddle, 16, 32);
    add(ScanCode::MouseBack, 64, 128);
    add(ScanCode::MouseForward, 256, 512);

    addScroll(ScanCode::MouseScrollDown, 1024, -120);
    addScroll(ScanCode::MouseScrollUp, 1024, 120);
    addScroll(ScanCode::MouseTiltLeft, 2048, -120);
    addScroll(ScanCode::MouseTiltRight, 2048, 120);
}

void MouseKeyMapper::add(ScanCode code, int pressState, int releaseState) {
    m_pressCodes.insert({pressState, code});
    m_releaseCodes.insert({releaseState, code});
    m_pressStates.insert({code, pressState});
    m_releaseStates.insert({code, releaseState});
}

void MouseKeyMapper::addScroll(ScanCode code, int state, int roll) {
    auto& rolls = m_scrollRolls;
    if (!rolls.count(state)) {
        rolls[state] = std::unordered_map<int, ScanCode>();
    }
    rolls[state][roll] = code;
    auto& states = m_scrollStates;
    states[code] = {state, roll};
}

bool MouseKeyMapper::hasCodeFor(int code) {
    if (m_pressCodes.count(code)) {
        return true;
    }
    if (m_releaseCodes.count(code)) {
        return true;
    }
    return false;
}
void MouseKeyMapper::getCode(int code, Key* key, bool* pressed) {
    if (m_pressCodes.count(code)) {
        *key = m_pressCodes[code];
        *pressed = true;
    }
    if (m_releaseCodes.count(code)) {
        *key = m_releaseCodes[code];
        *pressed = false;
    }
}
bool MouseKeyMapper::hasStateFor(ScanCode code) {
    if (m_pressStates.count(code)) {
        return true;
    }
    if (m_releaseStates.count(code)) {
        return true;
    }
    return false;
}
void MouseKeyMapper::getState(ScanCode code, bool pressed, InterceptionMouseStroke* stroke) {
    if (hasScrollStateFor(code)) {
        auto state = getScrollState(code);
        stroke->state = (unsigned short)state.state;
        stroke->rolling = (unsigned short)state.roll;
    } else if (pressed && m_pressStates.count(code)) {
        stroke->state = (unsigned short) m_pressStates[code];
    } else if (!pressed && m_releaseStates.count(code)) {
        stroke->state = (unsigned short) m_releaseStates[code];
    }
}
}