#ifndef DUMBUTF_DATATYPES_HPP
#define DUMBUTF_DATATYPES_HPP

#include <algorithm>
#include <cstdint>
#include <span>

namespace dumbutf {
enum ERROR { INVALID_UTF8 = -1 };

using UTF8 = std::uint8_t;
using UTF16 = std::uint16_t;
using UTF32 = std::uint32_t;

struct CPRange {
  uint32_t Start;
  uint32_t End;
};

inline bool operator<(UTF32 V, CPRange R) { return V < R.Start; }
inline bool operator<(CPRange R, UTF32 V) { return R.End < V; }

[[nodiscard]] inline bool unicodecharset_contains(std::span<const CPRange> RR,
                                                  UTF32 V) {
  // NOLINTNEXTLINE(modernize-use-ranges)
  return std::binary_search(RR.begin(), RR.end(), V);
  // Ranges version of binary search needed operator==, Idk why.
}
} // namespace dumbutf

#endif
