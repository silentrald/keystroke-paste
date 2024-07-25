/*=============================
 * Author/s: silentrald
 * Date Created: 2024-07-25
 *=============================*/

#ifndef KP_CLIPBOARD_HPP
#define KP_CLIPBOARD_HPP

#include "data.hpp"
#include <string>

namespace clip {

std::string get_latest_text(const Data& data) noexcept;

} // namespace clip

#endif
