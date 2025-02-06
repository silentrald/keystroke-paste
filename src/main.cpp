/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#include "data.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

namespace Modifiers {
enum Modifiers : u16 {
  SHIFT = 0x8000,
  CTRL = 0x4000,
  ALT = 0x2000,
};
} // namespace Modifiers

// NOTE: Can be remapped in the future
std::array<uint16_t, 127> key_map{
    // Special key codes
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    KEY_TAB,   // \t
    KEY_ENTER, // \n
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    //

    KEY_SPACE,
    KEY_1 | Modifiers::SHIFT,          // !
    KEY_APOSTROPHE | Modifiers::SHIFT, // "
    KEY_3 | Modifiers::SHIFT,          // #
    KEY_4 | Modifiers::SHIFT,          // $
    KEY_5 | Modifiers::SHIFT,          // %
    KEY_7 | Modifiers::SHIFT,          // &
    KEY_APOSTROPHE,
    KEY_9 | Modifiers::SHIFT,     // (
    KEY_0 | Modifiers::SHIFT,     // )
    KEY_8 | Modifiers::SHIFT,     // *
    KEY_EQUAL | Modifiers::SHIFT, // +
    KEY_COMMA, KEY_MINUS, KEY_DOT, KEY_SLASH,

    // Numbers
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    //

    KEY_SEMICOLON | Modifiers::SHIFT, // :
    KEY_SEMICOLON,
    KEY_COMMA | Modifiers::SHIFT, // <
    KEY_EQUAL,
    KEY_DOT | Modifiers::SHIFT,   // >
    KEY_SLASH | Modifiers::SHIFT, // ?
    KEY_2 | Modifiers::SHIFT,     // @

    // Caps characters
    KEY_A | Modifiers::SHIFT, KEY_B | Modifiers::SHIFT,
    KEY_C | Modifiers::SHIFT, KEY_D | Modifiers::SHIFT,
    KEY_E | Modifiers::SHIFT, KEY_F | Modifiers::SHIFT,
    KEY_G | Modifiers::SHIFT, KEY_H | Modifiers::SHIFT,
    KEY_I | Modifiers::SHIFT, KEY_J | Modifiers::SHIFT,
    KEY_K | Modifiers::SHIFT, KEY_L | Modifiers::SHIFT,
    KEY_M | Modifiers::SHIFT, KEY_N | Modifiers::SHIFT,
    KEY_O | Modifiers::SHIFT, KEY_P | Modifiers::SHIFT,
    KEY_Q | Modifiers::SHIFT, KEY_R | Modifiers::SHIFT,
    KEY_S | Modifiers::SHIFT, KEY_T | Modifiers::SHIFT,
    KEY_U | Modifiers::SHIFT, KEY_V | Modifiers::SHIFT,
    KEY_W | Modifiers::SHIFT, KEY_X | Modifiers::SHIFT,
    KEY_Y | Modifiers::SHIFT, KEY_Z | Modifiers::SHIFT,
    //

    KEY_LEFTBRACE, KEY_BACKSLASH, KEY_RIGHTBRACE,
    KEY_6 | Modifiers::SHIFT,     // ^
    KEY_MINUS | Modifiers::SHIFT, // _
    KEY_GRAVE,

    // Normal characters
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K,
    KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V,
    KEY_W, KEY_X, KEY_Y, KEY_Z,
    //

    KEY_LEFTBRACE | Modifiers::SHIFT,  // {
    KEY_BACKSLASH | Modifiers::SHIFT,  // |
    KEY_RIGHTBRACE | Modifiers::SHIFT, // }
    KEY_GRAVE | Modifiers::SHIFT,      // ~
};

// Uinput test
void emit(i32 fd, uint16_t type, uint16_t code, i32 value) {
  input_event ie{.type = type, .code = code, .value = value};
  write(fd, &ie, sizeof(ie));
}

const u32 DELAY = 1'000;
void key_press(int fd, uint16_t key) noexcept {
  if (key >= key_map.size()) {
    return;
  }
  key = key_map[key];
  if (key == 0) {
    return;
  }

  if (key & Modifiers::SHIFT) {
    emit(fd, EV_KEY, KEY_LEFTSHIFT, 1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
    usleep(DELAY);
  }

  emit(fd, EV_KEY, key & ~Modifiers::SHIFT, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  usleep(DELAY);

  if (key & Modifiers::SHIFT) {
    emit(fd, EV_KEY, KEY_LEFTSHIFT, 0);
  }
  emit(fd, EV_KEY, key & ~Modifiers::SHIFT, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  usleep(DELAY);
}

void test(const c8* text) noexcept {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  uinput_user_dev setup{
      .id =
          {
              .bustype = BUS_USB,
              .vendor = 0x1111,
              .product = 0x1111,
              .version = 1,
          }
  };
  std::strcpy(setup.name, "Simulated keyboard");

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  for (int key = KEY_ESC; key <= KEY_KPDOT; ++key) {
    ioctl(fd, UI_SET_KEYBIT, key);
  }

  write(fd, &setup, sizeof(setup));
  ioctl(fd, UI_DEV_CREATE);

  usleep(1'000'000);

  int size = std::strlen(text);
  for (int i = 0; i < size; ++i) {
    key_press(fd, text[i]);
  }

  ioctl(fd, UI_DEV_DESTROY);
  close(fd);
}

// TODO: handle loop and try to consume CTRL+V keypress
int main() noexcept {
  FILE* pipe = popen("wl-paste", "r");
  if (pipe == nullptr) {
    return 1;
  }

  std::array<c8, 256> buffer{};
  std::string output{};
  try {
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
      output.append(buffer.data());
    }
  } catch (...) {
    pclose(pipe);
    return 1;
  }

  usleep(2'000'000); // 2 seconds

  test(output.c_str());

  printf("Done\n");

  return 0;
}
