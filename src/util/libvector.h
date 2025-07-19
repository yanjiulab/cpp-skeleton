
#ifndef LIBVECTOR_H
#define LIBVECTOR_H

#define LIBVECTOR_VERSION "0.0.1"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* struct for vector */
struct vector {
    unsigned int active;  /* number of active slots */
    unsigned int alloced; /* number of allocated slot */
    unsigned int count;
    void** index; /* index to data */
};
typedef struct vector* vector_t;

#define VECTOR_MIN_SIZE   1

/* Reference slot at given index, caller must ensure slot is active */
#define vector_slot(V, I) ((V)->index[(I)])
#define vector_at(V, I)   ((V)->index[(I)])

#define vector_alloced(V) ((V)->alloced)
#define vector_count(V)   ((V)->count)
/* Note that this differs from vector_count() as it
 the count returned by vector_active() will include any empty slots */
// [X, X, 0, X, ...] => count: 3, active: 4
#define vector_active(V)  ((V)->active)

/* Basic operations */
extern vector_t vector_new(unsigned int size);
extern void vector_free(vector_t v);
extern void vector_clear(vector_t v);
extern void vector_ensure(vector_t v, unsigned int num);
extern int vector_empty_slot(vector_t v);
extern int vector_set(vector_t v, void* val);
extern int vector_set_index(vector_t v, unsigned int i, void* val);
extern void vector_unset(vector_t v, unsigned int i);
extern void vector_unset_value(vector_t v, void* val);
extern int vector_insert(vector_t v, unsigned int i, void* val);
extern void vector_remove(vector_t v, unsigned int ix);
extern void* vector_get(vector_t, unsigned int);
extern void* vector_get_ensure(vector_t, unsigned int);
#define vector_foreach(v, var, iter) \
    if ((v)->active > 0)             \
        for ((iter) = 0; (iter) < (v)->active && (((var) = (v)->index[(iter)]), 1); ++(iter))
#define vector_foreach_rev(v, var, iter) \
    if ((v)->active > 0)                 \
        for ((iter) = (v)->active - 1; (iter) >= 0 && (((var) = (v)->index[(iter)]), 1); --(iter))
bool vector_scan(vector_t v, bool (*iter)(const void* item, void* udata), void* udata);

/* Stack-like operations */
#define vector_first(v) vector_slot(v, 0)
#define vector_last(v)  vector_slot(v, vector_active(v) - 1)
extern void vector_push(vector_t v, void* val);
extern void* vector_pop(vector_t v);

/* Advanced operations */
void vector_swap(vector_t v, unsigned int i, unsigned int j);
void vector_reverse(vector_t v);
extern vector_t vector_copy(vector_t v);
extern void vector_compact(vector_t v);
extern void vector_to_array(vector_t v, void*** dest, int* argc);
extern vector_t array_to_vector(void** src, int argc);
int vector_ptr_cmp(const void* p1, const void* p2);
int vector_str_cmp(const void* p1, const void* p2);
int vector_int_cmp(const void* p1, const void* p2);
int vector_double_cmp(const void* p1, const void* p2);
void vector_sort(vector_t v, __compar_fn_t fn);
int vector_find(vector_t v, void* val, __compar_fn_t fn);

#ifdef __cplusplus
}
#endif

#endif
