#pragma once

#include <cctype>
#include <string>

namespace cse7761_core {

inline bool extract_calibration_payload(const std::string &text, std::string &payload) {
  static const char *const keys[] = {"calib_coefficient0", "calib_coefficient"};

  for (const char *key : keys) {
    const std::string marker(key);
    size_t position = text.find(marker);
    if (position == std::string::npos) {
      continue;
    }

    position += marker.size();
    // Skip the closing quote of a JSON key, if present.
    if (position < text.size() && text[position] == '"') {
      position++;
    }
    while (position < text.size() &&
           (std::isspace(static_cast<unsigned char>(text[position])) ||
            text[position] == ':' || text[position] == '=')) {
      position++;
    }

    if (position >= text.size()) {
      return false;
    }

    if (text[position] == '"') {
      position++;
      const size_t end = text.find('"', position);
      if (end == std::string::npos) {
        return false;
      }
      payload = text.substr(position, end - position);
    } else {
      const size_t start = text.find('[', position);
      const size_t end = text.rfind(']');
      if (start == std::string::npos || end == std::string::npos || end < start) {
        return false;
      }
      payload = text.substr(start, end - start + 1);
    }
    return !payload.empty();
  }

  // Direct NVS string: the whole value is the calibration payload.
  if (text.find('[') != std::string::npos && text.find(']') != std::string::npos) {
    payload = text;
    return true;
  }
  return false;
}

}  // namespace cse7761_core
