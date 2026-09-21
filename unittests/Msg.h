#ifndef DUMBUTF_UNITTESTS_MSG_H
#define DUMBUTF_UNITTESTS_MSG_H

#include "dumbutf/datatypes.hpp"
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <ostream>
#include <span>
#include <string>

struct CPPrint {
  dumbutf::UTF32 CP;
};

inline std::ostream &operator<<(std::ostream &OS, CPPrint const &V) {
  OS << "U+" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
     << V.CP;
  return OS;
}

struct CPSPrint {
  std::span<const dumbutf::UTF32> CPs;
};

inline std::ostream &operator<<(std::ostream &OS, CPSPrint const &V) {
  if (V.CPs.empty())
    return OS;

  OS << CPPrint(V.CPs[0]);
  for (size_t i = 1; i < V.CPs.size(); ++i) {
    OS << " " << CPPrint(V.CPs[i]);
  }

  return OS;
}

#endif
