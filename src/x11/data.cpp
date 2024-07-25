/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifdef KP_LINUX_X11

#include "../data.hpp"
#include "types.hpp"
#include <X11/Xlib.h>

error_code Data::init() noexcept {
  this->display = XOpenDisplay(nullptr);
  return this->display == nullptr ? error_code::DISPLAY_NOT_FOUND
                                  : error_code::OK;
}

Data::~Data() noexcept {
  if (this->display) {
    XCloseDisplay(this->display);
    this->display = nullptr;
  }
}

#endif
