/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#include "./clip.hpp"
#include "./key.hpp"
#include "data.hpp"
#include "types.hpp"
#include <cstdio>
#include <string>
#include <unistd.h>

// TODO: handle loop and try to consume CTRL+V keypress
int main() noexcept {
  Data data{};
  error_code ec = data.init();
  if (ec != error_code::OK) {
    print_error(ec);
    return -1;
  }

  std::string text = clip::get_latest_text(data);
  if (text.empty()) {
    printf("No text from clipboard\n");
    return -1;
  }

  printf("Got text: %s\n", text.c_str());

  usleep(2'000'000); // 2 seconds

  key::input_text(data, text.c_str());

  printf("Done\n");

  return 0;
}
