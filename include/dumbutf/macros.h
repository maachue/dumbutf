#ifndef DUMBUTF_MACROS_H
#define DUMBUTF_MACROS_H

#ifndef __has_attribute
#  define __has_attribute(x) 0
#endif

#if __has_attribute(always_inline)
#  define DUMBUTF_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#  define DUMBUTF_ALWQAYS_INLINE __force_inline
#else
#  define DUMBUTF_ALWAYS_INLINE inline
#endif

#endif
