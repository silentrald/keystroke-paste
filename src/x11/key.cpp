/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifdef KP_LINUX_X11

#include "../key.hpp"

#include <X11/Xlib.h>
// #include <X11/extensions/XTest.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

/**
 * Emulates a keypress on an x11 server
 **/
void send_key_event(
    Display* display, Window window, u32 keycode, bool is_pressed
) noexcept {
  XKeyEvent event{};
  event.keycode = keycode;
  event.type = is_pressed ? KeyPress : KeyRelease;
  event.display = display;
  event.window = window;

  XSendEvent(display, window, true, KeyPressMask, (XEvent*)&event);
  XSync(display, false);
  XFlush(display);
}

// void send_key_event_test(
//     Display* display, KeyCode keycode, bool is_pressed
// ) noexcept {
//   XTestFakeKeyEvent(display, keycode, is_pressed, CurrentTime);
//   XSync(display, false);
//   XFlush(display);
// }

void key::input_text(const Data& data, const c8* text) noexcept {
  ul32 window = 0;
  i32 revert = 0;
  XGetInputFocus(data.display, &window, &revert);

  u32 i = 0U;
  KeyCode keycode = 0U;
  for (c8 c = text[i]; c != '\0'; c = text[++i]) {
    keycode = XKeysymToKeycode(data.display, (KeySym)c);
    send_key_event(data.display, window, keycode, true);
    usleep(10'000); // 0.01 second sleep
  }
}

#endif
