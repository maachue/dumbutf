#include "dumbutf/charwidth.hpp"

namespace dumbutf {
int codepointCharWidth(UTF32 CP) {
#include "CharWidth/ZeroWidth.inc"
  if (unicodecharset_contains(ZeroWidthRanges, CP))
    return 0;

#include "CharWidth/TwoWidth.inc"
  if (unicodecharset_contains(TwoWidthRanges, CP))
    return 2;
  return 1;
}

bool isAmbiguousWidth(UTF32 CP) {
#include "CharWidth/AmbiguousWidth.inc"
  return unicodecharset_contains(AmbiguousWidthRanges, CP);
}
} // namespace dumbutf
