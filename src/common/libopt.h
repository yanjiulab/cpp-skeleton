
/**
 * Stream-based method parses all input parameters while reading it, and uses event callbacks
 * to fetch data for the user. It takes up very little memory.
 *
 * Key-value pair-based method reads all input parameters at one time, parses them according to
 * the rules same as stream-based method, and stores the results as key-value pairs, which can
 * be used by users at any time.
 */

#ifndef LIBOPT_H
#define LIBOPT_H

#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIBOPT_VERSION "0.1.0"

#ifdef __cplusplus
extern "C" {
#endif

// Option type, same as small case
#define NO_ARGUMENT       0
#define REQUIRED_ARGUMENT 1
#define OPTIONAL_ARGUMENT 2

// Define macro
#define OPTION_PREFIX     '-'
#define OPTION_DELIM      '='
#define OPTION_ENABLE     "1"
#define OPTION_DISABLE    "0"

// Parsed option
#define OPTION_MAX        64
#define NOPREFIX_OPTION   0
#define SHORT_OPTION      -1
#define LONG_OPTION       -2

// Built-in options:
// a means -a
// a: means -a <required>
// a:: means -a[optional]
// prefix ':' means getopt will return ':' when required options lack of option
#define OPTION_ALWAYS     ":hv"

typedef struct opt {
    // callbacks
    int (*opt_default_cb)(int);

    // env
    int envc;
    int env_len;
    char** os_envp;   // os argv pointer
    char** save_envp; // saved env (deep copy in heap)

    // arg
    int argc;         // argc
    int arg_len;      // len of cmdline (including ending '\0')
    char* cmdline;    // string format of argv
    char** os_argv;   // os argv pointer
    char** save_argv; // saved argv (deep copy in heap)

    // parsed arg
    int arg_kv_size;
    char** arg_kv;
    int arg_list_size;
    char** arg_list;
} opt_t;

extern opt_t opt_info; // Global option information
extern int opt_errors; // Global option error count

/**
 * @brief Save arguments and environ, and register default argument.
 * @note Common API
 */
void opt_init(int argc, char** const argv);

/**
 * @brief Add user options
 * @param optstr option string like OPT_ALWAYS
 * @param longopts long option array like lo_always[]
 * @param helpstr option help string
 * @note Common API
 */
void opt_add(const char* optstr, const struct option* longopts, const char* helpstr);

/**
 * @brief Free opt_info
 * @note Common API
 */
void opt_free();

/**
 * @brief Print default help message and exit with status code.
 * @param status print usage if status is 0, otherwise print "Invalid options.".
 * @param progname Program name
 */
void opt_help_exit(int status, char* progname);

/**
 * @brief Print default version message and exit.
 * @param progname Program name
 * @param copyright Program copyright if need
 */
void opt_version_exit(char* progname, char* copyright);

/**
 * @brief Set handler for default options.
 * @param cb callback
 * @note Stream-based method API
 */
void opt_set_handler(int (*cb)(int));

/**
 * @brief option process
 * @param argc argc
 * @param argv argv
 * @return If an option was successfully found, then opt_getopt() returns the option character.
 * If opt_getopt() encounters an option character that was not in optstring, then '?' is returned.
 * If opt_getopt() encounters an option with a missing argument, then ':' is returned.
 * For a long option, they return val if flag is NULL, and 0 otherwise.
 * If all command-line options have been parsed, then opt_getopt() will print help message and exit.
 * @note Stream-based method API
 */
int opt_getopt(int argc, char* const argv[]);

/**
 * @brief
 * @param argc
 * @param argv
 * @return
 * short option:
 * - no argument: -f / --foo
 * - required_argument: -f conf / -fconf / --foo conf / --foo=conf
 * - optional_argument: -f / -fconf / --foo / --foo=conf
 * @note Key-value pair-based method API
 */
int opt_parse(int argc, char** argv);

/**
 * @brief Search for a key in argument dict first, and then in environ dict if the
 * key cannot be found.
 * @param key key
 * @return NULL if key is not found, otherwise value string.
 * @note Key-value pair-based method API
 */
const char* opt_get(const char* key);

#ifdef __cplusplus
}
#endif
#endif
