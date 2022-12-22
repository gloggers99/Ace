//
// Created by lucas on 12/7/22.
//

#include "WindowManager.hpp"
#include <X11/keysymdef.h>

#include <utility>

/* helpers */

void WindowManager::bindKey(KeySym keySym, unsigned int modifier) {
    XGrabKey(this->display,
             XKeysymToKeycode(this->display, keySym),
             modifier,
             this->root,
             True,
             GrabModeAsync,
             GrabModeAsync);
}

void WindowManager::bindButton(unsigned int button, unsigned int mod) {
    XGrabButton(this->display,
                button,
                mod,
                this->root,
                True,
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync,
                0, 0);
}

std::vector<Window> WindowManager::getCurrentWindows(bool queryTree = false) {
    // We need to find the windows in the (simulated) viewport
    // So query every window and check if x >= 0 && y <= 0

    std::vector<Window> allWindows = this->windows;
    std::vector<Window> currentWindows;

    if (queryTree) {
        std::cerr << "using getCurrentWindows::queryTree method, results may vary!\n";

        // clear currentWindows because we are querying the tree for windows
        allWindows.clear();

        // function return values:
        Window *root_return;            // useless
        Window *parent_return;          // useless
        Window **children_return;       // we need this
        unsigned int *nchildren_return; // and this

        XQueryTree(this->display,
                   this->root,
                   root_return,
                   parent_return,
                   children_return,
                   nchildren_return);

        for (unsigned int i = 0; i < *nchildren_return; i++) {
            allWindows.push_back(*children_return[i]);
        }
    }

    for (Window window : allWindows) {
        XWindowAttributes windowAttributes;

        XGetWindowAttributes(this->display, window, &windowAttributes);

        // What constitutes a visible window?
        // x >= 0 && width <= (DisplayWidth - x)
        // y <= 0 && height <= (DisplayHeight - y)

        if (windowAttributes.x >= 0 && windowAttributes.width <= (XDisplayWidth(this->display, 0) - windowAttributes.x)
            && windowAttributes.y >= 0 && windowAttributes.height <= (XDisplayHeight(this->display, 0) - windowAttributes.y)) {
            currentWindows.push_back(window);
        }
    }

    return currentWindows;
}

bool WindowManager::isWindowValid(Window window) {
    // the window is valid if its part of this->windows
    // and it's not equal to 0.

    if (std::find(this->windows.begin(), this->windows.end(), window) != this->windows.end()
     && window != 0) {
        return true;
    }

    return false;
}

/* other */

void WindowManager::commandDispatcher(std::string input) {
    std::stringstream commandStream(input);
    std::string tmp;

    std::vector<std::string> command;

    while (getline(commandStream, tmp, ' '))
        command.push_back(tmp);

    if (!command.empty()) {
        if (command[0] == "shutdown")
            this->shutdown();
        if (command[0] == "shift") {
            if (command[1] == "up")
                this->shift(Direction::UP);
            if (command[1] == "down")
                this->shift(Direction::DOWN);
            if (command[1] == "left")
                this->shift(Direction::LEFT);
            if (command[1] == "right")
                this->shift(Direction::RIGHT);
        }
        if (command[0] == "kill")
            this->killWindow(this->focusedWindow);
    }
}

/* window manager functions */

void WindowManager::focusWindow(Window window) {
    if (std::find(this->windows.begin(), this->windows.end(), this->focusedWindow) != this->windows.end()) {
        if (this->focusedWindow != 0) {
            XSetWindowBorderWidth(this->display, this->focusedWindow, this->config.borderSize);
            XSetWindowBorder(this->display, this->focusedWindow, this->config.neutralBorderColor);
        }
    }

    if (std::find(this->windows.begin(), this->windows.end(), window) != this->windows.end()) {
        this->focusedWindow = window;

        XRaiseWindow(this->display, this->focusedWindow);
        XSetInputFocus(this->display, this->focusedWindow, RevertToNone, CurrentTime);
        XSetWindowBorderWidth(this->display, this->focusedWindow, this->config.borderSize);
        XSetWindowBorder(this->display, window, this->config.focusedBorderColor);
    }
}

void WindowManager::killWindow(Window window) {
    if (this->isWindowValid(window)) {
        XKillClient(this->display, window);
    }
}

void WindowManager::run() {
    XEvent event;

    for (;;) {
        XNextEvent(this->display, &event);

        switch (event.type) {
            // A window has asked us to be displayed on the screen:
            case MapRequest:
                this->windows.push_back(event.xmaprequest.window);
                XResizeWindow(this->display,
                              event.xmaprequest.window,
                              XDisplayWidth(this->display, 0) - (this->config.borderSize * 2),
                              XDisplayHeight(this->display, 0) - (this->config.borderSize * 2));
                this->focusWindow(event.xmaprequest.window);
                XMapWindow(this->display, event.xmaprequest.window);
                break;
            case UnmapNotify:
                this->windows.erase(std::remove(this->windows.begin(), this->windows.end(), event.xunmap.window), this->windows.end());
                break;
            case DestroyNotify:
                this->windows.erase(std::remove(this->windows.begin(), this->windows.end(), event.xdestroywindow.window), this->windows.end());
                break;

            case KeyPress:
                if (XKeycodeToKeysym(this->display, event.xkey.keycode, 0) == 'w') {
                    this->shift(Direction::UP);
                } else if (XKeycodeToKeysym(this->display, event.xkey.keycode, 0) == 'a') {
                    this->shift(Direction::LEFT);
                } else if (XKeycodeToKeysym(this->display, event.xkey.keycode, 0) == 's') {
                    this->shift(Direction::DOWN);
                } else if (XKeycodeToKeysym(this->display, event.xkey.keycode, 0) == 'd') {
                    this->shift(Direction::RIGHT);
                }

                for (std::pair<std::pair<unsigned int, KeySym>, std::string> keybind : this->config.keyBinds) {
                    if (XKeycodeToKeysym(this->display, event.xkey.keycode, 0) == keybind.first.second) {
                        this->commandDispatcher(keybind.second);
                    }
                }

                /*std::cout << "\033[2J\033[1;1H";
                std::cout << "windows in " << this->gridX << ", " << this->gridY << ":\n";
                for (Window w : this->getCurrentWindows()) {
                    std::cout << "\t" << w << "\n";
                }*/
                break;
            case ButtonPress:
                focusWindow(event.xbutton.subwindow);
                break;
            default:
                break;
        }

        XFlush(this->display);
    }
}

void WindowManager::shift(Direction direction) {
    // Shift the viewport in a direction by the width and height of the entire screen

    for (auto window : this->windows) {
        int displayHeight = XDisplayHeight(this->display, 0);
        int displayWidth = XDisplayWidth(this->display, 0);

        XWindowAttributes windowAttributes;
        XGetWindowAttributes(this->display, window, &windowAttributes);

        switch (direction) {
            case UP:
                XMoveWindow(this->display,
                            window,
                            windowAttributes.x,
                            windowAttributes.y + displayHeight);
                break;
            case DOWN:
                XMoveWindow(this->display,
                            window,
                            windowAttributes.x,
                            windowAttributes.y - displayHeight);
                break;
            case LEFT:
                XMoveWindow(this->display, window,
                            windowAttributes.x + displayWidth,
                            windowAttributes.y);
                break;
            case RIGHT:
                XMoveWindow(this->display,
                            window,
                            windowAttributes.x - displayWidth,
                            windowAttributes.y);
                break;
        }
    }

    // I wish this could be integrated with the above
    switch (direction) {
        case UP:
            this->gridY++;
            break;
        case DOWN:
            this->gridY--;
            break;
        case LEFT:
            this->gridX--;
            break;
        case RIGHT:
            this->gridX++;
            break;
    }

    // now that we have shifted, lets focus a window on this new viewport:
    std::vector<Window> availableWindows = this->getCurrentWindows();
    if (!availableWindows.empty())
    {
        this->focusWindow(availableWindows.front());
    }
}

void WindowManager::shutdown() {
    XCloseDisplay(this->display);
    exit(0);
}

/* constructors */

WindowManager::WindowManager(Config config) {
    this->config = std::move(config);

    XSetErrorHandler(WindowManager::x_error_handler);

    if (!(this->display = XOpenDisplay(nullptr)))
        throw std::runtime_error("failed to connect to X server");

    this->screen = DefaultScreen(this->display);
    this->root = XDefaultRootWindow(this->display);

    XSelectInput(this->display,
                 this->root,
                 SubstructureRedirectMask | SubstructureNotifyMask);

    XSetInputFocus(this->display, this->root, RevertToParent, CurrentTime);

    XSync(this->display, false);

    this->bindButton(Button1, 0);
    this->bindButton(Button3, 0);

    //this->bindKey(XK_w, 0);
    //this->bindKey(XK_a, 0);
    //this->bindKey(XK_s, 0);
    //this->bindKey(XK_d, 0);

    for (std::pair<std::pair<unsigned int, KeySym>, std::string> keybind : this->config.keyBinds) {
        this->bindKey(keybind.first.second, keybind.first.first);
    }

    this->run();
}

WindowManager::~WindowManager() {
    this->shutdown();
}