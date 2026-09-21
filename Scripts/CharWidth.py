#!/bin/env python3

"""
This file generated data for codepointCharWidth and isAmbiguousWidth

Arguments:
SCRIPT $(unicode_data_dir) $(output_dir)

The width classifications are derived from utf8proc.
[data_generator.jl](https://github.com/JuliaStrings/utf8proc/blob/master/data/data_generator.jl)
"""

import sys
from pathlib import Path

UnicodeDataDir = Path(sys.argv[1])

UnicodeData: list[tuple[int, str]] = []

with open(UnicodeDataDir / "UnicodeData.txt", "r", encoding="utf-8") as file:
    begin: int = -1

    for line in file:
        line = line.split("#")[0].strip()
        if not line:
            continue

        parts = line.split(";")

        if parts[1].endswith("First>"):
            begin = int(parts[0].strip(), 16)
            continue

        if parts[1].endswith("Last>"):
            if begin == -1:
                raise RuntimeError("??")

            cate = parts[2].strip()
            for cp in range(begin, int(parts[0].strip(), 16) + 1):
                UnicodeData.append((cp, cate))

        UnicodeData.append((int(parts[0].strip(), 16), parts[2].strip()))

EawData: list[tuple[tuple[int, int], str]] = []

with open(UnicodeDataDir / "EastAsianWidth.txt", "r", encoding="utf-8") as file:
    for line in file:
        line = line.split("#")[0].strip()
        if not line:
            continue

        parts = line.split(";")

        range_ = parts[0].strip()
        cate = parts[1].strip()

        if ".." in range_:
            start, end = range_.split("..")
            EawData.append(((int(start, 16), int(end, 16)), cate))
        else:
            val = int(range_, 16)
            EawData.append(((val, val), cate))


def eaw_lookup(codepoint: int) -> str | None:
    for (start, end), value in EawData:
        if start <= codepoint <= end:
            return value
    return None


def eaw_width(s: str) -> int | None:
    s.strip()
    if s in ("Na", "H"):
        return 1
    elif s in ("F", "W"):
        return 2
    elif s == "A":
        return -1


ZeroWidthDataCP: list[int] = []
TwoWidthDataCP: list[int] = []

for codepoint, cate in UnicodeData:
    width = 1

    if cate in ("Mn", "Mc", "Me", "Zl", "Zp", "Cc", "Cf", "Cs"):
        width = 0

    eaw = eaw_lookup(codepoint)
    if eaw is not None:
        eaw = eaw_width(eaw)

        if eaw is not None:
            width = 1 if eaw < 0 else eaw

    if cate == "Mn":
        width = 0

    if codepoint == 0x00AD:
        width = 1
    elif codepoint in (0x2028, 0x2029):
        width = 0

    if width == 0:
        ZeroWidthDataCP.append(codepoint)
    elif width == 2:
        TwoWidthDataCP.append(codepoint)


def group_consecutive_ranges(codepoints: list[int]) -> list[tuple[int, int]]:
    if not codepoints:
        return []

    ranges = []
    start = codepoints[0]
    prev = codepoints[0]

    for cp in codepoints[1:]:
        if cp == prev + 1:
            prev = cp
        else:
            ranges.append((start, prev))
            start = cp
            prev = cp
    ranges.append((start, prev))
    return ranges


ZeroWidthData = group_consecutive_ranges(ZeroWidthDataCP)
TwoWidthData = group_consecutive_ranges(TwoWidthDataCP)

AmbiguousWidthData: list[tuple[int, int]] = []


for range_, cate in EawData:
    if cate.strip() == "A":
        AmbiguousWidthData.append(range_)


def merge_range(rr: list[tuple[int, int]]) -> list[tuple[int, int]]:
    merged_ranges = []

    for r in rr:
        if not merged_ranges:
            merged_ranges.append(r)
        else:
            prev_start, prev_end = merged_ranges[-1]
            if r[0] <= prev_end + 1:
                merged_ranges[-1] = (prev_start, max(prev_end, r[1]))
            else:
                merged_ranges.append(r)

    return merged_ranges


ZeroWidthData = merge_range(ZeroWidthData)
TwoWidthData = merge_range(TwoWidthData)
AmbiguousWidthData = merge_range(AmbiguousWidthData)


def write_to_file(output: Path, name: str, data: list[tuple[int, int]]) -> None:
    with open(output, "w", encoding="utf-8") as file:
        file.write(f"constexpr CPRange {name}[] = {{\n")
        for start, end in data:
            file.write(f"{{ 0x{start:04X}, 0x{end:04X} }},\n")
        file.write("};")


OutputDir = Path(sys.argv[2])

write_to_file(OutputDir / "ZeroWidth.inc", "ZeroWidthRanges", ZeroWidthData)
write_to_file(OutputDir / "TwoWidth.inc", "TwoWidthRanges", TwoWidthData)
write_to_file(
    OutputDir / "AmbiguousWidth.inc", "AmbiguousWidthRanges", AmbiguousWidthData
)
