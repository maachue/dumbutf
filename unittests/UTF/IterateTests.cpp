//===========================================================================//
//
// Unit tests for: Iterate converation between UTF.
//
//===========================================================================//

#include "dumbutf/UTF.hpp"

#include "gtest/gtest.h"

#include "Msg.h"
#include "dumbutf/datatypes.hpp"

#include <ostream>
#include <string_view>

using namespace dumbutf;

namespace {
struct Iterate_UTF8_to_UTF32_Case {
  std::string_view UTF8Input;
  UTF32 UTF32Expected;
  ptrdiff_t ExpectedUTF8Length;
  const char *Desc = nullptr;

  friend std::ostream &operator<<(std::ostream &OS,
                                  Iterate_UTF8_to_UTF32_Case const &V) {
    OS << "UTF-8 Input: '" << V.UTF8Input << "'; UTF-32 Expected: '"
       << CPPrint(V.UTF32Expected) << "'";
    if (V.Desc)
      OS << "; Desc: " << V.Desc;

    return OS;
  }
};

} // namespace

//------------------------------------------------------------------------------
//
// 0. Tests for iterate_UTF8_to_UTF32 (lossless)
//
//------------------------------------------------------------------------------

struct Iterate_UTF8_to_UTF32_Test
    : public ::testing::TestWithParam<Iterate_UTF8_to_UTF32_Case> {};

TEST_P(Iterate_UTF8_to_UTF32_Test, Convert) {
  auto const &P = GetParam();
  UTF32 CP;

  EXPECT_EQ(iterate_UTF8_to_UTF32(P.UTF8Input, CP), P.ExpectedUTF8Length);
  EXPECT_EQ(CP, P.UTF32Expected);
}

static char StrWithNull[] = "\0";

INSTANTIATE_TEST_SUITE_P(
    OneByte, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"A", 'A', 1, "ASCII 'A'"},
        Iterate_UTF8_to_UTF32_Case{"b", 'b', 1, "ASCII 'b'"},
        Iterate_UTF8_to_UTF32_Case{"\x1b[31;1m", 0x1B, 1,
                                   "ASCII control character 'ESC'"},
        Iterate_UTF8_to_UTF32_Case{"\r\n", 0x000D, 1,
                                   "ASCII control character 'CR'"},
        // Iterate_UTF8_to_UTF32_Case{"\xE9", 0xFFFD, INVALID_UTF8,
        //                            "Leading 3 bytes, but ... nothing"},
        // Iterate_UTF8_to_UTF32_Case{"\xCF", 0xFFFD, INVALID_UTF8,
        //                            "Leading 2 bytes, but ... nothing"},
        Iterate_UTF8_to_UTF32_Case{{StrWithNull, 1}, 0x00, 1, "ASCII 'NULL'"},
        Iterate_UTF8_to_UTF32_Case{"@@fsf", '@', 1, "ASCII '@'"}));

INSTANTIATE_TEST_SUITE_P(
    TwoBytes, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(
        // Iterate_UTF8_to_UTF32_Case{"\xE9\x9B", 0xFFFD, 1,
        //                            "3 bytes, but only 2 provided"},
        Iterate_UTF8_to_UTF32_Case{"\xCF\x80", 0x03C0 /* pi */, 2, "PI"},
        Iterate_UTF8_to_UTF32_Case{"\xC2\xA9", 0x00A9, 2, "U+00A9 (©)"},
        Iterate_UTF8_to_UTF32_Case{"\xC3\xA9\x0A", 0x00E9, 2,
                                   "Latin small letter E with Acute U+00E9"},
        Iterate_UTF8_to_UTF32_Case{"\xCE\xB1%%$", 0x03B1, 2, "Alpha"},
        Iterate_UTF8_to_UTF32_Case{"\xD8\xA2$", 0x0622, 2, "Aleph"}));

INSTANTIATE_TEST_SUITE_P(
    OverlongAndSurrogate, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(Iterate_UTF8_to_UTF32_Case{"\xC0\xAF", 0xFFFD,
                                                 INVALID_UTF8,
                                                 "ASCII overlong '/'"},
                      Iterate_UTF8_to_UTF32_Case{"\xED\xA0\x80", 0xFFFD,
                                                 INVALID_UTF8,
                                                 "Surrogate U+D800"}));

INSTANTIATE_TEST_SUITE_P(
    ThreeBytes, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"\xE9\x9B\x80", 0x96C0, 3, "CJK U+96C0"},
        Iterate_UTF8_to_UTF32_Case{"\xE1\xBA\xA0", 0x1EA0, 3, "Latin-like: Ạ"},
        Iterate_UTF8_to_UTF32_Case{"\xE2\x82\xAC\x56\x50", 0x20AC, 3, "Euro"},
        Iterate_UTF8_to_UTF32_Case{"\xE2\x99\xA5@", 0x2665, 3,
                                   "Black Heart Suit"},
        Iterate_UTF8_to_UTF32_Case{"\xE4\xB8\xAD\xA", 0x4E2D, 3, "CJK U+4E2D"},
        Iterate_UTF8_to_UTF32_Case{"\xE3\x81\x82すず", 0x3042, 3,
                                   "CJK U+3042 Hiragana 'あ'"}));

INSTANTIATE_TEST_SUITE_P(
    FourBytes, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(Iterate_UTF8_to_UTF32_Case{"\xF0\x9F\x98\x80", 0x1F600, 4,
                                                 "Emoji Grinning Face"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x9F\x8E\x89", 0x1F389, 4,
                                                 "Emoji Part Popper"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x90\x8C\xB0\xA", 0x10330,
                                                 4, "Gothic Ahsa"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\xA0\x80\x80\xF0",
                                                 0x20000, 4,
                                                 "CJK (Ext B) Idk U+20000"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x9D\x84\x9E\x9D",
                                                 0x1D11E, 4,
                                                 "Musical Symbol G Clef"}));

INSTANTIATE_TEST_SUITE_P(
    Invalid, Iterate_UTF8_to_UTF32_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"\xE9", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xE9\x9B", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xE1", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xE1\x9B", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xCE", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xCE\xA", 0xFFFD, INVALID_UTF8, ""},
        Iterate_UTF8_to_UTF32_Case{"\xF0\x9B", 0xFFFD, INVALID_UTF8, ""}));

// NOTE:  Code tests below generated by Claude Sonnet 5 by prompting.

//------------------------------------------------------------------------------
//
// 1. Tests for iterate_UTF8_to_UTF32 _LOSSLY(lossly)
//
//------------------------------------------------------------------------------
//
// iterate_UTF8_to_UTF32_LOSSLY() never fails: it always returns a byte
// count > 0 (when Len > 0) and sets CP to either the decoded codepoint or
// U+FFFD (REPLACEMENT CHARACTER) on error. The error-recovery byte counts
// below follow the WHATWG Encoding Standard's UTF-8 decoder algorithm
// (https://encoding.spec.whatwg.org/#utf-8-decoder):
//
//   * A byte that can never start or continue a sequence (0x80-0xC1,
//     0xF5-0xFF, or a stray continuation byte 0x80-0xBF seen when no
//     sequence is in progress) is rejected immediately: 1 byte consumed.
//
//   * Once a leading byte starts a multi-byte sequence, if a subsequent
//     continuation byte is out of its expected range (this includes the
//     narrowed ranges used to reject overlong encodings for 0xE0/0xF0,
//     surrogate halves for 0xED, and code points above U+10FFFF for
//     0xF4), that offending byte is NOT consumed - it is "prepended"
//     back so it can start a fresh decode on the next call. Only the
//     bytes that were already validated as part of the failed sequence
//     are consumed.
//
//   * If the buffer simply runs out of bytes while a sequence is still
//     incomplete, every byte that was read (and validated so far) is
//     consumed, since there is nothing left to prepend.
//
//------------------------------------------------------------------------------

struct Iterate_UTF8_to_UTF32_LOSSLY_Test
    : public ::testing::TestWithParam<Iterate_UTF8_to_UTF32_Case> {};

TEST_P(Iterate_UTF8_to_UTF32_LOSSLY_Test, Convert) {
  auto const &P = GetParam();
  UTF32 CP;

  EXPECT_EQ(iterate_UTF8_to_UTF32_LOSSLY(P.UTF8Input, CP),
            P.ExpectedUTF8Length);
  EXPECT_EQ(CP, P.UTF32Expected);
}

// --- 1 byte: valid ASCII (same well-formed inputs as the lossless suite) ---
INSTANTIATE_TEST_SUITE_P(
    OneByte, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"A", 'A', 1, "ASCII 'A'"},
        Iterate_UTF8_to_UTF32_Case{"b", 'b', 1, "ASCII 'b'"},
        Iterate_UTF8_to_UTF32_Case{"\x1b[31;1m", 0x1B, 1,
                                   "ASCII control character 'ESC'"},
        Iterate_UTF8_to_UTF32_Case{"\r\n", 0x000D, 1,
                                   "ASCII control character 'CR'"},
        Iterate_UTF8_to_UTF32_Case{{StrWithNull, 1}, 0x00, 1, "ASCII 'NULL'"},
        Iterate_UTF8_to_UTF32_Case{"@@fsf", '@', 1, "ASCII '@'"}));

// --- 1 byte: invalid - bytes that can never start or continue a sequence.
// These are rejected immediately, consuming exactly 1 byte each. ---
INSTANTIATE_TEST_SUITE_P(
    OneByte_Invalid, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{
            "\x80", 0xFFFD, 1,
            "Stray continuation byte (no sequence in progress)"},
        Iterate_UTF8_to_UTF32_Case{
            "\xBF", 0xFFFD, 1,
            "Stray continuation byte (no sequence in progress)"},
        Iterate_UTF8_to_UTF32_Case{
            "\xC0", 0xFFFD, 1,
            "0xC0 can only ever encode an overlong sequence"},
        Iterate_UTF8_to_UTF32_Case{
            "\xC1", 0xFFFD, 1,
            "0xC1 can only ever encode an overlong sequence"},
        Iterate_UTF8_to_UTF32_Case{"\xF5", 0xFFFD, 1,
                                   "0xF5 would decode beyond U+10FFFF"},
        Iterate_UTF8_to_UTF32_Case{"\xFF", 0xFFFD, 1,
                                   "0xFF is never a valid UTF-8 byte"}));

// --- 2 bytes: valid (same well-formed inputs as the lossless suite) ---
INSTANTIATE_TEST_SUITE_P(
    TwoBytes, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"\xCF\x80", 0x03C0 /* pi */, 2, "PI"},
        Iterate_UTF8_to_UTF32_Case{"\xC2\xA9", 0x00A9, 2, "U+00A9 (©)"},
        Iterate_UTF8_to_UTF32_Case{"\xC3\xA9\x0A", 0x00E9, 2,
                                   "Latin small letter E with Acute U+00E9"},
        Iterate_UTF8_to_UTF32_Case{"\xCE\xB1%%$", 0x03B1, 2, "Alpha"},
        Iterate_UTF8_to_UTF32_Case{"\xD8\xA2$", 0x0622, 2, "Aleph"}));

// --- 2 bytes: invalid ---
INSTANTIATE_TEST_SUITE_P(
    TwoBytes_Invalid, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{
            "\xC2", 0xFFFD, 1,
            "Truncated: leading byte only, buffer ends before continuation"},
        Iterate_UTF8_to_UTF32_Case{
            "\xC2\x41", 0xFFFD, 1,
            "Continuation byte is plain ASCII 'A': not consumed (prepended), "
            "only leading byte counted"}));

// --- 3 bytes: valid (same well-formed inputs as the lossless suite) ---
INSTANTIATE_TEST_SUITE_P(
    ThreeBytes, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{"\xE9\x9B\x80", 0x96C0, 3, "CJK U+96C0"},
        Iterate_UTF8_to_UTF32_Case{"\xE1\xBA\xA0", 0x1EA0, 3, "Latin-like: Ạ"},
        Iterate_UTF8_to_UTF32_Case{"\xE2\x82\xAC\x56\x50", 0x20AC, 3, "Euro"},
        Iterate_UTF8_to_UTF32_Case{"\xE2\x99\xA5@", 0x2665, 3,
                                   "Black Heart Suit"},
        Iterate_UTF8_to_UTF32_Case{"\xE4\xB8\xAD\xA", 0x4E2D, 3, "CJK U+4E2D"},
        Iterate_UTF8_to_UTF32_Case{"\xE3\x81\x82すず", 0x3042, 3,
                                   "CJK U+3042 Hiragana 'あ'"}));

// --- 3 bytes: invalid (truncated at various points, bad continuation at
// various positions) ---
INSTANTIATE_TEST_SUITE_P(
    ThreeBytes_Invalid, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{
            "\xE9", 0xFFFD, 1,
            "Truncated: leading byte only, buffer ends immediately"},
        Iterate_UTF8_to_UTF32_Case{
            "\xE9\x9B", 0xFFFD, 2,
            "Truncated: leading + 1 valid continuation, buffer ends before "
            "the final byte; both already-read bytes are consumed"},
        Iterate_UTF8_to_UTF32_Case{
            "\xE1\x41", 0xFFFD, 1,
            "First continuation byte is plain ASCII 'A': not consumed "
            "(prepended), only leading byte counted"},
        Iterate_UTF8_to_UTF32_Case{
            "\xE1\xBA\x41", 0xFFFD, 2,
            "Second continuation byte is plain ASCII 'A': not consumed "
            "(prepended); leading byte + first (valid) continuation are "
            "counted"}));

// --- Overlong encodings and surrogate halves: rejected via the narrowed
// boundary check on the FIRST continuation byte, so only the leading byte
// is consumed (the offending byte is prepended for the next decode). ---
INSTANTIATE_TEST_SUITE_P(
    OverlongAndSurrogate, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{
            "\xC0\xAF", 0xFFFD, 1,
            "ASCII overlong '/' (0xC0 is always invalid as a leading byte)"},
        Iterate_UTF8_to_UTF32_Case{
            "\xE0\x9F\x80", 0xFFFD, 1,
            "Overlong 3-byte: 0xE0's continuation must be >= 0xA0"},
        Iterate_UTF8_to_UTF32_Case{"\xE0\x80\x80", 0xFFFD, 1,
                                   "Overlong 3-byte encoding of U+0000"},
        Iterate_UTF8_to_UTF32_Case{
            "\xED\xA0\x80", 0xFFFD, 1,
            "Surrogate U+D800: 0xED's continuation must be <= 0x9F"},
        Iterate_UTF8_to_UTF32_Case{
            "\xED\xBF\xBF", 0xFFFD, 1,
            "Surrogate U+DFFF: 0xED's continuation must be <= 0x9F"},
        Iterate_UTF8_to_UTF32_Case{
            "\xF0\x80\x80\x80", 0xFFFD, 1,
            "Overlong 4-byte: 0xF0's continuation must be >= 0x90"},
        Iterate_UTF8_to_UTF32_Case{
            "\xF4\x90\x80\x80", 0xFFFD, 1,
            "Beyond U+10FFFF: 0xF4's continuation must be <= 0x8F"}));

// --- 4 bytes: valid (same well-formed inputs as the lossless suite) ---
INSTANTIATE_TEST_SUITE_P(
    FourBytes, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(Iterate_UTF8_to_UTF32_Case{"\xF0\x9F\x98\x80", 0x1F600, 4,
                                                 "Emoji Grinning Face"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x9F\x8E\x89", 0x1F389, 4,
                                                 "Emoji Part Popper"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x90\x8C\xB0\xA", 0x10330,
                                                 4, "Gothic Ahsa"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\xA0\x80\x80\xF0",
                                                 0x20000, 4,
                                                 "CJK (Ext B) Idk U+20000"},
                      Iterate_UTF8_to_UTF32_Case{"\xF0\x9D\x84\x9E\x9D",
                                                 0x1D11E, 4,
                                                 "Musical Symbol G Clef"}));

// --- 4 bytes: invalid (truncated at various points, bad continuation at the
// final position) ---
INSTANTIATE_TEST_SUITE_P(
    FourBytes_Invalid, Iterate_UTF8_to_UTF32_LOSSLY_Test,
    ::testing::Values(
        Iterate_UTF8_to_UTF32_Case{
            "\xF0\x9B", 0xFFFD, 2,
            "Truncated: leading + 1 valid continuation, buffer ends before "
            "the remaining 2 bytes; both already-read bytes are consumed"},
        Iterate_UTF8_to_UTF32_Case{
            "\xF0\x9D\x84", 0xFFFD, 3,
            "Truncated: leading + 2 valid continuations, buffer ends before "
            "the final byte; all 3 already-read bytes are consumed"},
        Iterate_UTF8_to_UTF32_Case{
            "\xF0\x9F\x98\x41", 0xFFFD, 3,
            "Final continuation byte is plain ASCII 'A': not consumed "
            "(prepended); leading byte + first 2 (valid) continuations are "
            "counted"}));

// --- Edge case: empty buffer ---
INSTANTIATE_TEST_SUITE_P(EdgeCases, Iterate_UTF8_to_UTF32_LOSSLY_Test,
                         ::testing::Values(Iterate_UTF8_to_UTF32_Case{
                             "", 0xFFFD, 0, "Empty input: nothing to read"}));
