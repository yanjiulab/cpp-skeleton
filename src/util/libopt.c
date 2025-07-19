
#include "libopt.h"
#include <stdio.h>
#include <stdlib.h>

extern char** environ;

opt_t opt_info;
int opt_errors = 0;

// Option string, help string and long options.
struct optspec {
    const char* optstr;
    const char* helpstr;
    const struct option* longopts;
};

// Containers of all options (built-in and user custom)
static char comb_optstr[256];
static struct option comb_lo[OPTION_MAX];         // max 64 options
static struct option* comb_next_lo = &comb_lo[0]; // option ptr
static char comb_helpstr[4096];

// default options
static const struct option lo_always[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL}};
static const struct optspec os_always = {
    OPTION_ALWAYS,
    "  -h, --help           Display this help and exit\n"
    "  -v, --version        Print program version\n",
    lo_always};

static int opt_default_handler(int opt) {
    // struct option_chain* oc;
    switch (opt) {
    case 'h':
        opt_help_exit(0, NULL);
    case 'v':
        opt_version_exit(NULL, NULL);
    case ':':
        // fprintf(stderr, "Option '%s' requires an argument\n", opt_info.os_argv[optind - 1]);
        opt_errors++;
        break;
    case '?':
        // fprintf(stderr, "Invalid option '%s'\n", opt_info.os_argv[optind - 1]);
        opt_errors++;
        break;
    default:
        return 1;
    }
    return 0;
}

static void opt_extend(const struct optspec* os) {
    const struct option* lo;

    strncat(comb_optstr, os->optstr, sizeof(comb_optstr));
    strncat(comb_helpstr, os->helpstr, sizeof(comb_helpstr));
    for (lo = os->longopts; lo->name; lo++)
        memcpy(comb_next_lo++, lo, sizeof(*lo));
}

static int opt_len() {
    int len = 0;
    struct option* li = comb_lo;
    for (li = comb_lo; li->name; li++) {
        len++;
    }
    return len;
}

/*---------------------------------------------------------------------------
                            Common API
----------------------------------------------------------------------------*/

void opt_init(int argc, char** argv) {

    // Save command option
    int i = 0;
    opt_info.os_argv = argv;
    opt_info.argc = 0;
    opt_info.arg_len = 0;
    for (i = 0; argv[i]; ++i) {
        opt_info.arg_len += strlen(argv[i]) + 1;
    }
    opt_info.argc = i;
    char* argp = NULL;
    argp = calloc(1, opt_info.arg_len);
    opt_info.save_argv = calloc((opt_info.argc + 1), sizeof(char*));
    char* cmdline = NULL;
    cmdline = calloc(1, opt_info.arg_len);
    opt_info.cmdline = cmdline;
    for (i = 0; argv[i]; ++i) {
        strcpy(argp, argv[i]);
        opt_info.save_argv[i] = argp;
        argp += strlen(argv[i]) + 1;

        strcpy(cmdline, argv[i]);
        cmdline += strlen(argv[i]);
        *cmdline = ' ';
        ++cmdline;
    }
    opt_info.save_argv[opt_info.argc] = NULL;
    opt_info.cmdline[opt_info.arg_len - 1] = '\0';

#if defined(OS_WIN) || defined(OS_LINUX) || defined(OS_DARWIN)
    // Save env
    opt_info.os_envp = environ;
    opt_info.envc = 0;
    opt_info.env_len = 0;
    for (i = 0; environ[i]; ++i) {
        opt_info.env_len += strlen(environ[i]) + 1;
    }
    opt_info.envc = i;
    char* envp = NULL;
    envp = calloc(1, opt_info.env_len);
    opt_info.save_envp = calloc(opt_info.envc + 1, sizeof(char*));
    for (i = 0; environ[i]; ++i) {
        opt_info.save_envp[i] = envp;
        strcpy(opt_info.save_envp[i], environ[i]);
        envp += strlen(environ[i]) + 1;
    }
    opt_info.save_envp[opt_info.envc] = NULL;
#endif
    // Stream style: Register default option callback funcitons
    opt_extend(&os_always);
    opt_set_handler(opt_default_handler);
}

void opt_add(const char* optstr, const struct option* longopts, const char* helpstr) {
    const struct optspec main_opts = {optstr, helpstr, longopts};
    opt_extend(&main_opts);
}

void opt_free() {
    if (opt_info.save_argv) {
        free(opt_info.save_argv[0]);
        free(opt_info.save_argv);
    }
    free(opt_info.cmdline);
    if (opt_info.save_envp) {
        free(opt_info.save_envp[0]);
        free(opt_info.save_envp);
    }
    if (opt_info.arg_kv) {
        for (int i = 0; i < opt_info.arg_kv_size; ++i) {
            free(opt_info.arg_kv[i]);
        }
        free(opt_info.arg_kv);
    }
    if (opt_info.arg_list) {
        for (int i = 0; i < opt_info.arg_list_size; ++i) {
            free(opt_info.arg_list[i]);
        }
        free(opt_info.arg_list);
    }
}

void opt_help_exit(int status, char* progname) {

    FILE* target = status ? stderr : stdout;

    if (status != 0)
        fprintf(stderr, "Invalid options.\n\n");

    if (!progname) {
        char* idx = strrchr(opt_info.os_argv[0], '/');
        if (idx) progname = idx + 1;
    }

    fprintf(target, "Usage: %s [OPTION...]\n\n%s",
            progname, comb_helpstr);
    exit(status);
}

void opt_version_exit(char* progname, char* copyright) {
    if (!progname) {
        char* idx = strrchr(opt_info.os_argv[0], '/');
        if (idx) progname = idx + 1;
    }
    fprintf(stdout, "%s version %s (Compile time: %s %s)\n",
            progname, LIBOPT_VERSION, __DATE__, __TIME__);

    if (copyright)
        fprintf(stdout, "%s\n", copyright);
    exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------
                            Stream-based style API
----------------------------------------------------------------------------*/

void opt_set_handler(int (*cb)(int)) {
    opt_info.opt_default_cb = cb;
}

int opt_getopt(int argc, char* const argv[]) {

    int opt;
    comb_next_lo->name = NULL;
    
    do {
        opt = getopt_long(argc, argv, comb_optstr, comb_lo, NULL);
        if (opt_info.opt_default_cb && opt_info.opt_default_cb(opt))
            break;
    } while (opt != -1);

    if (opt == -1 && opt_errors)
        opt_help_exit(1, NULL);

    return opt;
}

/*---------------------------------------------------------------------------
                            Key-Value pair style API
----------------------------------------------------------------------------*/

static void init_arg_kv(int maxsize) {
    opt_info.arg_kv_size = 0;
    opt_info.arg_kv = calloc(maxsize + 1, sizeof(char*));
}

static void save_arg_kv(const char* key, int key_len, const char* val, int val_len) {
    if (key_len <= 0) key_len = strlen(key);
    if (val_len <= 0) val_len = strlen(val);
    char* arg = NULL;
    arg = calloc(1, key_len + val_len + 2);
    memcpy(arg, key, key_len);
    arg[key_len] = '=';
    memcpy(arg + key_len + 1, val, val_len);
    // printf("save_arg_kv: %s\n", arg);
    opt_info.arg_kv[opt_info.arg_kv_size++] = arg;
}

static void save_arg_kv_short(const char key, const char* val, int val_len) {
    if (val_len <= 0) val_len = strlen(val);
    char* arg = NULL;
    arg = calloc(1, 1 + val_len + 2);
    sprintf(arg, "%c", key);
    arg[1] = '=';
    memcpy(arg + 1 + 1, val, val_len);
    // printf("save_arg_kv: %s\n", arg);
    opt_info.arg_kv[opt_info.arg_kv_size++] = arg;
}

static void init_arg_list(int maxsize) {
    opt_info.arg_list_size = 0;
    opt_info.arg_list = calloc(maxsize, sizeof(char*));
}

static void save_arg_list(const char* arg) {
    // printf("save_arg_list: %s\n", arg);
    opt_info.arg_list[opt_info.arg_list_size++] = strdup(arg);
}

// #define UNDEFINED_OPTION -1
// static int get_arg_type(int short_opt, const char* options) {
//     if (options == NULL) return UNDEFINED_OPTION;
//     const char* p = options;
//     while (*p && *p != short_opt)
//         ++p;
//     if (*p == '\0') return UNDEFINED_OPTION;
//     if (*(p + 1) == ':') return REQUIRED_ARGUMENT;
//     return NO_ARGUMENT;
// }

static const struct option* get_option(const char* opt) {
    if (opt == NULL) return NULL;
    int len = strlen(opt);
    if (len == 0) return NULL;

    struct option* li = comb_lo;
    if (len == 1) {
        for (li = comb_lo; li->name; li++) {
            if (li->val == *opt) return li;
        }
    } else {
        for (li = comb_lo; li->name; li++) {
            if (strncmp(li->name, opt, len) == 0)
                return li;
        }
    }

    return NULL;
}

static const char* get_val(char** kvs, const char* key) {
    if (kvs == NULL) return NULL;
    int key_len = strlen(key);
    char* kv = NULL;
    int kv_len = 0;
    for (int i = 0; kvs[i]; ++i) {
        kv = kvs[i];
        kv_len = strlen(kv);
        if (kv_len <= key_len) continue;
        // key=val and val != \0
        if (memcmp(kv, key, key_len) == 0 && kv[key_len] == '=') {
            if (*(kv + key_len + 1))
                return kv + key_len + 1; // return value
            else
                return NULL; // "" return NULL
        }
    }
    return NULL;
}

int opt_parse(int argc, char** argv) {
    // const struct option* long_options = &comb_lo[0];

    int size = opt_len();
    init_arg_kv(size + 1);
    init_arg_list(argc);

    if (argc < 1) return 0;

    char opt[OPTION_MAX + 1] = {0};

    for (int i = 1; argv[i]; ++i) {
        char* arg = argv[i];
        int opt_type = NOPREFIX_OPTION;
        // prefix
        if (*arg == OPTION_PREFIX) {
            ++arg;
            opt_type = SHORT_OPTION;
            if (*arg == OPTION_PREFIX) {
                ++arg;
                opt_type = LONG_OPTION;
            }
        }
        int arg_len = strlen(arg);
        // delim
        char* delim = strchr(arg, OPTION_DELIM);
        if (delim) {
            if (delim == arg || delim == arg + arg_len - 1 || delim - arg > OPTION_MAX) {
                printf("Invalid option '%s'\n", argv[i]);
                return -10;
            }
            memcpy(opt, arg, delim - arg);
            opt[delim - arg] = '\0';
        } else {
            if (opt_type == SHORT_OPTION) {
                *opt = *arg;
                opt[1] = '\0';
            } else {
                strncpy(opt, arg, OPTION_MAX);
            }
        }

        // get_option
        const struct option* pOption = get_option(opt);
        if (pOption == NULL) {
            if (delim == NULL && opt_type == NOPREFIX_OPTION) {
                save_arg_list(arg);
                continue;
            } else {
                fprintf(stderr, "Invalid option: '%s'\n", argv[i]);
                return -10;
            }
        }
        const char* value = NULL;
        if (pOption->has_arg == no_argument) {
            // -h
            value = OPTION_ENABLE;
        } else if (pOption->has_arg == required_argument) {
            if (delim) {
                // --port=80
                value = delim + 1;
            } else {
                if (opt_type == SHORT_OPTION && *(arg + 1) != '\0') {
                    // p80
                    value = arg + 1;
                } else if (argv[i + 1] != NULL) {
                    // --port 80
                    value = argv[++i];
                } else {
                    printf("Option '%s' requires an argument\n", opt);
                    return -20;
                }
            }
        } else {
            if (opt_type == SHORT_OPTION) {
                if (*(arg + 1) != '\0') {
                    value = arg + 1; // -p80
                } else {
                    value = ""; // -p
                }
            } else {
                if (delim && *(delim + 1) != '\0')
                    value = delim + 1; // --port=80
                else
                    value = ""; // --port
            }
        }

        // preferred to use short_opt as key
        if (pOption->val < 128) {
            save_arg_kv_short(pOption->val, value, 0);
        } else {
            save_arg_kv(pOption->name, 0, value, 0);
        }
    }
    return 0;
}

const char* opt_get(const char* key) {
    const char* val;
    val = get_val(opt_info.arg_kv, key);
    if (val) return val;
    val = getenv(key); // get_val(opt_info.save_envp, key);
    if (val) return val;
    return NULL;
}