#include <iostream>

#include "WindowManager.hpp"
#include "Config.hpp"

// todo list:
// - gaps
// - basic window manager stuff (killing windows, moving them)
// - grid hinter (hardest feature yet)
// - sloppy focus
// - unmap windows that are out of focus (save ram maybe?)

// done list:
// - command dispatcher (needs work)
// - borders
// - automatic keybind setup
// - infinity grid like movement
// - window manager foundation

// issues:
// - when switching to an empty grid space after focusing a window
//   on a previous grid space, the window will remain focused.

int main() {
    Config config = Config();
    WindowManager wm = WindowManager(config);

    return 0;
}
