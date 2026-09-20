#ifndef DUMBUTF_CHARWIDTH_HPP
#define DUMBUTF_CHARWIDTH_HPP

#include "dumbutf/datatypes.hpp"
#include "dumbutf/macros.h"
#include <cstddef>
#include <string_view>

namespace dumbutf {
int codepointCharWidth(UTF32 CP);
bool isAmbiguousWidth(UTF32 CP);

DUMBUTF_ALWAYS_INLINE int codepointCharWidth(char32_t CP) {
  return codepointCharWidth(static_cast<UTF32>(CP));
}

int charWidthUTF32(std::u32string_view Str);
DUMBUTF_ALWAYS_INLINE int charWidthUTF32(const UTF32 *Data, size_t Size) {
  return charWidthUTF32({reinterpret_cast<const char32_t *>(Data), Size});
}

int charWidthUTF8(std::string_view Str);
DUMBUTF_ALWAYS_INLINE int charWidthUTF8(const UTF8 *Data, size_t Size) {
  return charWidthUTF8({reinterpret_cast<const char *>(Data), Size});
}
} // namespace dumbutf

#endif
