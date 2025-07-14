#include "libmem.h"

/*---------------------------------------------------------------------------
            X lib - application level memory type statistics
----------------------------------------------------------------------------*/
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#if HAVE_MALLOC_NP_H
#include <malloc_np.h>
#endif
#if HAVE_MALLOC_MALLOC_H
#include <malloc/malloc.h>
#endif

static struct memgroup* mg_first = NULL;
struct memgroup** mg_insert = &mg_first;

DEFINE_MGROUP(LIB, "libyjc");
DEFINE_MTYPE(LIB, TMP, "Temporary memory");
DEFINE_MTYPE(LIB, BITFIELD, "Bitfield memory");

static inline void mt_count_alloc(struct memtype* mt, size_t size, void* ptr) {
    size_t current;
    size_t oldsize;

    current = 1 + atomic_fetch_add_explicit(&mt->n_alloc, 1,
                                            memory_order_relaxed);
    oldsize = atomic_load_explicit(&mt->n_max, memory_order_relaxed);

    if (current > oldsize)
        /* note that this may fail, but approximation is sufficient */
        atomic_compare_exchange_weak_explicit(&mt->n_max, &oldsize,
                                              current,
                                              memory_order_relaxed,
                                              memory_order_relaxed);

    oldsize = atomic_load_explicit(&mt->size, memory_order_relaxed);

    if (oldsize == 0)
        oldsize = atomic_exchange_explicit(&mt->size, size, memory_order_relaxed);

    if (oldsize != 0 && oldsize != size && oldsize != SIZE_VAR)
        atomic_store_explicit(&mt->size, SIZE_VAR, memory_order_relaxed);

#if HAVE_MALLOC_USABLE_SIZE
    size_t mallocsz = malloc_usable_size(ptr);

    current = mallocsz + atomic_fetch_add_explicit(&mt->total, mallocsz,
                                                   memory_order_relaxed);
    oldsize = atomic_load_explicit(&mt->max_size, memory_order_relaxed);
    if (current > oldsize)
        /* note that this may fail, but approximation is sufficient */
        atomic_compare_exchange_weak_explicit(&mt->max_size, &oldsize,
                                              current,
                                              memory_order_relaxed,
                                              memory_order_relaxed);
#endif
}

static inline void mt_count_free(struct memtype* mt, void* ptr) {
    assert(mt->n_alloc);
    atomic_fetch_sub_explicit(&mt->n_alloc, 1, memory_order_relaxed);

#if HAVE_MALLOC_USABLE_SIZE
    size_t mallocsz = malloc_usable_size(ptr);

    atomic_fetch_sub_explicit(&mt->total, mallocsz, memory_order_relaxed);
#endif
}

static inline void* mt_checkalloc(struct memtype* mt, void* ptr, size_t size) {

    if (__builtin_expect(ptr == NULL, 0)) {
        if (size) {
            /* malloc(0) is allowed to return NULL */
            abort();
        }
        return NULL;
    }
    mt_count_alloc(mt, size, ptr);
    return ptr;
}

void* qmalloc(struct memtype* mt, size_t size) {
    return mt_checkalloc(mt, malloc(size), size);
}

void* qcalloc(struct memtype* mt, size_t size) {
    return mt_checkalloc(mt, calloc(size, 1), size);
}

void* qrealloc(struct memtype* mt, void* ptr, size_t size) {
    if (ptr)
        mt_count_free(mt, ptr);
    return mt_checkalloc(mt, ptr ? realloc(ptr, size) : malloc(size), size);
}

void* qstrdup(struct memtype* mt, const char* str) {
    return str ? mt_checkalloc(mt, strdup(str), strlen(str) + 1) : NULL;
}

void qcountfree(struct memtype* mt, void* ptr) {
    if (ptr)
        mt_count_free(mt, ptr);
}

void qfree(struct memtype* mt, void* ptr) {
    if (ptr)
        mt_count_free(mt, ptr);
    free(ptr);
}

int mem_walk(mem_walk_fn* func, void* arg) {
    struct memgroup* mg;
    struct memtype* mt;
    int rv;

    for (mg = mg_first; mg; mg = mg->next) {
        if ((rv = func(arg, mg, NULL)))
            return rv;
        for (mt = mg->types; mt; mt = mt->next)
            if ((rv = func(arg, mg, mt)))
                return rv;
    }
    return 0;
}

static int mem_walker(void* arg, struct memgroup* mg, struct memtype* mt) {
    if (!mt) {
        printf("--- mem %s ---\n", mg->name);
        printf("%-30s: %8s %-8s%s %8s %9s\n",
               "Type", "Current#", "  Size",
#if HAVE_MALLOC_USABLE_SIZE
               "     Total",
#else
               "",
#endif
               "Max#",
#if HAVE_MALLOC_USABLE_SIZE
               "MaxBytes"
#else
               ""
#endif
        );
    } else {
        if (mt->n_max != 0) {
            char size[32];
            snprintf(size, sizeof(size), "%6zu", mt->size);
#if HAVE_MALLOC_USABLE_SIZE
#define TSTR  " %9zu"
#define TARG  , mt->total
#define TARG2 , mt->max_size
#else
#define TSTR ""
#define TARG
#define TARG2
#endif
            printf("%-30s: %8zu %-8s" TSTR " %8zu" TSTR "\n",
                   mt->name,
                   mt->n_alloc,
                   mt->size == 0 ? ""
                   : mt->size == SIZE_VAR
                       ? "variable"
                       : size
                             TARG,
                   mt->n_max
                       TARG2);
        }
    }
    return 0;
}

int mem_stats_dump(FILE* fp) {
    return mem_walk(mem_walker, NULL);
}

/* Stats querying from users */
/* Return a pointer to a human friendly string describing
 * the byte count passed in. E.g:
 * "0 bytes", "2048 bytes", "110kB", "500MiB", "11GiB", etc.
 * Up to 4 significant figures will be given.
 * The pointer returned may be NULL (indicating an error)
 * or point to the given buffer, or point to static storage.
 */
const char* mtype_memstr(char* buf, size_t len, unsigned long bytes) {
    unsigned int m, k;

    /* easy cases */
    if (!bytes)
        return "0 bytes";
    if (bytes == 1)
        return "1 byte";

    /*
     * When we pass the 2gb barrier mallinfo() can no longer report
     * correct data so it just does something odd...
     * Reporting like Terrabytes of data.  Which makes users...
     * edgy.. yes edgy that's the term for it.
     * So let's just give up gracefully
     */
    if (bytes > 0x7fffffff)
        return "> 2GB";

    m = bytes >> 20;
    k = bytes >> 10;

    if (m > 10) {
        if (bytes & (1 << 19))
            m++;
        snprintf(buf, len, "%d MiB", m);
    } else if (k > 10) {
        if (bytes & (1 << 9))
            k++;
        snprintf(buf, len, "%d KiB", k);
    } else
        snprintf(buf, len, "%ld bytes", bytes);

    return buf;
}

int show_memory_mallinfo() {

#if (HAVE_MALLINFO2)
    struct mallinfo2 minfo = mallinfo2();
#elif (HAVE_MALLINFO)
    struct mallinfo minfo = mallinfo();
#else
    puts("HAVE_MALLINFO or HAVE_MALLINFO2 not defined.");
    return 0;
#endif

#if (HAVE_MALLINFO2) || (HAVE_MALLINFO)
    char buf[MTYPE_MEMSTR_LEN];
    fprintf(stdout, "System allocator statistics:\n");
    fprintf(stdout, "  Total heap allocated:  %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.arena));
    fprintf(stdout, "  Holding block headers: %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.hblkhd));
    fprintf(stdout, "  Used small blocks:     %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.usmblks));
    fprintf(stdout, "  Used ordinary blocks:  %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.uordblks));
    fprintf(stdout, "  Free small blocks:     %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.fsmblks));
    fprintf(stdout, "  Free ordinary blocks:  %s\n",
            mtype_memstr(buf, MTYPE_MEMSTR_LEN, minfo.fordblks));
    fprintf(stdout, "  Ordinary blocks:       %ld\n",
            (unsigned long)minfo.ordblks);
    fprintf(stdout, "  Small blocks:          %ld\n",
            (unsigned long)minfo.smblks);
    fprintf(stdout, "  Holding blocks:        %ld\n",
            (unsigned long)minfo.hblks);
    fprintf(stdout, "(see system documentation for 'mallinfo' for meaning)\n");
    return 1;
#endif /* HAVE_MALLINFO */
}

/*---------------------------------------------------------------------------
                            (deprecated) mtrace - malloc tracing
----------------------------------------------------------------------------*/
// void mtrace_enable() {
//     setenv("MALLOC_TRACE", "memleaks.log", 1);
//     // fprintf(stdout, "setting MALLOC_TRACE=%s\n", getenv("MALLOC_TRACE"));

//     // In the version of 2.34 of the GNU C library, memory allocation debugging was moved
//     // to a separate library, which must be pre-loaded when debugging is desired.
//     // setenv("LD_PRELOAD", "/lib/$(gcc -print-multiarch)/libc_malloc_debug.so", 1);
//     // fprintf(stdout, "setting LD_PRELOAD=%s\n", getenv("LD_PRELOAD"));
//     mtrace();
// }

// void mtrace_disable() {
//     muntrace();
// }

/*---------------------------------------------------------------------------
                    mt_xxx - application level memory tracing
----------------------------------------------------------------------------*/
// #ifdef MT_STACK_TRACE
// #include <execinfo.h>
// #ifndef MT_STACK_TRACE_MAX
// #define MT_STACK_TRACE_MAX 32
// #endif
// #endif

// typedef struct mt_node_t {
//     struct mt_node_t *prev, *next;
//     const char* file;
//     size_t line;
//     size_t size;
// #ifdef MT_STACK_TRACE
//     void* stacktrace[MT_STACK_TRACE_MAX];
//     size_t stacktrace_sz;
// #endif
// } mt_node_t;

// mt_node_t* mt_head;

// int _mt_has_node(mt_node_t* n) {
//     mt_node_t* node = mt_head;
//     while (node != NULL) {
//         if (node == n) return 1;
//         node = node->next;
//     }
//     return 0;
// }

// void _mt_abort(void) {
// #ifdef MT_STACK_TRACE
//     void* array[MT_STACK_TRACE_MAX];
//     size_t sz = backtrace(array, MT_STACK_TRACE_MAX);
//     backtrace_symbols_fd(array, sz, fileno(stderr));
// #endif
//     abort();
// }

// void* _mt_alloc(size_t sz, int zeroset, const char* file, unsigned line) {
//     mt_node_t* node = NULL;

//     if (zeroset) {
//         node = calloc(sizeof(*node) + sz, 1);
//     } else {
//         node = malloc(sizeof(*node) + sz);
//         if (node != NULL) {
//             memset(node, 0, sizeof(*node));
//         }
//     }

//     if (node == NULL) {
// #ifdef MT_ABORT_NULL
//         fprintf(stderr, "Couldn't allocate: %s, line %u\n", file, line);
//         _mt_abort();
// #else
//         return NULL;
// #endif
//     }

//     node->line = line;
//     node->file = file;
//     node->size = sz;

// #ifdef MT_STACK_TRACE
//     node->stacktrace_sz = backtrace(node->stacktrace, MT_STACK_TRACE_MAX);
// #endif

//     if (mt_head) {
//         mt_head->prev = node;
//         node->next = mt_head;
//     }
//     mt_head = node;

//     return (char*)node + sizeof(*node);
// }

// void* _mt_realloc(void* ptr, size_t sz, const char* file, unsigned line) {
//     mt_node_t* node = (mt_node_t*)((char*)ptr - sizeof(*node));
//     mt_node_t* old_node = node;

//     if (ptr == NULL) return _mt_alloc(sz, 0, file, line);

// #ifndef MT_UNSAFE
//     if (!_mt_has_node(node)) {
//         fprintf(stderr, "Bad realloc: %p %s, line %u\n", ptr, file, line);
//         _mt_abort();
//     }
// #endif

//     node = realloc(node, sizeof(*node) + sz);

//     if (node == NULL) {
// #ifdef MT_ABORT_NULL
//         fprintf(stderr, "Couldn't reallocate: %s, line %u\n", file, line);
//         _mt_abort();
// #else
//         return NULL;
// #endif
//     }

//     node->size = sz;
//     if (mt_head == old_node) mt_head = node;
//     if (node->prev) node->prev->next = node;
//     if (node->next) node->next->prev = node;

//     return (char*)node + sizeof(*node);
// }

// char* _mt_strdup(const char* str, const char* file, unsigned line) {
//     if (str == NULL) return NULL;
//     char* ptr = _mt_alloc(strlen(str) + 1, 1, file, line);
//     strcpy(ptr, str);
//     return ptr;
// }

// void _mt_free(void* ptr, const char* file, unsigned line) {
//     mt_node_t* node = (mt_node_t*)((char*)ptr - sizeof(*node));

//     if (ptr == NULL) return;

// #ifndef MT_UNSAFE
//     if (!_mt_has_node(node)) {
//         fprintf(stderr, "Bad free: %p %s, line %u\n", ptr, file, line);
//         _mt_abort();
//     }
// #endif

//     if (node == mt_head) mt_head = node->next;
//     if (node->prev) node->prev->next = node->next;
//     if (node->next) node->next->prev = node->prev;

//     free(node);
// }

// void mt_dump(FILE* fp) {
//     mt_node_t* node = mt_head;
//     size_t total = 0;

//     if (!fp) fp = stdout;

//     while (node != NULL) {
//         fprintf(fp, "Unfreed: %p %s, line %lu (%lu bytes)\n",
//                 (char*)node + sizeof(*node), node->file,
//                 (unsigned long)node->line, (unsigned long)node->size);

// #ifdef MT_STACK_TRACE
//         backtrace_symbols_fd(node->stacktrace, node->stacktrace_sz, fileno(fp));
//         fprintf(fp, "\n");
// #endif

//         total += node->size;
//         node = node->next;
//     }

//     fprintf(fp, "Total unfreed: %lu bytes\n", (unsigned long)total);
// }

// size_t _mt_size(void* ptr, const char* file, unsigned line) {
//     mt_node_t* node = (mt_node_t*)((char*)ptr - sizeof(*node));

// #ifndef MT_UNSAFE
//     if (!_mt_has_node(node)) {
//         fprintf(stderr, "Bad pointer: %p %s, line %u\n", ptr, file, line);
//         _mt_abort();
//     }
// #endif

//     return node->size;
// }

// size_t mt_usage(void) {
//     mt_node_t* node = mt_head;
//     size_t total = 0;

//     while (node != NULL) {
//         total += node->size;
//         node = node->next;
//     }

//     return total;
// }

// int mt_has(void* ptr) {
//     mt_node_t* node = (mt_node_t*)((char*)ptr - sizeof(*node));
//     return _mt_has_node(node);
// }