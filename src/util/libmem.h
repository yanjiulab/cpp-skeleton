
#ifndef LIBMEM_H
#define LIBMEM_H

// predefined here
#define HAVE_MALLOC_H           1
#define HAVE_MALLOC_USABLE_SIZE 1
#define HAVE_MALLINFO           1
// #define HAVE_MALLINFO2          1

#define LIBMEM_VERSION "0.0.1"

#include "libposix.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
            X lib - application level memory type statistics
----------------------------------------------------------------------------*/

/* Human friendly string for given byte count */
#define MTYPE_MEMSTR_LEN 20

#define SIZE_VAR         ~0UL
struct memtype {
    struct memtype *next, **ref;
    const char* name;
    atomic_size_t n_alloc;
    atomic_size_t n_max;
    atomic_size_t size;
#if HAVE_MALLOC_USABLE_SIZE
    atomic_size_t total;
    atomic_size_t max_size;
#endif
};

struct memgroup {
    struct memgroup *next, **ref;
    struct memtype *types, **insert;
    const char* name;
    /* ignore group on dumping memleaks at exit */
    bool active_at_exit;
};

/* macro usage:
 *
 *  mydaemon.h
 *    DECLARE_MGROUP(MYDAEMON);
 *    DECLARE_MTYPE(MYDAEMON_COMMON);
 *
 *  mydaemon.c
 *    DEFINE_MGROUP(MYDAEMON, "my daemon memory");
 *    DEFINE_MTYPE(MYDAEMON, MYDAEMON_COMMON,
 *                   "this mtype is used in multiple files in mydaemon");
 *    foo = qmalloc(MTYPE_MYDAEMON_COMMON, sizeof(*foo))
 *
 *  mydaemon_io.c
 *    bar = qmalloc(MTYPE_MYDAEMON_COMMON, sizeof(*bar))
 *
 *    DEFINE_MTYPE_STATIC(MYDAEMON, MYDAEMON_IO,
 *                          "this mtype is used only in this file");
 *    baz = qmalloc(MTYPE_MYDAEMON_IO, sizeof(*baz))
 *
 *  Note:  Naming conventions (MGROUP_ and MTYPE_ prefixes are enforced
 *         by not having these as part of the macro arguments)
 *  Note:  MTYPE_* are symbols to the compiler (of type struct memtype *),
 *         but MGROUP_* aren't.
 */

#define DECLARE_MGROUP(name) extern struct memgroup _mg_##name
#define _DEFINE_MGROUP(mname, desc, ...)                                   \
    struct memgroup _mg_##mname                                            \
        __attribute__((section(".data.mgroups"))) = {                      \
            .name = desc,                                                  \
            .types = NULL,                                                 \
            .next = NULL,                                                  \
            .insert = NULL,                                                \
            .ref = NULL,                                                   \
            __VA_ARGS__};                                                  \
    static void _mginit_##mname(void) __attribute__((_CONSTRUCTOR(1000))); \
    static void _mginit_##mname(void) {                                    \
        extern struct memgroup** mg_insert;                                \
        _mg_##mname.ref = mg_insert;                                       \
        *mg_insert = &_mg_##mname;                                         \
        mg_insert = &_mg_##mname.next;                                     \
    }                                                                      \
    static void _mgfini_##mname(void) __attribute__((_DESTRUCTOR(1000)));  \
    static void _mgfini_##mname(void) {                                    \
        if (_mg_##mname.next)                                              \
            _mg_##mname.next->ref = _mg_##mname.ref;                       \
        *_mg_##mname.ref = _mg_##mname.next;                               \
    }                                                                      \
    MACRO_REQUIRE_SEMICOLON() /* end */

#define DEFINE_MGROUP(mname, desc) \
    _DEFINE_MGROUP(mname, desc, )
#define DEFINE_MGROUP_ACTIVEATEXIT(mname, desc) \
    _DEFINE_MGROUP(mname, desc, .active_at_exit = true)

#define DECLARE_MTYPE(name) \
    extern struct memtype MTYPE_##name[1] /* end */

#define DEFINE_MTYPE_ATTR(group, mname, attr, desc)                        \
    attr struct memtype MTYPE_##mname[1]                                   \
        __attribute__((section(".data.mtypes"))) = {{                      \
            .name = desc,                                                  \
            .next = NULL,                                                  \
            .n_alloc = 0,                                                  \
            .size = 0,                                                     \
            .ref = NULL,                                                   \
        }};                                                                \
    static void _mtinit_##mname(void) __attribute__((_CONSTRUCTOR(1001))); \
    static void _mtinit_##mname(void) {                                    \
        if (_mg_##group.insert == NULL)                                    \
            _mg_##group.insert = &_mg_##group.types;                       \
        MTYPE_##mname->ref = _mg_##group.insert;                           \
        *_mg_##group.insert = MTYPE_##mname;                               \
        _mg_##group.insert = &MTYPE_##mname->next;                         \
    }                                                                      \
    static void _mtfini_##mname(void) __attribute__((_DESTRUCTOR(1001)));  \
    static void _mtfini_##mname(void) {                                    \
        if (MTYPE_##mname->next)                                           \
            MTYPE_##mname->next->ref = MTYPE_##mname->ref;                 \
        *MTYPE_##mname->ref = MTYPE_##mname->next;                         \
    }                                                                      \
    MACRO_REQUIRE_SEMICOLON() /* end */

#define DEFINE_MTYPE(group, name, desc)    \
    DEFINE_MTYPE_ATTR(group, name, , desc) \
    /* end */

#define DEFINE_MTYPE_STATIC(group, name, desc)   \
    DEFINE_MTYPE_ATTR(group, name, static, desc) \
    /* end */

DECLARE_MGROUP(LIB);
DECLARE_MTYPE(TMP);

extern void* qmalloc(struct memtype* mt, size_t size)
    __attribute__((malloc, _ALLOC_SIZE(2), nonnull(1) _RET_NONNULL));
extern void* qcalloc(struct memtype* mt, size_t size)
    __attribute__((malloc, _ALLOC_SIZE(2), nonnull(1) _RET_NONNULL));
extern void* qrealloc(struct memtype* mt, void* ptr, size_t size)
    __attribute__((_ALLOC_SIZE(3), nonnull(1) _RET_NONNULL));
extern void* qstrdup(struct memtype* mt, const char* str)
    __attribute__((malloc, nonnull(1) _RET_NONNULL));
extern void qcountfree(struct memtype* mt, void* ptr)
    __attribute__((nonnull(1)));
extern void qfree(struct memtype* mt, void* ptr) __attribute__((nonnull(1)));

#define XMALLOC(mtype, size)       qmalloc(mtype, size)
#define XCALLOC(mtype, size)       qcalloc(mtype, size)
#define XREALLOC(mtype, ptr, size) qrealloc(mtype, ptr, size)
#define XSTRDUP(mtype, str)        qstrdup(mtype, str)
#define XCOUNTFREE(mtype, ptr)     qcountfree(mtype, ptr)
#define XFREE(mtype, ptr)  \
    do {                   \
        qfree(mtype, ptr); \
        ptr = NULL;        \
    } while (0)

static inline size_t mtype_stats_alloc(struct memtype* mt) {
    return mt->n_alloc;
}

/* NB: calls are ordered by memgroup; and there is a call with mt == NULL for
 * each memgroup (so that a header can be printed, and empty memgroups show)
 *
 * return value: 0: continue, !0: abort walk.  mem_walk will return the
 * last value from mem_walk_fn. */
typedef int mem_walk_fn(void* arg, struct memgroup* mg, struct memtype* mt);
extern int mem_walk(mem_walk_fn* func, void* arg);
extern int mem_stats_dump(FILE* fp);
#define mem_stats_dump_stderr() mem_stats_dump(stderr)
const char* mtype_memstr(char* buf, size_t len, unsigned long bytes);
int show_memory_mallinfo();

/*---------------------------------------------------------------------------
                            (deprecated) mtrace - malloc tracing
----------------------------------------------------------------------------*/
// mtrace [option]... [binary] mtracedata

/**
 * @brief set MALLOC_TRACE to "memleaks.log" before calling mtrace()
 */
// void mtrace_enable();

/**
 * @brief simply call muntrace()
 */
// void mtrace_disable();

/*---------------------------------------------------------------------------
                    (deprecated) mt_xxx - application level memory tracing
----------------------------------------------------------------------------*/
// #define mt_malloc(sz)       _mt_alloc(sz, 0, __FILE__, __LINE__)
// #define mt_calloc(num, sz)  _mt_alloc((num) * (sz), 1, __FILE__, __LINE__)
// #define mt_realloc(ptr, sz) _mt_realloc(ptr, sz, __FILE__, __LINE__)
// #define mt_size(ptr)        _mt_size(ptr, __FILE__, __LINE__)
// #define mt_strdup(str)      _mt_strdup(str, __FILE__, __LINE__)
// #define mt_free(ptr)
//     do {
//         _mt_free(ptr, __FILE__, __LINE__);
//         ptr = NULL;
//     } while (0)

// void* _mt_alloc(size_t, int, const char*, unsigned);
// void* _mt_realloc(void*, size_t, const char*, unsigned);
// char* _mt_strdup(const char* str, const char*, unsigned);
// void _mt_free(void*, const char*, unsigned);
// size_t _mt_size(void*, const char*, unsigned);
// size_t mt_usage(void);
// void mt_dump(FILE*);
// int mt_has(void* ptr);

#ifdef __cplusplus
}
#endif
#endif
