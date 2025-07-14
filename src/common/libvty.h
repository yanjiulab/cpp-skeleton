/**
 * VTY - Virtual Terminal [aka TeletYpe] Interface is a command line interface (CLI)
 * for user interaction with the daemon.
 *
 * The implementation is inspired by:
 *   - Quagga/FRR VTY
 *   - cmdf
 *   - linenoise
 */

#ifndef LIBVTY_H
#define LIBVTY_H

#define LIBVTY_VERSION "0.0.1"

#include "linenoise.h"

#include <libvector.h>
#include <stdio.h>
#include <stdint.h>

/* Error codes (for CMDF_RETURN) */
#define CMD_OK                    0
#define CMD_ERROR_UNKNOWN_COMMAND -1
#define CMD_ERROR_ARGUMENT_ERROR  -2
#define CMD_ERROR_TOO_MANY_ARGS   -3
#define CMD_ERROR_TOO_FEW_ARGS    -4
#define CMD_ERROR_OUT_OF_MEMORY   -5

/* Flags */
#define CMD_FLAG_ASYNC            0x00000001
#define CMD_FLAG_AUTOEXIT         0x00000002

/* Definitions */
#define ASYNC_BUFFLEN             1024
#define CMD_PPRINT_RIGHT_OFFSET   1
#define CMD_FREE                  free
#define CMD_MALLOC                malloc
#define CMD_INIT_SIZE             16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vty_s vty_t;

typedef struct cmd_arglist_s {
    char** args;  /* NULL-terminated string list */
    size_t count; /* Argument list count */
} cmd_arglist_t;

typedef int (*cmd_cb)(vty_t* ctx, cmd_arglist_t* arglist);

typedef struct cmd_entry_s {
    const char* cmdname; /* Command name */
    const char* params;  /* Command parameters */
    const char* help;    /* Command help */
    cmd_cb callback;     /* Command callback */
} cmd_entry_t;

struct vty_windowsize {
    unsigned short h, w;
};

struct vty_s {
    /* Is this vty connect to file or not */
    enum { CMD_TERM, /* stdin/stdout UI */
           CMD_FILE, /* reading and writing files */
           CMD_NET,  /* tcp or udp  */
    } type;

    uint32_t flags;

    /* Properties */
    const char *prompt, *intro, *doc_header;

    /* Index in cmd__entries array from which commands would be active */
    // int entry_start;
    vector_t entries;

    /* Flags */
    int exit_flag;

    /* in, out, err */
    int ifd;
    int ofd;
    FILE* vty_stdin;
    FILE* vty_stdout;

    /* Callback pointers */
    cmd_cb do_emptyline;
    int (*do_command)(vty_t* ctx, const char*, cmd_arglist_t*);
};

/* Default callbacks */
int cmd_default_do_command(vty_t* ctx, const char* cmdname, cmd_arglist_t* arglist);
int cmd_default_do_help(vty_t* ctx, cmd_arglist_t* arglist);
int cmd_default_do_emptyline(vty_t* ctx, cmd_arglist_t* arglist /* Unused */);
int cmd_default_do_exit(vty_t* ctx, cmd_arglist_t* arglist /* Unused */);
int cmd_default_do_noop(vty_t* vty, cmd_arglist_t* arglist /* Unused */);

/* Public interface functions */

/**
 * @brief Adding/Updating command.
 * @param callback command function porinter, NULL for removing command.
 * @param cmdname command name which can not be started or ended with the blank character.
 * @param cmdparams command parameters syntax (used as line completion hint).
 * @param help command help message (shown in help command).
 * @return always 0.
 * @note vty_register_command() can be called before vty_new() or after vty_new().
 */
int vty_register_command(cmd_cb callback, const char* cmdname, const char* cmdparams, const char* help);
int vty_remove_command(const char* cmdname);
void cmd_add_history(const char* line); // todo
void cmd_save_history();
void cmd_sort();

/* Synchronized API */
void vty_term(vty_t* vty);

/* Asynchronized API */
vty_t* vty_new(int type, const char* prompt, int ifd, int ofd, int flags);
int vty_parse(vty_t* vty, char* inputbuff);
int vty_out(vty_t* vty, const char* format, ...);

#include <libae.h>
vty_t* vty_term_server(aeEventLoop* loop);
int vty_tcp_server(aeEventLoop* loop, const char* host, int port);
int vty_tcp_client(aeEventLoop* loop, const char* host, int port);

#ifdef __cplusplus
}
#endif

#endif