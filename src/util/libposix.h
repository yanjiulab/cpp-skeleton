
#ifndef LIBPOSIX_H
#define LIBPOSIX_H

#define LIBPOSIX_VERSION "0.1.1"

/*---------------------------------------------------------------------------
                            OS
----------------------------------------------------------------------------*/
#if defined(WIN64) || defined(_WIN64)
#define OS_WIN64
#define OS_WIN32
#elif defined(WIN32) || defined(_WIN32)
#define OS_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#define OS_ANDROID
#define OS_LINUX
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define OS_LINUX
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#include <TargetConditionals.h>
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define OS_MAC
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#define OS_IOS
#endif
#define OS_DARWIN
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#define OS_FREEBSD
#define OS_BSD
#elif defined(__NetBSD__)
#define OS_NETBSD
#define OS_BSD
#elif defined(__OpenBSD__)
#define OS_OPENBSD
#define OS_BSD
#elif defined(sun) || defined(__sun) || defined(__sun__)
#define OS_SOLARIS
#else
#warning "Untested operating system platform!"
#endif

#if defined(OS_WIN32) || defined(OS_WIN64)
#undef OS_UNIX
#define OS_WIN
#else
#define OS_UNIX
#endif

// #if defined(OS_WIN)
// #include "libposix4win.h"
// #elif defined(OS_UNIX)
// #include "libposix4nix.h"
// #elif defined(OS_DARWIN)
// #include "libposix4apple.h"
// #elif defined(OS_ANDROID)
// #define OS_ANDROID
// #include "libposix4roid.h"
// #elif defined(OS_RTOS)
// #include "libposix4rtos.h"
// #else
// #warning "Untested operating system platform!"
// #endif

/*---------------------------------------------------------------------------
                            ARCH
----------------------------------------------------------------------------*/
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#define ARCH_X64
#define ARCH_X86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define ARCH_X86
#define ARCH_X86_32
#elif defined(__aarch64__) || defined(__ARM64__) || defined(_M_ARM64)
#define ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#define ARCH_ARM
#elif defined(__mips64__)
#define ARCH_MIPS64
#elif defined(__mips__)
#define ARCH_MIPS
#else
#warning "Untested hardware architecture!"
#endif

/*---------------------------------------------------------------------------
                            COMPILER
----------------------------------------------------------------------------*/
#if defined(_MSC_VER)
#define COMPILER_MSVC

#if (_MSC_VER < 1200) // Visual C++ 6.0
#define MSVS_VERSION 1998
#define MSVC_VERSION 60
#elif (_MSC_VER >= 1200) && (_MSC_VER < 1300) // Visual Studio 2002, MSVC++ 7.0
#define MSVS_VERSION 2002
#define MSVC_VERSION 70
#elif (_MSC_VER >= 1300) && (_MSC_VER < 1400) // Visual Studio 2003, MSVC++ 7.1
#define MSVS_VERSION 2003
#define MSVC_VERSION 71
#elif (_MSC_VER >= 1400) && (_MSC_VER < 1500) // Visual Studio 2005, MSVC++ 8.0
#define MSVS_VERSION 2005
#define MSVC_VERSION 80
#elif (_MSC_VER >= 1500) && (_MSC_VER < 1600) // Visual Studio 2008, MSVC++ 9.0
#define MSVS_VERSION 2008
#define MSVC_VERSION 90
#elif (_MSC_VER >= 1600) && (_MSC_VER < 1700) // Visual Studio 2010, MSVC++ 10.0
#define MSVS_VERSION 2010
#define MSVC_VERSION 100
#elif (_MSC_VER >= 1700) && (_MSC_VER < 1800) // Visual Studio 2012, MSVC++ 11.0
#define MSVS_VERSION 2012
#define MSVC_VERSION 110
#elif (_MSC_VER >= 1800) && (_MSC_VER < 1900) // Visual Studio 2013, MSVC++ 12.0
#define MSVS_VERSION 2013
#define MSVC_VERSION 120
#elif (_MSC_VER >= 1900) && (_MSC_VER < 1910) // Visual Studio 2015, MSVC++ 14.0
#define MSVS_VERSION 2015
#define MSVC_VERSION 140
#elif (_MSC_VER >= 1910) && (_MSC_VER < 1920) // Visual Studio 2017, MSVC++ 15.0
#define MSVS_VERSION 2017
#define MSVC_VERSION 150
#elif (_MSC_VER >= 1920) && (_MSC_VER < 2000) // Visual Studio 2019, MSVC++ 16.0
#define MSVS_VERSION 2019
#define MSVC_VERSION 160
#endif

#pragma warning(disable : 4018) // signed/unsigned comparison
#pragma warning(disable : 4100) // unused param
#pragma warning(disable : 4102) // unreferenced label
#pragma warning(disable : 4244) // conversion loss of data
#pragma warning(disable : 4267) // size_t => int
#pragma warning(disable : 4819) // Unicode
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS

#elif defined(__GNUC__)
#define COMPILER_GCC

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#elif defined(__clang__)
#define COMPILER_CLANG

#elif defined(__MINGW32__) || defined(__MINGW64__)
#define COMPILER_MINGW

#elif defined(__MSYS__)
#define COMPILER_MSYS

#elif defined(__CYGWIN__)
#define COMPILER_CYGWIN

#else
#warning "Untested compiler!"
#endif

#if defined(__clang__)
#if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 5)
#define _RET_NONNULL , returns_nonnull
#endif
#if __has_attribute(fallthrough)
#define _FALLTHROUGH __attribute__((fallthrough));
#endif
#define _CONSTRUCTOR(x) constructor(x)
#define _DEPRECATED(x)  deprecated(x)
#if __has_builtin(assume)
#define assume(x) __builtin_assume(x)
#endif
#elif defined(__GNUC__)
#if (__GNUC__ >= 3)
#define likely(_x)   __builtin_expect(!!(_x), 1)
#define unlikely(_x) __builtin_expect(!!(_x), 0)
#endif
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)
#define _RET_NONNULL , returns_nonnull
#endif
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#define _CONSTRUCTOR(x) constructor(x)
#define _DESTRUCTOR(x)  destructor(x)
#define _ALLOC_SIZE(x)  alloc_size(x)
#endif
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#define _DEPRECATED(x) deprecated(x)
#define assume(x)                          \
    do {                                   \
        if (!(x)) __builtin_unreachable(); \
    } while (0)
#endif
#if __GNUC__ < 5
#define __has_attribute(x) 0
#endif
#if __GNUC__ >= 7
#define _FALLTHROUGH __attribute__((fallthrough));
#endif

#else
#define likely(_x)   !!(_x)
#define unlikely(_x) !!(_x)

#endif

/*
 * Fix Visual Studio Code error: attribute "constructor" does not take
 * arguments. Caused by the macro `DEFINE_MTYPE_ATTR`.
 */
#ifdef __INTELLISENSE__
#pragma diag_suppress 1094
#endif /* __INTELISENSE__ */

#if __has_attribute(hot)
#define _OPTIMIZE_HOT __attribute__((hot))
#else
#define _OPTIMIZE_HOT
#endif
#if __has_attribute(optimize)
#define _OPTIMIZE_O3 __attribute__((optimize("3")))
#else
#define _OPTIMIZE_O3
#endif
#define OPTIMIZE _OPTIMIZE_O3 _OPTIMIZE_HOT

#if !defined(__GNUC__)
#error module code needs GCC visibility extensions
#elif __GNUC__ < 4
#error module code needs GCC visibility extensions
#else
#define DSO_PUBLIC __attribute__((visibility("default")))
#define DSO_SELF   __attribute__((visibility("protected")))
#define DSO_LOCAL  __attribute__((visibility("hidden")))
#endif

#ifdef __sun
/* Solaris doesn't do constructor priorities due to linker restrictions */
#undef _CONSTRUCTOR
#undef _DESTRUCTOR
#endif

/* fallback versions */
#ifndef _RET_NONNULL
#define _RET_NONNULL
#endif
#ifndef _CONSTRUCTOR
#define _CONSTRUCTOR(x) constructor
#endif
#ifndef _DESTRUCTOR
#define _DESTRUCTOR(x) destructor
#endif
#ifndef _ALLOC_SIZE
#define _ALLOC_SIZE(x)
#endif
#ifndef _FALLTHROUGH
#define _FALLTHROUGH
#endif
#ifndef _DEPRECATED
#define _DEPRECATED(x) deprecated
#endif
#ifndef assume
#define assume(x)
#endif

/*---------------------------------------------------------------------------
                            MACROS
----------------------------------------------------------------------------*/

/* for helper functions defined inside macros */
#define macro_inline static inline __attribute__((unused))
#define macro_pure   static inline __attribute__((unused, pure))

/* if the macro ends with a function definition */
#define MACRO_REQUIRE_SEMICOLON() \
    _Static_assert(1, "please add a semicolon after this macro")

/* variadic macros, use like:
 * #define V_0()  ...
 * #define V_1(x) ...
 * #define V(...) MACRO_VARIANT(V, ##__VA_ARGS__)(__VA_ARGS__)
 */
#define _MACRO_VARIANT(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, N, ...) N

#define _CONCAT2(a, b)                                                      a##b
#define _CONCAT(a, b)                                                       _CONCAT2(a, b)

#define MACRO_VARIANT(NAME, ...)                   \
    _CONCAT(NAME, _MACRO_VARIANT(0, ##__VA_ARGS__, \
                                 _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0))

#define NAMECTR(name) _CONCAT(name, __COUNTER__)

/* per-arg repeat macros, use like:
 * #define PERARG(n) ...n...
 * #define FOO(...) MACRO_REPEAT(PERARG, ##__VA_ARGS__)
 */

#define _MACRO_REPEAT_0(NAME)
#define _MACRO_REPEAT_1(NAME, A1) \
    NAME(A1)
#define _MACRO_REPEAT_2(NAME, A1, A2) \
    NAME(A1)                          \
    NAME(A2)
#define _MACRO_REPEAT_3(NAME, A1, A2, A3) \
    NAME(A1)                              \
    NAME(A2)                              \
    NAME(A3)
#define _MACRO_REPEAT_4(NAME, A1, A2, A3, A4) \
    NAME(A1)                                  \
    NAME(A2)                                  \
    NAME(A3)                                  \
    NAME(A4)
#define _MACRO_REPEAT_5(NAME, A1, A2, A3, A4, A5) \
    NAME(A1)                                      \
    NAME(A2)                                      \
    NAME(A3)                                      \
    NAME(A4)                                      \
    NAME(A5)
#define _MACRO_REPEAT_6(NAME, A1, A2, A3, A4, A5, A6) \
    NAME(A1)                                          \
    NAME(A2)                                          \
    NAME(A3)                                          \
    NAME(A4)                                          \
    NAME(A5)                                          \
    NAME(A6)
#define _MACRO_REPEAT_7(NAME, A1, A2, A3, A4, A5, A6, A7) \
    NAME(A1)                                              \
    NAME(A2)                                              \
    NAME(A3)                                              \
    NAME(A4)                                              \
    NAME(A5)                                              \
    NAME(A6)                                              \
    NAME(A7)
#define _MACRO_REPEAT_8(NAME, A1, A2, A3, A4, A5, A6, A7, A8) \
    NAME(A1)                                                  \
    NAME(A2)                                                  \
    NAME(A3)                                                  \
    NAME(A4)                                                  \
    NAME(A5)                                                  \
    NAME(A6)                                                  \
    NAME(A7)                                                  \
    NAME(A8)

#define MACRO_REPEAT(NAME, ...)                 \
    MACRO_VARIANT(_MACRO_REPEAT, ##__VA_ARGS__) \
    (NAME, ##__VA_ARGS__)

/* per-arglist repeat macro, use like this:
 * #define foo(...) MAP_LISTS(F, ##__VA_ARGS__)
 * where F is a n-ary function where n is the number of args in each arglist.
 * e.g.: MAP_LISTS(f, (a, b), (c, d))
 * expands to: f(a, b); f(c, d)
 */

#define ESC(...) __VA_ARGS__
#define MAP_LISTS(M, ...)                      \
    _CONCAT(_MAP_LISTS_, PP_NARG(__VA_ARGS__)) \
    (M, ##__VA_ARGS__)
#define _MAP_LISTS_0(M)
#define _MAP_LISTS_1(M, _1)                 ESC(M _1)
#define _MAP_LISTS_2(M, _1, _2)             ESC(M _1; M _2)
#define _MAP_LISTS_3(M, _1, _2, _3)         ESC(M _1; M _2; M _3)
#define _MAP_LISTS_4(M, _1, _2, _3, _4)     ESC(M _1; M _2; M _3; M _4)
#define _MAP_LISTS_5(M, _1, _2, _3, _4, _5) ESC(M _1; M _2; M _3; M _4; M _5)
#define _MAP_LISTS_6(M, _1, _2, _3, _4, _5, _6) \
    ESC(M _1; M _2; M _3; M _4; M _5; M _6)
#define _MAP_LISTS_7(M, _1, _2, _3, _4, _5, _6, _7) \
    ESC(M _1; M _2; M _3; M _4; M _5; M _6; M _7)
#define _MAP_LISTS_8(M, _1, _2, _3, _4, _5, _6, _7, _8) \
    ESC(M _1; M _2; M _3; M _4; M _5; M _6; M _7; M _8)

/*
 * for warnings on macros, put in the macro content like this:
 *   #define MACRO BLA CPP_WARN("MACRO has been deprecated")
 */
#define CPP_STR(X) #X

#if defined(__ICC)
#define CPP_NOTICE(text) _Pragma(CPP_STR(message __FILE__ ": " text))
#define CPP_WARN(text)   CPP_NOTICE(text)

#elif (defined(__GNUC__) && (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))) || (defined(__clang__) && (__clang_major__ >= 4 || (__clang_major__ == 3 && __clang_minor__ >= 5)))
#define CPP_WARN(text)   _Pragma(CPP_STR(GCC warning text))
#define CPP_NOTICE(text) _Pragma(CPP_STR(message text))

#else
#define CPP_WARN(text)
#define CPP_NOTICE(text)
#endif

/* Some insane macros to count number of varargs to a functionlike macro */
#define PP_ARG_N(                                     \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,          \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
    _61, _62, _63, N, ...) N

#define PP_RSEQ_N()                             \
    62, 61, 60,                                 \
        59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
        49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
        39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_NARG(...)  PP_NARG_(_, ##__VA_ARGS__, PP_RSEQ_N())

/*---------------------------------------------------------------------------
                            Usefual defs
----------------------------------------------------------------------------*/
#define ABS(n)        ((n) > 0 ? (n) : -(n))
#define NABS(n)       ((n) < 0 ? (n) : -(n))
#define MAX(a, b)                          \
    ({                                     \
        typeof(a) _max_a = (a);            \
        typeof(b) _max_b = (b);            \
        _max_a > _max_b ? _max_a : _max_b; \
    })

#define MIN(a, b)                          \
    ({                                     \
        typeof(a) _min_a = (a);            \
        typeof(b) _min_b = (b);            \
        _min_a < _min_b ? _min_a : _min_b; \
    })

#define SWAP(a, b)             \
    do {                       \
        typeof(a) __tmp = (a); \
        (a) = (b);             \
        (b) = __tmp;           \
    } while (0)

#define LIMIT(lower, v, upper) ((v) < (lower) ? (lower) : (v) > (upper) ? (upper) \
                                                                        : (v))

#define MAX_PATH 260

#define DUMP_BUFFER(buf, len)                                            \
    do {                                                                 \
        int _i, _j = 0;                                                  \
        char _tmp[128] = {0};                                            \
        if (buf == NULL || len <= 0) {                                   \
            break;                                                       \
        }                                                                \
        for (_i = 0; _i < len; _i++) {                                   \
            if (!(_i % 16)) {                                            \
                if (_i != 0) {                                           \
                    printf("%s", _tmp);                                  \
                }                                                        \
                memset(_tmp, 0, sizeof(_tmp));                           \
                _j = 0;                                                  \
                _j += snprintf(_tmp + _j, 64, "\n%p: ", buf + _i);       \
            }                                                            \
            _j += snprintf(_tmp + _j, 4, "%02hhx ", *((char*)buf + _i)); \
        }                                                                \
        printf("%s\n", _tmp);                                            \
    } while (0)

#define STRINGIFY(x)           STRINGIFY_HELPER(x)
#define STRINGIFY_HELPER(x)    #x

#define STRINGCAT(x, y)        STRINGCAT_HELPER(x, y)
#define STRINGCAT_HELPER(x, y) x##y

#define numcmp(a, b)                                          \
    ({                                                        \
        typeof(a) _cmp_a = (a);                               \
        typeof(b) _cmp_b = (b);                               \
        (_cmp_a < _cmp_b) ? -1 : ((_cmp_a > _cmp_b) ? 1 : 0); \
    })

#define array_size(ar) (sizeof(ar) / sizeof(ar[0]))

#ifndef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE*)0)->MEMBER)
#endif
#endif

#ifdef container_of
#undef container_of
#endif

#if !(defined(__cplusplus) || defined(test__cplusplus))
/* this variant of container_of() retains 'const' on pointers without needing
 * to be told to do so.  The following will all work without warning:
 *
 * struct member *p;
 * const struct member *cp;
 *
 * const struct cont *x = container_of(cp, struct cont, member);
 * const struct cont *x = container_of(cp, const struct cont, member);
 * const struct cont *x = container_of(p,  struct cont, member);
 * const struct cont *x = container_of(p,  const struct cont, member);
 * struct cont *x       = container_of(p,  struct cont, member);
 *
 * but the following will generate warnings about stripping const:
 *
 * struct cont *x       = container_of(cp, struct cont, member);
 * struct cont *x       = container_of(cp, const struct cont, member);
 * struct cont *x       = container_of(p,  const struct cont, member);
 */
#define container_of(ptr, type, member)                           \
    (__builtin_choose_expr(                                       \
        __builtin_types_compatible_p(typeof(&((type*)0)->member), \
                                     typeof(ptr)) ||              \
            __builtin_types_compatible_p(void*, typeof(ptr)),     \
        ({                                                        \
            typeof(((type*)0)->member)* __mptr = (void*)(ptr);    \
            (type*)((char*)__mptr - offsetof(type, member));      \
        }),                                                       \
        ({                                                        \
            typeof(((const type*)0)->member)* __mptr = (ptr);     \
            (const type*)((const char*)__mptr -                   \
                          offsetof(type, member));                \
        })))
#else
/* current C++ compilers don't have the builtins used above; so this version
 * of the macro doesn't do the const check. */
#define container_of(ptr, type, member)                   \
    ({                                                    \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })
#endif

#define container_of_null(ptr, type, member)            \
    ({                                                  \
        typeof(ptr) _tmp = (ptr);                       \
        _tmp ? container_of(_tmp, type, member) : NULL; \
    })

/*---------------------------------------------------------------------------
                            API
----------------------------------------------------------------------------*/
#if defined(__GNUC__)
#define EXPORT __attribute__((visibility("default")))
#define HIDDEN __attribute__((visibility("hidden")))
#else
#define EXPORT
#define HIDDEN
#endif

#define INLINE static inline

#ifdef __cplusplus

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#ifndef BEGIN_EXTERN_C
#define BEGIN_EXTERN_C extern "C" {
#endif

#ifndef END_EXTERN_C
#define END_EXTERN_C } // extern "C"
#endif

#ifndef DEFAULT
#define DEFAULT(x) = x
#endif

#else

#define EXTERN_C extern
#define BEGIN_EXTERN_C
#define END_EXTERN_C

#ifndef DEFAULT
#define DEFAULT(x)
#endif

#endif // __cplusplus

/*---------------------------------------------------------------------------
                            Standard Headers
----------------------------------------------------------------------------*/
/* ANSI C (C89) */
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
/* C99 */
#include <stdbool.h>
#include <stdint.h>
/* C11 */
#include <stdatomic.h>
#include <inttypes.h>
// #include <threads.h> // use pthread
/* POSIX/Unix Headers */
#define __USE_XOPEN2K 1 // for posix extension
#ifdef OS_UNIX
#include <unistd.h>
#include <dirent.h> // for mkdir,rmdir,chdir,getcwd
// socket
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netdb.h> // for gethostbyname
// Frequently used
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h> // for gettimeofday
#include <fcntl.h>
#include <pthread.h>
#include <features.h>

// sleep, usleep
#define msleep(ms)     usleep((ms) * 1000)
#define delay(ms)      msleep(ms)
#define mkdir_777(dir) mkdir(dir, 0777)
#endif

/*---------------------------------------------------------------------------
                            ENDIAN & BYTE_ORDER
----------------------------------------------------------------------------*/
/* ENDIAN */
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef NET_ENDIAN
#define NET_ENDIAN BIG_ENDIAN
#endif

/* BYTE_ORDER */
#ifndef BYTE_ORDER
#if defined(__BYTE_ORDER)
#define BYTE_ORDER __BYTE_ORDER
#elif defined(__BYTE_ORDER__)
#define BYTE_ORDER __BYTE_ORDER__
#elif defined(ARCH_X86) || defined(ARCH_X86_64) ||  \
    defined(__ARMEL__) || defined(__AARCH64EL__) || \
    defined(__MIPSEL) || defined(__MIPS64EL)
#define BYTE_ORDER LITTLE_ENDIAN
#elif defined(__ARMEB__) || defined(__AARCH64EB__) || \
    defined(__MIPSEB) || defined(__MIPS64EB)
#define BYTE_ORDER BIG_ENDIAN
#elif defined(OS_WIN)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#warning "Unknown byte order!"
#endif
#endif

#if defined(OS_MAC)
#include <libkern/OSByteOrder.h>
#define htobe16(v) OSSwapHostToBigInt16(v)
#define htobe32(v) OSSwapHostToBigInt32(v)
#define htobe64(v) OSSwapHostToBigInt64(v)
#define be16toh(v) OSSwapBigToHostInt16(v)
#define be32toh(v) OSSwapBigToHostInt32(v)
#define be64toh(v) OSSwapBigToHostInt64(v)

#define htole16(v) OSSwapHostToLittleInt16(v)
#define htole32(v) OSSwapHostToLittleInt32(v)
#define htole64(v) OSSwapHostToLittleInt64(v)
#define le16toh(v) OSSwapLittleToHostInt16(v)
#define le32toh(v) OSSwapLittleToHostInt32(v)
#define le64toh(v) OSSwapLittleToHostInt64(v)
#elif defined(OS_WIN)
#define htobe16(v) htons(v)
#define htobe32(v) htonl(v)
#define htobe64(v) htonll(v)
#define be16toh(v) ntohs(v)
#define be32toh(v) ntohl(v)
#define be64toh(v) ntohll(v)

#if (BYTE_ORDER == LITTLE_ENDIAN)
#define htole16(v) (v)
#define htole32(v) (v)
#define htole64(v) (v)
#define le16toh(v) (v)
#define le32toh(v) (v)
#define le64toh(v) (v)
#elif (BYTE_ORDER == BIG_ENDIAN)
#define htole16(v) __builtin_bswap16(v)
#define htole32(v) __builtin_bswap32(v)
#define htole64(v) __builtin_bswap64(v)
#define le16toh(v) __builtin_bswap16(v)
#define le32toh(v) __builtin_bswap32(v)
#define le64toh(v) __builtin_bswap64(v)
#endif
#elif defined(OS_UNIX)
#include <endian.h>
#else
#warning "Not found endian.h!"
#endif

#define PI8(p)         *(int8_t*)(p)
#define PI16(p)        *(int16_t*)(p)
#define PI32(p)        *(int32_t*)(p)
#define PI64(p)        *(int64_t*)(p)

#define PU8(p)         *(uint8_t*)(p)
#define PU16(p)        *(uint16_t*)(p)
#define PU32(p)        *(uint32_t*)(p)
#define PU64(p)        *(uint64_t*)(p)

#define PF32(p)        *(float*)(p)
#define PF64(p)        *(double*)(p)

#define GET_BE16(p)    be16toh(PU16(p))
#define GET_BE32(p)    be32toh(PU32(p))
#define GET_BE64(p)    be64toh(PU64(p))

#define GET_LE16(p)    le16toh(PU16(p))
#define GET_LE32(p)    le32toh(PU32(p))
#define GET_LE64(p)    le64toh(PU64(p))

#define PUT_BE16(p, v) PU16(p) = htobe16(v)
#define PUT_BE32(p, v) PU32(p) = htobe32(v)
#define PUT_BE64(p, v) PU64(p) = htobe64(v)

#define PUT_LE16(p, v) PU16(p) = htole16(v)
#define PUT_LE32(p, v) PU32(p) = htole32(v)
#define PUT_LE64(p, v) PU64(p) = htole64(v)

// clang-format off
// NOTE: uint8_t* p = (uint8_t*)buf;
#define POP_BE8(p, v)   v = *p; ++p
#define POP_BE16(p, v)  v = be16toh(PU16(p)); p += 2
#define POP_BE32(p, v)  v = be32toh(PU32(p)); p += 4
#define POP_BE64(p, v)  v = be64toh(PU64(p)); p += 8

#define POP_LE8(p, v)   v= *p; ++p
#define POP_LE16(p, v)  v = le16toh(PU16(p)); p += 2
#define POP_LE32(p, v)  v = le32toh(PU32(p)); p += 4
#define POP_LE64(p, v)  v = le64toh(PU64(p)); p += 8

#define PUSH_BE8(p, v)  *p = v; ++p
#define PUSH_BE16(p, v) PU16(p) = htobe16(v); p += 2
#define PUSH_BE32(p, v) PU32(p) = htobe32(v); p += 4
#define PUSH_BE64(p, v) PU64(p) = htobe64(v); p += 8

#define PUSH_LE8(p, v)  *p = v; ++p
#define PUSH_LE16(p, v) PU16(p) = htole16(v); p += 2
#define PUSH_LE32(p, v) PU32(p) = htole32(v); p += 4
#define PUSH_LE64(p, v) PU64(p) = htole64(v); p += 8

// NOTE: NET_ENDIAN = BIG_ENDIAN
#define POP8(p, v)      POP_BE8(p, v)
#define POP16(p, v)     POP_BE16(p, v)
#define POP32(p, v)     POP_BE32(p, v)
#define POP64(p, v)     POP_BE64(p, v)
#define POP_N(p, v, n)  memcpy(v, p, n); p += n

#define PUSH8(p, v)     PUSH_BE8(p, v)
#define PUSH16(p, v)    PUSH_BE16(p, v)
#define PUSH32(p, v)    PUSH_BE32(p, v)
#define PUSH64(p, v)    PUSH_BE64(p, v)
#define PUSH_N(p, v, n) memcpy(p, v, n); p += n
// clang-format on

static inline int detect_endian() {
    union {
        char c;
        short s;
    } u;
    u.s = 0x1122;
    return u.c == 0x11 ? BIG_ENDIAN : LITTLE_ENDIAN;
}

/*---------------------------------------------------------------------------
                            Flag manipulation
----------------------------------------------------------------------------*/
#define CHECK_FLAG(V, F)   ((V) & (F))
#define SET_FLAG(V, F)     (V) |= (F)
#define UNSET_FLAG(V, F)   (V) &= ~(F)
#define RESET_FLAG(V)      (V) = 0
#define COND_FLAG(V, F, C) ((C) ? (SET_FLAG(V, F)) : (UNSET_FLAG(V, F)))

/*---------------------------------------------------------------------------
                            Math
----------------------------------------------------------------------------*/
static inline unsigned long floor2e(unsigned long num) {
    unsigned long n = num;
    int e = 0;
    while (n >>= 1)
        ++e;
    unsigned long ret = 1;
    while (e--)
        ret <<= 1;
    return ret;
}

static inline unsigned long ceil2e(unsigned long num) {
    // 2**0 = 1
    if (num == 0 || num == 1)
        return 1;
    unsigned long n = num - 1;
    int e = 1;
    while (n >>= 1)
        ++e;
    unsigned long ret = 1;
    while (e--)
        ret <<= 1;
    return ret;
}

// varint little-endian
// MSB
static inline int varint_encode(long long value, unsigned char* buf) {
    unsigned char ch;
    unsigned char* p = buf;
    int bytes = 0;
    do {
        ch = value & 0x7F;
        value >>= 7;
        *p++ = value == 0 ? ch : (ch | 0x80);
        ++bytes;
    } while (value);
    return bytes;
}

// @param[IN|OUT] len: in=>buflen, out=>varint bytesize
static inline long long varint_decode(const unsigned char* buf, int* len) {
    long long ret = 0;
    int bytes = 0, bits = 0;
    const unsigned char* p = buf;
    do {
        if (len && *len && bytes == *len) {
            // Not enough length
            *len = 0;
            return 0;
        }
        ret |= ((long long)(*p & 0x7F)) << bits;
        ++bytes;
        if ((*p & 0x80) == 0) {
            // Found end
            if (len)
                *len = bytes;
            return ret;
        }
        ++p;
        bits += 7;
    } while (bytes < 10);

    // Not found end
    if (len)
        *len = -1;
    return ret;
}

/*---------------------------------------------------------------------------
                            ASCII charaters related
----------------------------------------------------------------------------*/
// [0, 0x20)    control-charaters
// [0x20, 0x7F) printable-charaters
// 0x0A => LF
// 0x0D => CR
// 0x20 => SPACE
// 0x7F => DEL
// [0x09, 0x0D] => \t\n\v\f\r
// [0x30, 0x39] => 0~9
// [0x41, 0x5A] => A~Z
// [0x61, 0x7A] => a~z

#define IS_ALPHA(c)     (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_DIGIT(c)     ((c) >= '0' && (c) <= '9')
#define IS_ALPHANUM(c)  (IS_ALPHA(c) || IS_DIGIT(c))
#define IS_CNTRL(c)     ((c) >= 0 && (c) < 0x20)
#define IS_GRAPH(c)     ((c) >= 0x20 && (c) < 0x7F)
#define IS_HEX(c)       (IS_DIGIT(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IS_LOWER(c)     (((c) >= 'a' && (c) <= 'z'))
#define IS_UPPER(c)     (((c) >= 'A' && (c) <= 'Z'))
#define LOWER(c)        ((c) | 0x20)
#define UPPER(c)        ((c) & ~0x20)

#define CR              '\r'
#define LF              '\n'
#define CRLF            "\r\n"

/*---------------------------------------------------------------------------
                            Color
----------------------------------------------------------------------------*/
// Attributes
#define attr_normal     "0" // 正常
#define attr_bold       "1" // 加粗/高亮
#define attr_underlined "4" // 下划线
#define attr_blinking   "5" // 闪烁
#define attr_reversed   "7" // 反转
#define attr_concealed  "8" // 隐藏

// Foreground color
// Normal
#define fg_black        "30"
#define fg_red          "31"
#define fg_green        "32"
#define fg_yellow       "33"
#define fg_blue         "34"
#define fg_purple       "35"
#define fg_cyan         "36"
#define fg_grey         "37"
// Lighter
#define fg_dark_grey    "90"
#define fg_light_red    "91"
#define fg_light_green  "92"
#define fg_light_yellow "93"
#define fg_light_blue   "94"
#define fg_light_purple "95"
#define fg_light_cyan   "96"
#define fg_light_grey   "97"

// Background color
// Normal
#define bg_black        "40"
#define bg_red          "41"
#define bg_green        "42"
#define bg_yellow       "43"
#define bg_blue         "44"
#define bg_purple       "45"
#define bg_cyan         "46"
#define bg_grey         "47"
// Lighter
#define bg_dark_grey    "100"
#define bg_light_red    "101"
#define bg_light_green  "102"
#define bg_light_yellow "103"
#define bg_light_blue   "104"
#define bg_light_purple "105"
#define bg_light_cyan   "106"
#define bg_light_grey   "107"

// Easy use
#define NORMAL          "\x1b[0m"
#define BLACK           "\x1b[" attr_normal ";" fg_black "m"
#define RED             "\x1b[" attr_normal ";" fg_red "m"
#define GREEN           "\x1b[" attr_normal ";" fg_green "m"
#define YELLOW          "\x1b[" attr_normal ";" fg_yellow "m"
#define BLUE            "\x1b[" attr_normal ";" fg_blue "m"
#define PURPLE          "\x1b[" attr_normal ";" fg_purple "m"
#define CYAN            "\x1b[" attr_normal ";" fg_cyan "m"
#define GREY            "\x1b[" attr_normal ";" fg_grey "m"
#define L_DKGREY        "\x1b[" attr_normal ";" fg_dark_grey "m"
#define L_RED           "\x1b[" attr_normal ";" fg_light_red "m"
#define L_GREEN         "\x1b[" attr_normal ";" fg_light_green "m"
#define L_YELLOW        "\x1b[" attr_normal ";" fg_light_yellow "m"
#define L_BLUE          "\x1b[" attr_normal ";" fg_light_blue "m"
#define L_PURPLE        "\x1b[" attr_normal ";" fg_light_purple "m"
#define L_CYAN          "\x1b[" attr_normal ";" fg_light_cyan "m"
#define L_GREY          "\x1b[" attr_normal ";" fg_light_grey "m"
#define B_DKGREY        "\x1b[" attr_bold ";" fg_dark_grey "m"
#define B_RED           "\x1b[" attr_bold ";" fg_red "m"
#define B_GREEN         "\x1b[" attr_bold ";" fg_green "m"
#define B_ORANGE        "\x1b[" attr_bold ";" fg_yellow "m"
#define B_BLUE          "\x1b[" attr_bold ";" fg_blue "m"
#define B_PURPLE        "\x1b[" attr_bold ";" fg_purple "m"
#define B_CYAN          "\x1b[" attr_bold ";" fg_cyan "m"
#define B_GREY          "\x1b[" attr_bold ";" fg_grey "m"

/*---------------------------------------------------------------------------
                            DEBUG
----------------------------------------------------------------------------*/
#ifdef NODEBUG
#define printd(...)
#else
// #define printd(...) printf(__VA_ARGS__)
// fprintf(stdout, GRAY "%s %d] " NOCOLOR fmt, __FILE__, __LINE__, ##__VA_ARGS__);
// fprintf(stdout, L_DKGREY "[%s:%d:%s] ", __FILE__, __LINE__, __FUNCTION__);
#define printd(fmt, ...) fprintf(stdout, L_DKGREY "[%s:%d] " NORMAL fmt CRLF, __FILE__, __LINE__, ##__VA_ARGS__);
#endif

#define printe(fmt, ...)                                                                             \
    do {                                                                                             \
        fprintf(stderr, L_DKGREY "[%s:%d] " RED fmt NORMAL CRLF, __FILE__, __LINE__, ##__VA_ARGS__); \
        fflush(stderr);                                                                              \
    } while (0)

/*---------------------------------------------------------------------------
                            Types
----------------------------------------------------------------------------*/
/* Most functions in this codebase return one of these two values to let the
 * caller know whether there was a problem.
 */
typedef enum status_e {
    STATUS_OK = 0,
    STATUS_ERR = -1,
    STATUS_WARN = -2,   /* a non-fatal error or warning */
    STATUS_TIMEOUT = -3 /* timeout sniffing outbound packet */
} status_t;

/* Interface index type. */
typedef signed int ifindex_t;

/* VRF ID type. */
typedef uint32_t vrf_id_t;

typedef float float32_t;
typedef double float64_t;

typedef int (*method_t)(void* userdata);
typedef void (*procedure_t)(void* userdata);

/* Explicit type conversions */
// LD, LU, LLD, LLU for explicit conversion of integer
#define LD(v)  ((long)(v))
#define LU(v)  ((unsigned long)(v))
#define LLD(v) ((long long)(v))
#define LLU(v) ((unsigned long long)(v))

/*---------------------------------------------------------------------------
                            Modules
----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                            tools
----------------------------------------------------------------------------*/
// Generates a random in of range [min, max]
int rand_int(int min, int max);
// Generates a random string of len in heap if buf is NULL, otherwise in buf.
char* rand_str(char* buf, int len);
#define rand_double drand48
int get_proc_name(char* name, size_t len);

/*---------------------------------------------------------------------------
                            version
----------------------------------------------------------------------------*/
#define DATE_YEAR  ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 + (__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))

#define DATE_MONTH (__DATE__[2] == 'n'   ? (__DATE__[1] == 'a' ? 1 : 6) \
                    : __DATE__[2] == 'b' ? 2                            \
                    : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 3 : 4) \
                    : __DATE__[2] == 'y' ? 5                            \
                    : __DATE__[2] == 'l' ? 7                            \
                    : __DATE__[2] == 'g' ? 8                            \
                    : __DATE__[2] == 'p' ? 9                            \
                    : __DATE__[2] == 't' ? 10                           \
                    : __DATE__[2] == 'v' ? 11                           \
                                         : 12)

#define DATE_DAY        ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 + (__DATE__[5] - '0'))

#define DATE_AS_INT     (((YEAR - 2000) * 12 + MONTH) * 31 + DAY)

#define TIME_HOUR       ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0'))

#define TIME_MINUTE     ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0'))

#define TIME_SECOND     ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'))

#define VERSION_COMPILE __DATE__ " "__TIME__

static inline char* version_compile() {
    char* cv = calloc(1, 20);
    snprintf(cv, 20, "%4d-%02d-%02d %02d:%02d:%02d", DATE_YEAR, DATE_MONTH, DATE_DAY,
             TIME_HOUR, TIME_MINUTE, TIME_SECOND);
    return cv;
}

// 1.2.3.4 => 0x01020304
int version_atoi(const char* str);

// 0x01020304 => 1.2.3.4
void version_itoa(int hex, char* str);

/*---------------------------------------------------------------------------
                            thread wrapper
----------------------------------------------------------------------------*/

static inline void timespec_after(struct timespec* ts, unsigned int ms) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts->tv_sec = tv.tv_sec + ms / 1000;
    ts->tv_nsec = tv.tv_usec * 1000 + ms % 1000 * 1000000;
    if (ts->tv_nsec >= 1000000000) {
        ts->tv_nsec -= 1000000000;
        ts->tv_sec += 1;
    }
}

/* ---------------------------- pthread ---------------------------- */
typedef pthread_t thread_t;
typedef void* (*thread_routine)(void*);

#define thread_id     pthread_self       // obtain ID of the calling thread
#define gettid        (long)pthread_self // obtain ID of the calling thread in proc style
#define thread_equal  pthread_equal      // compare thread IDs
#define thread_exit   pthread_exit       // terminate calling thread
#define thread_join   pthread_join       // join with a terminated thread
#define thread_cancel pthread_cancel     // send a cancellation request to a thread
#define thread_atexit_push \
    pthread_cleanup_push // push thread cancellation clean-up handlers
#define thread_atexit_pop \
    pthread_cleanup_pop // pop thread cancellation clean-up handlers

#define THREAD_ROUTINE(fname) void* fname(void* userdata)
static inline thread_t thread_create(thread_routine fn, void* userdata) {
    thread_t th;
    pthread_create(&th, NULL, fn, userdata);
    return th;
}

/* ---------------------------- pthread mutex ---------------------------- */
#define mutex_t            pthread_mutex_t
#define mutex_init(pmutex) pthread_mutex_init(pmutex, NULL)
// make recursive mutex
#define recursive_mutex_init(pmutex)                               \
    do {                                                           \
        pthread_mutexattr_t attr;                                  \
        pthread_mutexattr_init(&attr);                             \
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); \
        pthread_mutex_init(pmutex, &attr);                         \
    } while (0)
#define mutex_destroy pthread_mutex_destroy
#define mutex_lock    pthread_mutex_lock
#define mutex_trylock pthread_mutex_trylock
static inline int thread_mutex_lock_for(pthread_mutex_t* mutex,
                                        unsigned int ms) {
    struct timespec ts;
    timespec_after(&ts, ms);
    return pthread_mutex_timedlock(mutex, &ts) != ETIMEDOUT;
}
// lock for t ms
#define mutex_timedlock(pmutex, ms) thread_mutex_lock_for(pmutex, ms)
#define mutex_unlock                pthread_mutex_unlock

/* ---------------------------- pthread recursive mutex ---------------------------- */
#define recursive_mutex_t           pthread_mutex_t
#define recursive_mutex_init(pmutex)                               \
    do {                                                           \
        pthread_mutexattr_t attr;                                  \
        pthread_mutexattr_init(&attr);                             \
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); \
        pthread_mutex_init(pmutex, &attr);                         \
    } while (0)
#define recursive_mutex_destroy     pthread_mutex_destroy
#define recursive_mutex_lock        pthread_mutex_lock
#define recursive_mutex_trylock     pthread_mutex_trylock
#define recursive_mutex_unlock      pthread_mutex_unlock

/* ---------------------------- pthread rwlock ---------------------------- */
#define rwlock_t                    pthread_rwlock_t
#define rwlock_init(prwlock)        pthread_rwlock_init(prwlock, NULL)
#define rwlock_destroy              pthread_rwlock_destroy
#define rwlock_rdlock               pthread_rwlock_rdlock
#define rwlock_tryrdlock            pthread_rwlock_tryrdlock
#define rwlock_rdunlock             pthread_rwlock_unlock
#define rwlock_wrlock               pthread_rwlock_wrlock
#define rwlock_trywrlock            pthread_rwlock_trywrlock
#define rwlock_wrunlock             pthread_rwlock_unlock

/* ---------------------------- pthread spin ---------------------------- */
#define spinlock_t                  pthread_spinlock_t
#define spinlock_init(pspin)        pthread_spin_init(pspin, PTHREAD_PROCESS_PRIVATE)
#define spinlock_destroy            pthread_spin_destroy
#define spinlock_lock               pthread_spin_lock
#define spinlock_trylock            pthread_spin_trylock
#define spinlock_unlock             pthread_spin_unlock

/* ---------------------------- pthread barrier ---------------------------- */
#define barrier_t                   pthread_barrier_t;
#define barrier_init(pbarrier, cnt) pthread_barrier_init(pbarrier, NULL, cnt)
#define barrier_destroy             pthread_barrier_destroy
#define barrier_wait                pthread_barrier_wait

/* ---------------------------- pthread condvar ---------------------------- */
// condvar is a synchronizer
#define condvar_t                   pthread_cond_t
#define condvar_init(pcond)         pthread_cond_init(pcond, NULL)
#define condvar_destroy             pthread_cond_destroy
#define condvar_wait                pthread_cond_wait
#define condvar_signal              pthread_cond_signal
#define condvar_broadcast           pthread_cond_broadcast
// true:  OK
// false: ETIMEDOUT
static inline int condvar_wait_for(condvar_t* cond, mutex_t* mutex,
                                   unsigned int ms) {
    struct timespec ts;
    timespec_after(&ts, ms);
    return pthread_cond_timedwait(cond, mutex, &ts) != ETIMEDOUT;
}

/* ---------------------------- pthread once ---------------------------- */
#define thread_once_t    pthread_once_t
#define THREAD_ONCE_INIT PTHREAD_ONCE_INIT
#define thread_once      pthread_once // dynamic package initialization

/* ---------------------------- semaphore ---------------------------- */
#include <semaphore.h>
#define sem_t                 sem_t // actually a counter
// sem_init(psem, 1) == mutex
#define sem_init(psem, value) sem_init(psem, PTHREAD_PROCESS_PRIVATE, value)
#define sem_destroy           sem_destroy
// if counter > 0, return immediately and counter - 1; else block current thread
#define sem_wait              sem_wait
// counter + 1
#define sem_post              sem_post
// true:  OK
// false: ETIMEDOUT
static inline int sem_wait_for(sem_t* sem, unsigned int ms) {
    struct timespec ts;
    timespec_after(&ts, ms);
    return sem_timedwait(sem, &ts) != ETIMEDOUT;
}

/*---------------------------------------------------------------------------
                            File and Path
----------------------------------------------------------------------------*/
#define strrchr_dot(str) strrchr(str, '.')
// /dir1/dir2/file.txt -> /file.txt
char* strrchr_dir(const char* filepath);
// /dir1/dir2/file.txt -> file.txt
const char* file_basename(const char* filepath);
// /dir1/dir2/file.txt -> txt
const char* file_suffixname(const char* filename);
// => mkdir -p dir
int mkdir_p(const char* dir);

char* get_executable_path(char* buf, int size);
char* get_executable_dir(char* buf, int size);
char* get_executable_file(char* buf, int size);
char* get_run_dir(char* buf, int size);
#ifdef __cplusplus
}
#endif

#endif
