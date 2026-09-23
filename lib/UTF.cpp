#include "dumbutf/UTF.hpp"
#include "dumbutf/datatypes.hpp"
#include <cstddef>
#include <cstdint>

namespace dumbutf {
unsigned singleCodepointUTF8Length(UTF8 Start) {
  constexpr const char trailingBytesForUTF8[256] = {
      // old RFC 2279
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

// Ths function below derived from `utf8proc` (utf8proc_iterate)
// Licensed under MIT "expat" license
#define UTF8_CONT(x) (((x) & 0xC0) == 0x80)
int iterate_UTF8_to_UTF32(const UTF8 *Str, size_t Len, UTF32 &CP) {
  CP = 0xFFFD;

  if (Len == 0)
    return 0;

  auto UC = static_cast<UTF32>(static_cast<UTF8>(Str[0]));

  if (UC < 0x0080) {
    CP = UC;
    return 1;
  }

  if ((UTF32)(UC - 0xC2) > (0xF4 - 0xC2))
    return INVALID_UTF8;

  // 2 bytes
  if (UC < 0xE0) {
    if (Len < 2 || !UTF8_CONT(Str[1]))
      return INVALID_UTF8;
    CP = ((UC & 0x1F) << 6) | (Str[1] & 0x3F);
    return 2;
  }

  if (UC < 0xF0) {
    if (Len < 3 || !UTF8_CONT(Str[1]) || !UTF8_CONT(Str[2]))
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
  if (Len < 4 || !UTF8_CONT(Str[1]) || !UTF8_CONT(Str[2]) || !UTF8_CONT(Str[3]))
    return INVALID_UTF8;

  if (UC == 0xF0)
    if (Str[1] < 0x90 || (UC == 0xF4 && Str[1] > 0x8F))
      return INVALID_UTF8;

  CP = ((UC & 7) << 18) | ((Str[1] & 0x3F) << 12) | ((Str[2] & 0x3f) << 6) |
       (Str[3] & 0x3F);

  return 4;
}

size_t iterate_UTF8_to_UTF32_LOSSLY(const UTF8 *Str, size_t Len, UTF32 &CP) {
  if (Len == 0) {
    CP = UTF32_REPLACEMENT_CHARACTER;
    return 0;
  }

  auto b0 = static_cast<uint8_t>(Str[0]);

  if (b0 <= 0x7F) {
    CP = b0;
    return 1;
  }

  uint32_t Needed = 0;
  uint8_t lower = 0x80;
  uint8_t upper = 0xBF;

  if (b0 >= 0xC2 && b0 <= 0xDF) {
    Needed = 1;
    CP = b0 & 0x1F;
  } else if (b0 >= 0xE0 && b0 <= 0xEF) {
    if (b0 == 0xE0)
      lower = 0xA0;
    if (b0 == 0xED)
      upper = 0x9F;
    Needed = 2;
    CP = b0 & 0x0F;
  } else if (b0 >= 0xF0 && b0 <= 0xF4) {
    if (b0 == 0xF0)
      lower = 0x90;
    if (b0 == 0xF4)
      upper = 0x8F;
    Needed = 3;
    CP = b0 & 0x07;
  } else {
    CP = 0xFFFD;
    return 1;
  }

  for (size_t i = 1; i <= Needed; ++i) {
    if (i >= Len) {
      CP = 0xFFFD;
      return i;
    }

    auto b = static_cast<UTF8>(Str[i]);

    UTF8 CurrentLower = (i == 1) ? lower : 0x80;
    UTF8 CurrentUpper = (i == 1) ? upper : 0xBF;

    if (b < CurrentLower || b > CurrentUpper) {
      CP = 0xFFFD;
      return i;
    }

    CP = (CP << 6) | (b & 0x3F);
  }

  return Needed + 1;
}

ptrdiff_t length_UTF8_to_UTF32(const UTF8 *Str, size_t Len) {
  if (Len == 0)
    return 0;

  ptrdiff_t Result = 0;
  size_t Count = 0;
  for (size_t I = 0; I < Len; I += Count) {
    UTF32 b = Str[I];

    if (b <= 0x0080) {
      Result++;
      Count = 1;
      continue;
    }

    if ((UTF32)(b - 0xC2) > (0xF4 - 0xC2))
      return INVALID_UTF8;

    if (b < 0xE0) {
      if (Len < I + 2 || !UTF8_CONT(Str[I + 1]))
        return INVALID_UTF8;

      Count = 2;
      continue;
    }

    if (b < 0xF0) {
      if (Len < I + 3 || !UTF8_CONT(Str[I + 1]) || !UTF8_CONT(Str[I + 2]))
        return INVALID_UTF8;

      if (b == 0xED && static_cast<UTF8>(Str[I + 1]) > 0x9F)
        return INVALID_UTF8;

      b = ((b & 0xF) << 12) | ((Str[I + 1] & 0x3F) << 6) | (Str[I + 2] & 0x3F);
      if (b < 0x800)
        return INVALID_UTF8;

      Result++;
      Count = 3;
      continue;
    }

    // 4 bytes
    if (Len < I + 4 || !UTF8_CONT(Str[I + 1]) || !UTF8_CONT(Str[I + 2]) ||
        !UTF8_CONT(Str[3]))
      return INVALID_UTF8;

    if (b == 0xF0) {
      if (Str[I + 1] < 0x90)
        return INVALID_UTF8;
      if (b == 0xF4)
        if (Str[I + 1] > 0x8F)
          return INVALID_UTF8;

      Result++;
      Count = 4;
    }
  }

  return Result;
}
size_t length_UTF8_to_UTF32_LOSSLY(const UTF8 *Str, size_t Len) {
  if (Len == 0)
    return 0;

  size_t Count = 0;
  size_t Result = 0;
  for (size_t i = 0; i < Len; i += Count) {
    auto b0 = static_cast<uint8_t>(Str[i]);
    if (b0 <= 0x7F) {
      Count = 1;
      Result++;
      continue;
    }

    int Needed = 0;
    uint8_t lower = 0x80;
    uint8_t upper = 0xBF;

    if (b0 >= 0xC2 && b0 <= 0xDF) {
      Needed = 1;
    } else if (b0 >= 0xE0 && b0 <= 0xEF) {
      if (b0 == 0xE0)
        lower = 0xA0;
      if (b0 == 0xED)
        upper = 0x9F;
      Needed = 2;
    } else if (b0 >= 0xF0 && b0 <= 0xF4) {
      if (b0 == 0xF0)
        lower = 0x90;
      if (b0 == 0xF4)
        upper = 0x8F;
      Needed = 3;
    } else {
      Result++;
      Count = 1;
      continue;
    }

    bool Skip = false;
    for (size_t j = 1; j <= Needed; ++j) {
      if (i + j >= Len) {
        Result++;
        Count = j;
        Skip = true;
        break;
      }

      auto b = static_cast<UTF8>(Str[i + j]);

      UTF8 CurrentLower = (j == 1) ? lower : 0x80;
      UTF8 CurrentUpper = (j == 1) ? upper : 0xBF;

      if (b < CurrentLower || b > CurrentUpper) {
        Result++;
        Count = j;
        Skip = true;
        break;
      }
    }

    if (!Skip) {
      Result++;
      Count = Needed + 1;
    }
  }

  return Result;
}

ptrdiff_t convert_UTF8_to_UTF32_LOSSLY(const UTF8 *UTF8Str, size_t UTF8StrLen,
                                       UTF32 *UTF32Str, size_t UTF32StrLen) {
  if (!UTF8StrLen)
    return 0;

  size_t Count = 0;
  size_t Curr = 0;
  for (size_t i = 0; i < UTF8StrLen; i += Count) {
    UTF32 U;
    Count = iterate_UTF8_to_UTF32_LOSSLY(UTF8Str + i, UTF8StrLen - i, U);
    if (Curr >= UTF32StrLen) {
      return BUFFER_TOO_SMALL;
    }
    UTF32Str[Curr++] = U;
  }

  return Curr;
}
} // namespace dumbutf
