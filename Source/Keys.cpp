#include "Keys.h"

namespace kh {

bool containsKey(const std::set<Key>& set, Key key) {
    if (set.count(key) > 0) {
        return true;
    }
    for (ScanCode relative : key.getAlternatives()) {
        if (set.count(relative) > 0) {
            return true;
        }
    }
    return false;
}
bool eraseKey(std::set<Key>& set, Key key) {
    if (set.count(key) > 0) {
        set.erase(key);
        return true;
    }
    for (ScanCode relative : key.getAlternatives()) {
        if (set.count(relative) > 0) {
            set.erase(relative);
            return true;
        }
    }
    return false;
}
}