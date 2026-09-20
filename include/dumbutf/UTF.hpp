#ifndef DUMBUTF_UTF_HPP
#define DUMBUTF_UTF_HPP

#include "dumbutf/datatypes.hpp"
#include "dumbutf/macros.h"
#include <cstddef>
#include <string_view>

namespace dumbutf {
unsigned singleCodepointUTF8Length(UTF8 Start);
DUMBUTF_ALWAYS_INLINE unsigned singleCodepointUTF8Length(char Start) {
  return singleCodepointUTF8Length(static_cast<UTF8>(Start));
}

int iterate_UTF8_to_UTF32(std::u8string_view Str, UTF32 &CP);
DUMBUTF_ALWAYS_INLINE int iterate_UTF8_to_UTF32(UTF8 const *Data, size_t Size,
                                                UTF32 &CP) {
  return iterate_UTF8_to_UTF32({reinterpret_cast<const char8_t *>(Data), Size},
                               CP);
}

DUMBUTF_ALWAYS_INLINE int iterate_UTF8_to_UTF32(std::string_view Str,
                                                UTF32 &CP) {
  return iterate_UTF8_to_UTF32(
      {reinterpret_cast<const char8_t *>(Str.data()), Str.size()}, CP);
}

int iterate_UTF8_to_UTF32_LOSSLY(std::string_view Str, UTF32 &CP);
DUMBUTF_ALWAYS_INLINE int iterate_UTF8_to_UTF32_LOSSLY(UTF8 const *Data,
                                                       size_t Size, UTF32 &CP) {
  return iterate_UTF8_to_UTF32_LOSSLY(
      {reinterpret_cast<const char *>(Data), Size}, CP);
}

ptrdiff_t lengthUTF8_to_UTF32(std::string_view Str);
DUMBUTF_ALWAYS_INLINE ptrdiff_t lengthUTF8_to_UTF32(const UTF8 *Data,
                                                    size_t S) {
  return lengthUTF8_to_UTF32({reinterpret_cast<const char *>(Data), S});
}
} // namespace dumbutf

#endif
