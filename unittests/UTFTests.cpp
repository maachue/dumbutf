#include "Msg.h"
#include "dumbutf/UTF.hpp"
#include "simdutf.h"
#include "gtest/gtest.h"
#include <cassert>
#include <cstddef>
#include <string_view>

using namespace dumbutf;

// ---------------------------------------------------------------------
// iterate_UTF8_to_UTF32() tests
// ---------------------------------------------------------------------
struct IterateUTF8_to_UTF32_Case {
  std::string_view UTF8Input;
  UTF32 ExpectedCP;
  size_t ExpectedLength;
  const char *Desc;
};

class IterateUTF8_to_UTF32Test
    : public ::testing::TestWithParam<IterateUTF8_to_UTF32_Case> {};

TEST_P(IterateUTF8_to_UTF32Test, UTF8_to_UTF32_singleCP) {
  const auto &C = GetParam();
  auto CP = static_cast<UTF32>(-1);
  EXPECT_EQ(iterate_UTF8_to_UTF32(C.UTF8Input, CP), C.ExpectedLength)
      << "Str: " << C.UTF8Input << " " << CPMessage(C.ExpectedCP)
      << " Desc: " << C.Desc;
  EXPECT_EQ(CP, C.ExpectedCP) << CPMessage(C.ExpectedCP) << " Desc: " << C.Desc;
}

INSTANTIATE_TEST_SUITE_P(Nuthing, IterateUTF8_to_UTF32Test,
                         ::testing::Values(IterateUTF8_to_UTF32_Case{
                             "", static_cast<UTF32>(-1), 0, "Nuthing"}));

static const char NullString[] = "\0";

INSTANTIATE_TEST_SUITE_P(
    ASCII, IterateUTF8_to_UTF32Test,
    ::testing::Values(
        IterateUTF8_to_UTF32_Case{{NullString, 1}, 0x0000, 1, "NULL"},
        IterateUTF8_to_UTF32_Case{"\n", 0x000A, 1, "LINE FEED"},
        IterateUTF8_to_UTF32_Case{"a", 0x0061, 1, "LATIN SMALL LETTER A"},
        IterateUTF8_to_UTF32_Case{"F", 0x0046, 1, "LATIN CAPITAL LETTER F"},
        IterateUTF8_to_UTF32_Case{"abbccffggfvfrr", 0x0061, 1,
                                  "Multi-CP LATIN SMALL LATTER A"},
        IterateUTF8_to_UTF32_Case{"ffggbbggRRggffqqdsd", 0x0066, 1,
                                  "Multi-CP LATIN CAPITAL LETTER F"},
        IterateUTF8_to_UTF32_Case{"RRFFFFDDVBBGG", 0x0052, 1,
                                  "Multi-CP LATIN CAPITAL LETTER R"}));

// ---------------------------------------------------------------------
// 2-Byte UTF-8 Test Cases (Range: U+0080 - U+07FF)
// ---------------------------------------------------------------------
INSTANTIATE_TEST_SUITE_P(
    TwoBytes, IterateUTF8_to_UTF32Test,
    ::testing::Values(
        IterateUTF8_to_UTF32_Case{"¢", 0x00A2, 2, "CENT SIGN"},
        IterateUTF8_to_UTF32_Case{"é", 0x00E9, 2,
                                  "LATIN SMALL LETTER E WITH ACUTE"},
        IterateUTF8_to_UTF32_Case{"đ", 0x0111, 2,
                                  "LATIN SMALL LETTER D WITH STROKE"},
        IterateUTF8_to_UTF32_Case{"α", 0x03B1, 2, "GREEK SMALL LETTER ALPHA"},
        IterateUTF8_to_UTF32_Case{"éabc", 0x00E9, 2,
                                  "Multi-CP LATIN SMALL LETTER E WITH ACUTE"},
        IterateUTF8_to_UTF32_Case{"αβγ", 0x03B1, 2,
                                  "Multi-CP GREEK SMALL LETTER ALPHA"}));

// ---------------------------------------------------------------------
// 3-Byte UTF-8 Test Cases (Range: U+0800 - U+FFFF)
// ---------------------------------------------------------------------
INSTANTIATE_TEST_SUITE_P(
    ThreeBytes, IterateUTF8_to_UTF32Test,
    ::testing::Values(
        IterateUTF8_to_UTF32_Case{"�", 0xFFFD, 3, "REPLACEMENT CHARACTER"},
        IterateUTF8_to_UTF32_Case{"€", 0x20AC, 3, "EURO SIGN"},
        IterateUTF8_to_UTF32_Case{"あ", 0x3042, 3, "HIRAGANA LETTER A"},
        IterateUTF8_to_UTF32_Case{"雀", 0x96C0, 3,
                                  "CJK UNIFIED IDEOGRAPH-96C0"},
        IterateUTF8_to_UTF32_Case{"한", 0xD55C, 3, "HANGUL SYLLABLE HAN"},
        IterateUTF8_to_UTF32_Case{"雀aaassbb", 0x96C0, 3,
                                  "Multi-CP CJK UNIFIED IDEOGRAPH-96C0"},
        IterateUTF8_to_UTF32_Case{
            "雀！こにちわ！", 0x96C0, 3,
            "Multi-three bytes CP CJK UNIFIED IDEOGRAPH-96C0"}));

// ---------------------------------------------------------------------
// 4-Byte UTF-8 Test Cases (Range: U+10000 - U+10FFFF)
// ---------------------------------------------------------------------
INSTANTIATE_TEST_SUITE_P(
    FourBytes, IterateUTF8_to_UTF32Test,
    ::testing::Values(
        IterateUTF8_to_UTF32_Case{"𐍈", 0x10348, 4, "GOTHIC LETTER HWAIR"},
        IterateUTF8_to_UTF32_Case{"😀", 0x1F600, 4, "GRINNING FACE EMOJI"},
        IterateUTF8_to_UTF32_Case{"🚀", 0x1F680, 4, "ROCKET EMOJI"},
        IterateUTF8_to_UTF32_Case{"🀄", 0x1F004, 4, "MAHJONG TILE RED DRAGON"},
        IterateUTF8_to_UTF32_Case{"😀abc", 0x1F600, 4,
                                  "Multi-CP GRINNING FACE EMOJI"},
        IterateUTF8_to_UTF32_Case{"🚀🎉🔥", 0x1F680, 4,
                                  "Multi-four bytes EMOJI"}));

#ifdef DUMBUTF_TESTS_FULL
TEST(IterateUTF8_to_UTF32Test, AllCPs) {
  for (UTF32 CP = 0; CP <= 0x10FFFF; CP++) {
    char Tmp[4];
    auto Size = simdutf::convert_utf32_to_utf8(
        reinterpret_cast<const char32_t *>(&CP), 1, Tmp);
    if (Size == 0)
      continue;

    auto CPCon = static_cast<UTF32>(-1);
    EXPECT_EQ(dumbutf::iterate_UTF8_to_UTF32({Tmp, Size}, CPCon), Size);
    EXPECT_EQ(CPCon, CP);
  }
}
#endif
