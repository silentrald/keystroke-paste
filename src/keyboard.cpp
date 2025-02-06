/*=============================
 * Author/s: silentrald
 * Date Created: 2025-02-06
 *=============================*/

#include "./keyboard.hpp"

namespace kp {

Keyboard::Keyboard(Keyboard&& other) noexcept : fd(other.fd) {
  other.fd = -1;
}

Keyboard& Keyboard::operator=(Keyboard&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  this->fd = rhs.fd;
  rhs.fd = -1;

  return *this;
}

Keyboard::~Keyboard() noexcept {
  if (this->fd == -1) {
    return;
  }

  ioctl(this->fd, UI_DEV_DESTROY);
  close(this->fd);
  this->fd = -1;
}

void Keyboard::init() noexcept {
  this->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (this->fd == -1) {
    return;
  }

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
}

void Keyboard::set_delay(u32 delay) noexcept {
  this->delay = delay;
}

void Keyboard::key_press(u16 key) const noexcept {
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
    usleep(this->delay);
  }

  emit(fd, EV_KEY, key & ~Modifiers::SHIFT, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  usleep(this->delay);

  if (key & Modifiers::SHIFT) {
    emit(fd, EV_KEY, KEY_LEFTSHIFT, 0);
  }
  emit(fd, EV_KEY, key & ~Modifiers::SHIFT, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  usleep(this->delay);
}

void Keyboard::emit(i32 fd, uint16_t type, uint16_t code, i32 value) noexcept {
  input_event ie{.type = type, .code = code, .value = value};
  write(fd, &ie, sizeof(ie));
}

} // namespace kp
