#include "dumbutf/UTF.hpp"
#include "fmt/printf.h"
#include <string_view>

using namespace dumbutf;

int main(int argc, char **argv) {
  std::u8string_view W = u8"hehahah";
  char32_t CP;
  fmt::println("{}", iterate_UTF8_to_UTF32(W, CP));
  fmt::println("{:04X}", static_cast<UTF32>(CP));

  std::string_view Str = argv[1];

  fmt::println("{}", iterate_UTF8_to_UTF32_LOSSLY(Str, CP));

  return 0;
}
