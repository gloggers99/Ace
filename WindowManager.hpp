//
// Created by lucas on 12/7/22.
//

#ifndef ACE_WINDOWMANAGER_HPP
#define ACE_WINDOWMANAGER_HPP

#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <thread>
#include <sstream>

extern "C" {
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/keysym.h>
    #include <X11/Xatom.h>
};

#include "Config.hpp"

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class WindowManager {
private:
    Config config;

    Display *display;
    int screen;
    Window root;
    std::vector<Window> windows;

    Window focusedWindow = 0;

    /* hinter */
    int gridX = 0;
    int gridY = 0;

    /* callbacks */
    static int x_error_handler(Display *display,
                               XErrorEvent *e) {
        std::cout << "XError triggered\n" << "\n";

        return 0;
    }

    /* helpers */
    void bindKey(KeySym keySym, unsigned int mod);
    void bindButton(unsigned int button, unsigned int mod);
    std::vector<Window> getCurrentWindows(bool queryTree);
    bool isWindowValid(Window window);

    /* other */
    void commandDispatcher(std::string input);

    /* window manager functions */
    void focusWindow(Window window  );
    void killWindow(Window window);
    void run();
    void shift(Direction direction);
    void shutdown();

public:
    explicit WindowManager(Config config);
    ~WindowManager();
};


#endif //ACE_WINDOWMANAGER_HPP
