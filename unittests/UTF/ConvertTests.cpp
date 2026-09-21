#include "dumbutf/UTF.hpp"

#include "Msg.h"

#include <ostream>
#include <span>
#include <string_view>

using namespace dumbutf;

namespace {
struct Convert_UTF8_to_UTF32_Case {
  std::string_view UTF8Input;
  std::span<UTF32> UTF32StringExpected;
  const char *Desc = nullptr;

  friend std::ostream &operator<<(std::ostream &OS,
                                  Convert_UTF8_to_UTF32_Case const &V) {
    OS << "UTF-8 Input: '" << V.UTF8Input << "'; UTF-32 Expected: '"
       << CPSPrint(V.UTF32StringExpected) << "'";
    if (V.Desc)
      OS << "; Desc: " << V.Desc;

    return OS;
  }
};
} // namespace
