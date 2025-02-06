/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#include "keyboard.hpp"
#include "types.hpp"
#include <array>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

// TODO: handle loop and try to consume CTRL+V keypress
int main() noexcept {
  kp::Keyboard keyboard{};
  // TODO: return an error code here
  keyboard.init();

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

  // Wait for 3 seconds, for keyboard setup and preparing the user
  //   to pick an active window to do the keystrokes
  usleep(3'000'000);

  keyboard.set_delay(1);
  for (const auto c : output) {
    keyboard.key_press(c);
  }

  printf("Done\n");

  return 0;
}
