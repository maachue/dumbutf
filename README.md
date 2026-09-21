# dumbutf

Dumb and st*pid library about Unicode.

## Why design/code this?

I don't know. I just want to do something to kill time.

## Required

C++20.

## Example

```cpp
#include "dumbutf/dumbutf.hpp"

int main(int argc, char **argv) {
  if (argc < 2)
    return 1;

  std::string_view Str = argv[1];

  auto Size = length_UTF8_to_UTF32_LOSSLY(Str);

  dumbutf::UTF32 *UTF32Str = ::new UTF32[Size];
  
  auto S = convert_UTF8_to_UTF32_LOSSLY(Str, UTF32Str, Size);
  if (S <= 0) {
    delete[] UTF32Str;
    return 1;
  }

  delete[] UTF32Str;
  return 0;
}
```

## Third party

- [`utf8proc`](https://github.com/JuliaStrings/utf8proc) - MIT/"legacy" utf8proc license, [./utf8proc/], only use for testing.

## AI Usage

Yup, I really tired with writing test cases and documenting.

## License

***!!!READ CAREFULLY LICENSE FILE BEFORE USING THIS PROJECT!!!***

[MIT AND MIT AND UNICODE-LICENSE-V3](./LICENSE)

Thanks to [`utf8proc`](https://github.com/JuliaStrings/utf8proc) a lot!
