/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifndef KP_TYPES_HPP
#define KP_TYPES_HPP

#include <cstdint>

using i32 = int32_t;
using u32 = uint32_t;
using ul32 = unsigned long;
using c8 = char;
using uc8 = unsigned char;

enum error_code : u32 {
  OK = 0U,

#ifdef KP_LINUX_X11
  DISPLAY_NOT_FOUND = 1U,
#endif

  BAD = UINT32_MAX
};

void print_error(error_code ec) noexcept;

#endif
