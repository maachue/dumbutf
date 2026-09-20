#ifndef DUMBUTF_UNITTESTS_MSG_H
#define DUMBUTF_UNITTESTS_MSG_H

#include "dumbutf/datatypes.hpp"
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <ostream>
#include <string>

// std::string CPMessage(dumbutf::UTF32 cp) {
//   char buf[32];
//   std::snprintf(buf, sizeof(buf), "Failed at CP: U+%04X", cp);
//   return std::string(buf);
// }

struct CPMessage {
  dumbutf::UTF32 CP;
};

inline std::ostream &operator<<(std::ostream &OS, CPMessage const &V) {
  OS << "Failed at CP: U+" << std::hex << std::uppercase << std::setfill('0')
     << std::setw(4) << V.CP;
  return OS;
}

#endif
