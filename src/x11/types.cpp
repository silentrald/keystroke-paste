/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifdef KP_LINUX_X11

#include "../types.hpp"
#include <cstdio>

void print_error(error_code ec) noexcept {
  switch (ec) {
  case error_code::OK:
    break;

  case error_code::DISPLAY_NOT_FOUND:
    printf("X11 display not found\n");
    break;

  case error_code::BAD:
  default:
    printf("Unknown error\n");
    break;
  }
}

#endif
