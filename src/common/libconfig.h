#ifndef LIBCONFIG_H
#define LIBCONFIG_H

#define LIBCONFIG_VERSION "0.1.0"

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#define PATH_MAX        4096
#ifdef __cplusplus
extern "C" {
#endif

typedef struct config {
    struct config_ops* ops;
    char path[PATH_MAX];
    void* priv;
} config_t;

typedef struct config_ops {
    int (*load)(struct config* c, const char* name);
    void (*unload)(struct config* c);
    void (*dump)(struct config* c, FILE* f);
    int (*save)(struct config* c);

    char* (*get_string)(struct config* c, ...);
    int (*set_string)(struct config* c, ...);
    int (*get_int)(struct config* c, ...);
    int (*set_int)(struct config* c, ...);
    double (*get_double)(struct config* c, ...);
    int (*set_double)(struct config* c, ...);
    bool (*get_boolean)(struct config* c, ...);
    int (*set_boolean)(struct config* c, ...);

    void (*del)(struct config* c, const char* key);
} config_ops_t;

struct config* conf_load(const char* name);
void conf_unload(struct config* c);
int conf_set(struct config* c, const char* key, const char* val);
void conf_del(struct config* c, const char* key);
void conf_dump(struct config* c);
int conf_save(struct config* c);
void conf_dump_to_file(FILE* f, struct config* c);

/*
 * xxx = {
 *     yyy = {
 *         "aaa",
 *         "bbb",
 *     }
 * }
 * conf_get_type(c, "xxx", "yyy", 1) will get "aaa"
 * conf_get_type(c, "xxx", "yyy", 2) will get "bbb"
 * 0 or NULL will be recorgize end of args, must start array with 1
 */
extern struct config* g_config;
#define conf_get_int(c, ...)     g_config->ops->get_int(c, __VA_ARGS__, NULL)
#define conf_set_int(c, ...)     g_config->ops->set_int(c, __VA_ARGS__, NULL)
#define conf_get_string(c, ...)  g_config->ops->get_string(c, __VA_ARGS__, NULL)
#define conf_set_string(c, ...)  g_config->ops->set_string(c, __VA_ARGS__, NULL)
#define conf_get_double(c, ...)  g_config->ops->get_double(c, __VA_ARGS__, NULL)
#define conf_set_double(c, ...)  g_config->ops->set_double(c, __VA_ARGS__, NULL)
#define conf_get_boolean(c, ...) g_config->ops->get_boolean(c, __VA_ARGS__, NULL)
#define conf_set_boolean(c, ...) g_config->ops->set_boolean(c, __VA_ARGS__, NULL)
#define conf_get_length(c, ...)  g_config->ops->get_length(c, __VA_ARGS__, NULL)

// Utils
#define TYPE_EMPTY               0
#define TYPE_INT                 1
#define TYPE_CHARP               2

struct int_charp {
    int type;
    union {
        int ival;
        char* cval;
    };
};

/*
 * va_arg_type can get value from ap ignore type
 * firstly, try to match "char *", which must be pointer of memory in higher address
 * if the type is "int", the value force to "char *" will be the real value
 * because the int value is the index of table in conf file, and should be limited.
 * so we use MAX_CONF_ENTRY to divide the type of "int" or "char *"
 */
#define MAX_CONF_ENTRY 4096
#define va_arg_type(ap, mix)                        \
    do {                                            \
        char* __tmp = va_arg(ap, char*);            \
        if (!__tmp) {                               \
            mix.type = TYPE_EMPTY;                  \
            mix.cval = NULL;                        \
        } else if (__tmp < (char*)MAX_CONF_ENTRY) { \
            mix.type = TYPE_INT;                    \
            mix.ival = *(int*)&__tmp;               \
        } else {                                    \
            mix.type = TYPE_CHARP;                  \
            mix.cval = __tmp;                       \
        }                                           \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif
