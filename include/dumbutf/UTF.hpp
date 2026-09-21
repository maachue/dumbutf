#ifndef DUMBUTF_UTF_HPP
#define DUMBUTF_UTF_HPP

#include "dumbutf/datatypes.hpp"
#include "dumbutf/macros.h"
#include <cstddef>

namespace dumbutf {
unsigned singleCodepointUTF8Length(UTF8 Start);
template <ByteLike T>
DUMBUTF_ALWAYS_INLINE unsigned singleCodepointUTF8Length(T Start) {
  return singleCodepointUTF8Length(static_cast<UTF8>(Start));
}

/// Reads a single UTF-8 codepoint from a byte buffer.
///
/// Derived from `utf8proc` (`utf8proc_iterate`), licensed under the MIT "Expat"
/// License.
///
/// \param[in]  Str Pointer to the UTF-8 encoded byte array.
/// \param[in]  Len Maximum number of bytes to read from \p Str.
/// \param[out] CP  Output reference where the decoded UTF-32 codepoint will be
/// stored.
///                 Set to `U+FFFD` (Replacement Character) if decoding fails.
///
/// \return On success, returns the number of bytes read (positive integer).
///         On failure, returns a negative error code.
int iterate_UTF8_to_UTF32(const UTF8 *Str, size_t Len, UTF32 &CP);

/// Reads a single UTF-8 codepoint from a range/view-like object.
///
/// \tparam T Type conforming to `Utf8RangeViewLike`.
/// \tparam F Type conforming to `UTF32WritableChar`.
///
/// \param[in]  UTF8String UTF-8 string view or container.
/// \param[out] CP         Output reference to store the decoded UTF-32
/// codepoint.
///
/// \return Number of bytes read on success, or a negative error code on
/// failure.
template <Utf8RangeViewLike T, UTF32WritableChar F>
DUMBUTF_ALWAYS_INLINE int iterate_UTF8_to_UTF32(const T &UTF8String, F &CP) {
  return iterate_UTF8_to_UTF32(
      reinterpret_cast<const UTF8 *>(UTF8String.data()),
      static_cast<size_t>(UTF8String.size()), reinterpret_cast<UTF32 &>(CP));
}

/// Reads a single UTF-8 codepoint lossly from a byte buffer.
///
/// Invalid UTF-8 sequences are replaced with `U+FFFD` without returning an
/// error.
///
/// \param[in]  Str Pointer to the UTF-8 encoded byte array.
/// \param[in]  Len Maximum number of bytes to read from \p Str.
/// \param[out] CP  Output reference where the decoded UTF-32 codepoint (or
/// `U+FFFD`) is stored.
///
/// \return The number of bytes consumed from the input buffer (always > 0).
size_t iterate_UTF8_to_UTF32_LOSSLY(const UTF8 *Str, size_t Len, UTF32 &CP);

/// Reads a single UTF-8 codepoint lossly from a range/view-like object.
///
/// \tparam T Type conforming to `Utf8RangeViewLike`.
/// \tparam F Type conforming to `UTF32WritableChar`.
///
/// \param[in]  UTF8String UTF-8 string view or container.
/// \param[out] CP         Output reference where the decoded UTF-32 codepoint
/// is stored.
///
/// \return The number of bytes consumed from the input string.
template <Utf8RangeViewLike T, UTF32WritableChar F>
DUMBUTF_ALWAYS_INLINE size_t iterate_UTF8_to_UTF32_LOSSLY(const T &UTF8String,
                                                          F &CP) {
  return iterate_UTF8_to_UTF32_LOSSLY(
      reinterpret_cast<const UTF8 *>(UTF8String.data()),
      static_cast<size_t>(UTF8String.size()), reinterpret_cast<UTF32 &>(CP));
}

/// Determines the required UTF-32 buffer length (in codepoints) for lossy
/// conversion.
///
/// \param[in] Data Pointer to the UTF-8 byte array.
/// \param[in] Len  Length of the UTF-8 buffer in bytes.
///
/// \return The number of UTF-32 elements (`UTF32`) required to store the
/// converted string.
size_t length_UTF8_to_UTF32_LOSSLY(const UTF8 *Data, size_t Len);

/// Determines the required UTF-32 buffer length (in codepoints) for lossy
/// conversion.
///
/// \tparam T Type conforming to `Utf8RangeViewLike`.
///
/// \param[in] View UTF-8 string view or container.
///
/// \return The number of UTF-32 elements (`UTF32`) required to store the
/// converted string.
template <Utf8RangeViewLike T>
DUMBUTF_ALWAYS_INLINE size_t length_UTF8_to_UTF32_LOSSLY(const T &View) {
  return length_UTF8_to_UTF32_LOSSLY(
      reinterpret_cast<const UTF8 *>(View.data()),
      static_cast<size_t>(View.size()));
}

/// Converts a UTF-8 string to a UTF-32 string lossly.
///
/// Invalid byte sequences are replaced with `U+FFFD`.
///
/// \param[in]  UTF8Str     Pointer to the source UTF-8 byte buffer.
/// \param[in]  UTF8StrLen  Length of the source buffer in bytes.
/// \param[out] UTF32Str    Pointer to the destination UTF-32 buffer.
/// \param[in]  UTF32StrLen Capacity of the destination buffer in UTF-32
/// elements.
///
/// \return The number of converted UTF-32 codepoints written to \p UTF32Str on
/// success,
///         or a negative error code (e.g., buffer overflow/insufficient
///         capacity).
ptrdiff_t convert_UTF8_to_UTF32_LOSSLY(const UTF8 *UTF8Str, size_t UTF8StrLen,
                                       UTF32 *UTF32Str, size_t UTF32StrLen);

/// Converts a UTF-8 string buffer to a UTF-32 buffer lossly.
///
/// \tparam T Type conforming to `ByteLike`.
/// \tparam F Type conforming to `UTF32WritableChar`.
///
/// \param[in]  UTF8Str     Pointer to the source byte buffer.
/// \param[in]  UTF8StrLen  Length of the source buffer in bytes.
/// \param[out] UTF32String Pointer to the destination UTF-32 buffer.
/// \param[in]  UTF32StrLen Capacity of the destination buffer in UTF-32
/// elements.
///
/// \return Number of written UTF-32 codepoints, or a negative error code on
/// failure.
template <ByteLike T, UTF32WritableChar F>
ptrdiff_t convert_UTF8_to_UTF32_LOSSLY(const T *UTF8Str, size_t UTF8StrLen,
                                       F *UTF32String, size_t UTF32StrLen) {
  return convert_UTF8_to_UTF32_LOSSLY(
      reinterpret_cast<const UTF8 *>(UTF8Str), UTF8StrLen,
      reinterpret_cast<UTF32 *>(UTF32String), UTF32StrLen);
}

/// Converts a UTF-8 view to a UTF-32 array lossly.
///
/// \tparam T Type conforming to `Utf8RangeViewLike`.
/// \tparam F Type conforming to `UTF32WritableChar`.
///
/// \param[in]  UTF8Str      Source UTF-8 range or view.
/// \param[out] UTF32Str     Pointer to the destination UTF-32 array.
/// \param[in]  UTF32StrSize Capacity of the destination array in UTF-32
/// elements.
///
/// \return Number of written UTF-32 codepoints, or a negative error code on
/// failure.
template <Utf8RangeViewLike T, UTF32WritableChar F>
ptrdiff_t convert_UTF8_to_UTF32_LOSSLY(const T &UTF8Str, F *UTF32Str,
                                       size_t UTF32StrSize) {
  return convert_UTF8_to_UTF32_LOSSLY(
      reinterpret_cast<const UTF8 *>(UTF8Str.data()),
      static_cast<size_t>(UTF8Str.size()), reinterpret_cast<UTF32 *>(UTF32Str),
      UTF32StrSize);
}

/// Converts a UTF-8 view to a writable UTF-32 view lossly.
///
/// \tparam T Type conforming to `Utf8RangeViewLike`.
/// \tparam F Type conforming to `WritableUTF32ViewLike`.
///
/// \param[in]  UTF8Str  Source UTF-8 range or view.
/// \param[out] UTF32Str Destination writable UTF-32 range or view.
///
/// \return Number of written UTF-32 codepoints, or a negative error code on
/// failure.
template <Utf8RangeViewLike T, WritableUTF32ViewLike F>
ptrdiff_t convert_UTF8_to_UTF32_LOSSLY(const T &UTF8Str, F &UTF32Str) {
  return convert_UTF8_to_UTF32_LOSSLY(
      reinterpret_cast<const UTF8 *>(UTF8Str.data()),
      static_cast<size_t>(UTF8Str.size()),
      reinterpret_cast<UTF32 *>(UTF32Str.data()),
      static_cast<size_t>(UTF32Str.size()));
}
} // namespace dumbutf

#endif
