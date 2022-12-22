//
// Created by lucas on 12/18/22.
//

#ifndef ACE_CONFIG_HPP
#define ACE_CONFIG_HPP

#include <map>

class Config {
public:
    std::map<std::pair<unsigned int, KeySym>, std::string> keyBinds = {
            {{Mod4Mask, XK_w}, "shift up"},
            {{Mod4Mask, XK_a}, "shift left"},
            {{Mod4Mask, XK_s}, "shift down"},
            {{Mod4Mask, XK_d}, "shift right"},
            {{Mod4Mask | ShiftMask, XK_c}, "kill"},
            {{Mod4Mask | ShiftMask, XK_q}, "shutdown"}
    };

    unsigned int gapSize = 10;
    unsigned int borderSize = 2;

    // border colors:
    int neutralBorderColor = 0xffffff;
    int focusedBorderColor = 0xff0000;
    int selectedBorderColor = 0x002aff;

};

#endif //ACE_CONFIG_HPP
