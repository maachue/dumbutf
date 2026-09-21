#ifndef DUMBUTF_DATATYPES_HPP
#define DUMBUTF_DATATYPES_HPP

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <span>
#include <type_traits>

namespace dumbutf {
enum ERROR { INVALID_UTF8 = -1, BUFFER_OVERFLOW = -2 };

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

constexpr UTF32 UTF32_REPLACEMENT_CHARACTER = 0xFFFD;

template <typename T>
concept ByteLike =
    sizeof(T) == 1 && (std::same_as<std::remove_cv_t<T>, char> ||
                       std::same_as<std::remove_cv_t<T>, char8_t> ||
                       std::same_as<std::remove_cv_t<T>, uint8_t>);

template <typename T>
concept UTF32WritableChar = sizeof(T) == 4 && !std::is_const_v<T> &&
                            (std::same_as<std::remove_cv_t<T>, uint32_t> ||
                             std::same_as<std::remove_cv_t<T>, char32_t>);

template <typename T>
concept Utf8RangeViewLike = requires(T v) {
  { v.data() } -> std::contiguous_iterator;
  { v.size() } -> std::convertible_to<std::size_t>;
  requires ByteLike<std::remove_reference_t<decltype(*v.data())>>;
};

template <typename T>
concept WritableUTF32ViewLike = requires(T v) {
  { v.data() } -> std::contiguous_iterator;
  { v.size() } -> std::convertible_to<size_t>;
  requires UTF32WritableChar<std::remove_reference_t<decltype(*v.data())>>;
};
} // namespace dumbutf

#endif
