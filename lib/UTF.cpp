#include "dumbutf/UTF.hpp"
#include "dumbutf/datatypes.hpp"
#include <cstddef>

namespace dumbutf {
unsigned singleCodepointUTF8Length(UTF8 Start) {
  constexpr const char trailingBytesForUTF8[256] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

  return trailingBytesForUTF8[Start] + 1;
}

constexpr UTF32 UTF32_REPLACEMENT_CHARACTER = 0xFFFD;

#define UTF8_CONT(x) (((x) & 0xC0) == 0x80)

int iterate_UTF8_to_UTF32(std::u8string_view Str, UTF32 &CP) {
  if (Str.empty())
    return 0;

  CP = 0xFFFD;
  auto UC = static_cast<UTF32>(static_cast<UTF8>(Str.front()));

  if (UC < 0x0080) {
    CP = UC;
    return 1;
  }

  if ((UTF32)(UC - 0xC2) > (0xF4 - 0xC2))
    return INVALID_UTF8;

  // 2 bytes
  if (UC < 0xE0) {
    if (Str.size() < 2 || !UTF8_CONT(Str[1]))
      return INVALID_UTF8;
    CP = ((UC & 0x1F) << 6) | (Str[1] & 0x3F);
    return 2;
  }

  if (UC < 0xF0) {
    if (Str.size() < 3 || !UTF8_CONT(Str[1]) || !UTF8_CONT(Str[2]))
      return INVALID_UTF8;

    if (UC == 0xED && static_cast<UTF8>(Str[1]) > 0x9F)
      return INVALID_UTF8;

    UC = ((UC & 0xF) << 12) | ((Str[1] & 0x3F) << 6) | (Str[2] & 0x3F);
    if (UC < 0x800)
      return INVALID_UTF8;
    CP = UC;
    return 3;
  }

  // 4 bytes
  if (Str.size() < 4 || !UTF8_CONT(Str[1]) || !UTF8_CONT(Str[2]) ||
      !UTF8_CONT(Str[3]))
    return INVALID_UTF8;

  if (UC == 0xF0)
    if (Str[1] < 0x90)
      return INVALID_UTF8;
    else if (UC == 0xF4)
      if (Str[1] > 0x8F)
        return INVALID_UTF8;

  CP = ((UC & 7) << 18) | ((Str[1] & 0x3F) << 12) | ((Str[2] & 0x3f) << 6) |
       (Str[3] & 0x3F);

  return 4;
}

int iterate_UTF8_to_UTF32_LOSSLY(std::string_view Str, UTF32 &CP) { return 0; }

ptrdiff_t lengthUTF8_to_UTF32(std::string_view Str) { return -1; }
} // namespace dumbutf
