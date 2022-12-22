//
// Created by lucas on 12/18/22.
//

#ifndef ACE_CONFIG_HPP
#define ACE_CONFIG_HPP

#include <vector>

class Config {
public:
    //std::vector<std::pair<KeySym, (*func)(int)>> =

    unsigned int gapSize = 10;
    unsigned int borderSize = 2;

    // border colors:
    int neutralBorderColor = 0xffffff;
    int focusedBorderColor = 0xff0000;
    int selectedBorderColor = 0x002aff;

};

#endif //ACE_CONFIG_HPP
