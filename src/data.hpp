/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifndef KP_DATA_HPP
#define KP_DATA_HPP

#include "types.hpp"

#if KP_LINUX_X11

#include <X11/Xlib.h>

struct Data { // NOLINT
  Display* display = nullptr;
  ~Data() noexcept;

  error_code init() noexcept;
};

// #elif

#endif

#endif
