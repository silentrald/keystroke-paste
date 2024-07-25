/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

// NOTE: std::string can throw an error
// Refer: https://github.com/exebook/x11clipboard/blob/master/x11paste.c

#ifdef KP_LINUX_X11

#include "./clip.hpp"
#include "./types.hpp"
#include <X11/Xlib.h>
#include <string>

const i32 XA_STRING = 31;
static Atom UTF8;

std::string x_paste_type(Display* display, Window window, Atom atom) noexcept {
  XEvent event{};
  i32 format = 0;
  ul32 N = 0U;
  ul32 size = 0U;
  c8* data = nullptr;
  std::string text{};

  Atom target{};
  Atom CLIPBOARD = XInternAtom(display, "CLIPBOARD", 0);
  Atom XSEL_DATA = XInternAtom(display, "XSEL_DATA", 0);

  XConvertSelection(display, CLIPBOARD, atom, XSEL_DATA, window, CurrentTime);
  XSync(display, 0);
  XNextEvent(display, &event);

  switch (event.type) {
  case SelectionNotify:
    if (event.xselection.selection != CLIPBOARD)
      break;
    if (event.xselection.property) {
      XGetWindowProperty(
          event.xselection.display, event.xselection.requestor,
          event.xselection.property, 0L, (~0L), 0, AnyPropertyType, &target,
          &format, &size, &N, (uc8**)&data
      );
      if (target == UTF8 || target == XA_STRING) {
        text.assign(data, size);
        XFree(data);
      }
      XDeleteProperty(
          event.xselection.display, event.xselection.requestor,
          event.xselection.property
      );
    }
  }
  return std::move(text);
}

std::string XPaste(Display* display, Window window) noexcept {
  std::string text{};
  UTF8 = XInternAtom(display, "UTF8_STRING", True);
  if (UTF8 != None)
    text = x_paste_type(display, window, UTF8);
  if (text.empty())
    text = x_paste_type(display, window, XA_STRING);
  return std::move(text);
}

std::string clip::get_latest_text(const Data& data) noexcept {
  i32 N = DefaultScreen(data.display);
  Window window = XCreateSimpleWindow(
      data.display, RootWindow(data.display, N), 0, 0, 1, 1, 0,
      BlackPixel(data.display, N), WhitePixel(data.display, N)
  );
  return std::move(XPaste(data.display, window));
}

#endif
