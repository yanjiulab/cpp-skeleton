
#include "libvty.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "libanet.h"
#include "liblog.h"
#include "libstr.h"

vector_t vty_cmds = NULL;

// TODO: use version
// Hello, this is FRRouting (version 9.0.1).
// Copyright 1996-2005 Kunihiro Ishiguro, et al.

static const char* vty_default_prompt = "(cmd) ";
static const char* vty_default_intro = "Hello, This is Virtual Terminal Interface.";
static const char* vty_default_doc_header = "Commands:";

/*---------------------------------------------------------------------------
                            Utility Functions
----------------------------------------------------------------------------*/
/* Trim cmd string in place */
static void cmd_trim(char* src) {
    char *begin = src, *end, *newln;
    size_t end_location;

    /* Check for empty string */
    if (strlen(src) == 1 && (src[0] == '\n' || src[0] == '\0')) {
        src[0] = '\0';
        return;
    }

    /* Replace newline */
    newln = strrchr(src, '\n');
    if (newln)
        *newln = '\0';

    /* Replace spaces and re-align the string */
    while (isspace((int)*begin))
        begin++;

    if (src != begin) {
        end_location = strlen(begin);
        memmove(src, begin, strlen(begin) + 1);
        memset(begin + end_location, '\0', sizeof(char) * strlen(begin - end_location));
    }

    /* Replaces spaces at the end of the string */
    end = strrchr(src, '\0');

    /* Check if we haven't reached the end of the string.
     * Because if we did, we have nothing else to do. */
    if (src == end)
        return;
    else
        end--;

    if (isspace((int)*end)) {
        do {
            *end = '\0';
            end--;
        } while (isspace((int)*end));
    }
}

/* Duplicate cmd string ending with `\0` */
static char* cmd_strdup(const char* src) {
    char* dst = (char*)(malloc(sizeof(char) * (strlen(src) + 1))); /* src + '\0' */
    if (!dst)
        return NULL;

    return strcpy(dst, src);
}

static struct vty_windowsize cmd_get_window_size() {

    struct vty_windowsize cm_winsize;
    struct winsize ws;

    memset(&cm_winsize, 0, sizeof(struct vty_windowsize));

    /* if this ioctl() fails, return the zeroed struct as-is */
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
        return cm_winsize;

    cm_winsize.w = ws.ws_col;
    cm_winsize.h = ws.ws_row;

    return cm_winsize;
}

/*
 * Print a string from the current line, down to the next line if needed,
 * which is padded with loffset characters.
 * The function takes the input string and prints it word by word - if a word can not
 * be printed on the line without being concatenated, it is printed on the next line.
 */
static void cmd_pprint(vty_t* vty, size_t loffset, const char* const strtoprint) {
    const struct vty_windowsize winsize = cmd_get_window_size();
    size_t total_printed = loffset, i, wordlen;
    char *strbuff = cmd_strdup(strtoprint), *wordptr;

    /* If we couldn't allocate a buffer, print regularly, exit. */
    if (!strbuff) {
        vty_out(vty, "\n%s\n", strtoprint);
        return;
    }

    /* Begin splitting by words */
    wordptr = strtok(strbuff, " \t\n");
    while (wordptr) {
        /* Check if we can print this word. */
        wordlen = strlen(wordptr);
        if (total_printed + (wordlen + 1) > (size_t)(winsize.w - CMD_PPRINT_RIGHT_OFFSET)) {
            /* Go to the next line and print the word there. */
            /* Print newline and loffset spaces */
            vty_out(vty, "\n");
            for (i = 0; i < loffset; i++)
                vty_out(vty, " ");

            total_printed = loffset;
        }

        /* Print the word */
        vty_out(vty, "%s ", wordptr);
        total_printed += wordlen + 1; /* strlen(word) + space */

        /* Get the next word */
        wordptr = strtok(NULL, " \t\n");
    }

    vty_out(vty, "\n");

    CMD_FREE(strbuff);
}

static cmd_arglist_t* cmd_parse_arguments(char* argline) {
    cmd_arglist_t* arglist = NULL;
    size_t i;
    char *strptr, *startptr;
    enum states { NONE,
                  IN_WORD,
                  IN_QUOTES } state = NONE;

    /* Check if there are any arguments */
    if (!argline)
        return NULL;

    /* Allocate argument list */
    arglist = (cmd_arglist_t*)(malloc(sizeof(cmd_arglist_t)));
    if (!arglist)
        return NULL;

    arglist->count = 0;

    /* First pass on the arguments line - use the state machine to determine how many
     * argument do we have. */
    for (strptr = startptr = argline, i = 0; *strptr; strptr++) {
        switch (state) {
        case NONE:
            /*
             * Space = Don't care.
             * Quotes = a quoted argument begins.
             * Anything else = Inside a word.
             */
            if (isspace((int)*strptr))
                continue;
            else if (*strptr == '\"')
                state = IN_QUOTES;
            else
                state = IN_WORD;

            break;
        case IN_QUOTES:
            /*
             * Space = Don't care, since we're inside quotes.
             * Quotes = Quotes have ended, so count++
             * Anything else = Don't care, since we're inside quotes.
             */
            if (isspace((int)*strptr))
                continue;
            else if (*strptr == '\"') {
                state = NONE;
                arglist->count++;

                break;
            } else
                continue;
        case IN_WORD:
            /*
             * Space = A word just terminated, so count++
             * Quotes = Ignore - quote is part of the word
             * Anything else = Still in word
             */
            if (isspace((int)*strptr)) {
                state = NONE;
                arglist->count++;

                break;
            } else if (*strptr == '\"')
                continue;
            else
                continue;
        }
    }

    /* Handle last argument counting, if any */
    if (state != NONE)
        arglist->count++;

    /* Now we can allocate the argument list */
    arglist->args = (char**)(malloc(sizeof(char*) * (arglist->count + 1))); /* + NULL */
    if (!arglist->args) {
        free(arglist);
        return NULL;
    }

    /* Populate argument list */
    state = NONE;
    for (strptr = startptr = argline, i = 0; *strptr; strptr++) {
        switch (state) {
        case NONE:
            /* Space = No word yet.
             * Quotes = Quotes started, so ignore everything inbetween.
             * Else = Probably a word. */
            if (isspace((int)*strptr))
                continue;
            else if (*strptr == '\"') {
                state = IN_QUOTES;
                startptr = strptr + 1;
            } else {
                state = IN_WORD;
                startptr = strptr;
            }

            break;
        case IN_QUOTES:
            /* Space = Ignore it, iterate futher.
             * Quotes = End quotes, so put the entire quoted part as an argument.
             * Else = Whatever is between the quotes */
            if (*strptr == '\"') {
                *strptr = '\0';
                arglist->args[i++] = cmd_strdup(startptr);
                state = NONE;
            }

            break;
        case IN_WORD:
            /* Space = End of word, so parse it.
             * Quote = Some quote inside of a word. We treat it is a word.
             * Else = Still a word. */
            if (isspace((int)*strptr)) {
                *strptr = '\0';
                arglist->args[i++] = cmd_strdup(startptr);
                state = NONE;
            }

            break;
        }
    }

    /* Get the last argument, if any. */
    if (state != NONE && i < arglist->count)
        arglist->args[i++] = cmd_strdup(startptr);

    /* Set up null terminator in the argument list */
    arglist->args[i] = NULL;

    return arglist;
}

static void cmd_free_arglist(cmd_arglist_t* arglist) {
    size_t i;

    /* Check if any argument list was provided. */
    if (arglist) {
        /* Free every argument */
        for (i = 0; i < arglist->count - 1; i++)
            free(arglist->args[i]);

        free(arglist->args);
    }

    free(arglist);
}

/*---------------------------------------------------------------------------
                            Default callbacks
----------------------------------------------------------------------------*/
int cmd_default_do_command(vty_t* vty, const char* cmdname, cmd_arglist_t* arglist) {
    int i;
    cmd_entry_t* entry;

    /* Iterate through the commands list. Find and execute the appropriate command */
    vector_foreach(vty_cmds, entry, i) {
        if (strcmp(cmdname, entry->cmdname) == 0 && entry->callback) {
            if (entry->params && entry->params[0] != '[' && !arglist) {
                // required argument do not have any argument
                return CMD_ERROR_ARGUMENT_ERROR;
            } else {
                // check
                return entry->callback(vty, arglist);
            }
        }
    }

    return CMD_ERROR_UNKNOWN_COMMAND;
}

void cmd_print_command_list(vty_t* vty) {

    int i, j, offset;
    cmd_entry_t* entry;
    const struct vty_windowsize winsize = cmd_get_window_size();

    /* Print documented commands */
    vty_out(vty, "\n%s\n\n", vty->doc_header);
    vector_foreach(vty_cmds, entry, i) {
        if (entry->help) {

            /* Print command */
            string_t cmd = str_new(entry->cmdname);
            if (entry->params) cmd = str_catfmt(cmd, " %s", entry->params);
            int len = str_len(cmd);
            offset = vty_out(vty, "%-16s ", cmd);
            str_free(cmd);

            /* Check if we need to break into the next line. */
            if (len >= 17) {
                vty_out(vty, "\n");
                offset = 17;
                for (j = 0; j < offset; j++)
                    vty_out(vty, " ");
            } else if (len + strlen(entry->help) >= winsize.w) {
                offset = 17;
            }
            cmd_pprint(vty, offset, entry->help);
        }
    }

    vty_out(vty, "\n");
}

int cmd_default_do_help(vty_t* vty, cmd_arglist_t* arglist) {
    int i;
    size_t offset;
    cmd_entry_t* entry;

    if (arglist) {
        if (arglist->count == 1) {
            vector_foreach(vty_cmds, entry, i) {
                if (strcmp(arglist->args[0], entry->cmdname) == 0) {
                    /* Print help, if any */
                    if (entry->help) {
                        vty_out(vty, "%s\n", entry->help);
                    } else {
                        vty_out(vty, "\n(No documentation)\n");
                    }
                    return CMD_OK;
                }
            }
            /* If we reached this, means that the command was not found */
            vty_out(vty, "Command '%s' was not found.\n", arglist->args[0]);
            return CMD_ERROR_UNKNOWN_COMMAND;
        } else {
            vty_out(vty, "Too many arguments for the 'help' command!\n");
            return CMD_ERROR_TOO_MANY_ARGS;
        }
    } else {
        cmd_print_command_list(vty);
    }

    return CMD_OK;
}

int cmd_default_do_emptyline(vty_t* vty, cmd_arglist_t* arglist /* Unusued */) {
    return CMD_OK;
}

int cmd_default_do_exit(vty_t* vty, cmd_arglist_t* arglist /* Unused */) {
    exit(EXIT_SUCCESS);

    return CMD_OK;
}

int cmd_default_do_noop(vty_t* vty, cmd_arglist_t* arglist /* Unused */) {
    return CMD_OK;
}

void cmd_default_completion(const char* buf, linenoiseCompletions* lc) {

    int i;
    cmd_entry_t* entry;
    vector_foreach(vty_cmds, entry, i) {
        if (strncasecmp(buf, entry->cmdname, strlen(buf)) == 0) {
            linenoiseAddCompletion(lc, entry->cmdname);
        }
    }
}

char* cmd_default_hints(const char* buf, int* color, int* bold) {
    char* hint = NULL;
    int i;
    cmd_entry_t* entry;
    vector_foreach(vty_cmds, entry, i) {
        if (strcasecmp(buf, entry->cmdname) == 0) {
            if (!entry->params) return NULL;
            *color = 90;
            *bold = 0;
            hint = malloc(strlen(entry->params + 2));
            hint[0] = ' ';
            strcpy(hint + 1, entry->params);
            return hint;
        }
    }
    return NULL;
}

void cmd_default_free_hints(void* ptr) {
    free(ptr);
}

int cmd_do_mask(vty_t* vty, cmd_arglist_t* arglist /* Unused */) {
    linenoiseMaskModeEnable();
    return 0;
}

int cmd_do_unmask(vty_t* vty, cmd_arglist_t* arglist /* Unused */) {
    linenoiseMaskModeDisable();
    return 0;
}

void cmd_add_history(const char* line) {
#ifdef CMD_READLINE_SUPPORT
    add_history(line);
#endif
    linenoiseHistoryAdd(line); /* Add to the history. */
}

void cmd_save_history() {
    linenoiseHistorySave("history.txt"); /* Save the history on disk. */
}

static int cmd_cmp(const void* p1, const void* p2) {
    cmd_entry_t* str1 = *(cmd_entry_t**)p1;
    cmd_entry_t* str2 = *(cmd_entry_t**)p2;
    if (!str1->cmdname && !str2->cmdname)
        return 0;
    if (!str1->cmdname)
        return -1;
    if (!str2->cmdname)
        return 1;
    return strcmp(str1->cmdname, str2->cmdname);
}

void cmd_sort() {
    vector_sort(vty_cmds, cmd_cmp);
}

/*---------------------------------------------------------------------------
                            Low level
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                            High level
----------------------------------------------------------------------------*/

int vty_register_command(cmd_cb callback, const char* cmdname,
                         const char* cmdparams, const char* help) {

    if (!vty_cmds)
        vty_cmds = vector_new(CMD_INIT_SIZE);

    /* Initialize new entry */
    cmd_entry_t* entry;
    entry = calloc(1, sizeof(*entry));
    entry->cmdname = cmdname;
    entry->params = cmdparams;
    entry->help = help ? help : "(No documentation)";
    entry->callback = callback;

    /* Adding/Updating cmdname */
    int i;
    cmd_entry_t* e;
    vector_foreach(vty_cmds, e, i) {
        if (strcmp(e->cmdname, cmdname) == 0) {
            if (callback) {
                // update
                free(e);
                vty_cmds->index[i] = entry;
                return CMD_OK;
            }
        }
    }

    vector_push(vty_cmds, entry);

    return CMD_OK;
}

int vty_remove_command(const char* cmdname) {
    /* Check cmdname */
    int i;
    cmd_entry_t* entry;
    vector_foreach(vty_cmds, entry, i) {
        if (strcmp(entry->cmdname, cmdname) == 0) {
            vector_remove(vty_cmds, i);
            return CMD_OK;
        }
    }

    return CMD_ERROR_UNKNOWN_COMMAND;
}

int vty_out(vty_t* vty, const char* format, ...) {
    va_list args;
    int len = 0;
    char buf[1024];
    int ret;

    va_start(args, format);
    len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    switch (vty->type) {
    case CMD_FILE:
    case CMD_TERM:
        ret = fprintf(vty->vty_stdout, "%s", buf);
        break;
    case CMD_NET:
        ret = write(vty->ofd, buf, len);
        break;
    default:
        break;
    }

    return ret;
}

int vty_parse(vty_t* vty, char* inputbuff) {

    if (!inputbuff) return CMD_ERROR_UNKNOWN_COMMAND;

    cmd_trim(inputbuff);

    /* If input is empty, call do_emptyline command. */
    if (inputbuff[0] == '\0') {
        vty->do_emptyline(vty, NULL);
        return CMD_ERROR_UNKNOWN_COMMAND;
    }

    /* If we've reached this, then line has something in it.
     * If readline is enabled, save this to history. */
    cmd_add_history(inputbuff);

    /* Search commands first, followed by arguments. */
    int retflag;
    cmd_arglist_t* cmd_args;
    char *cmdptr = inputbuff, *argsptr = NULL, *spcptr = inputbuff;

    int i;
    cmd_entry_t *entry, *match = NULL;
    int max = 0;
    vector_foreach(vty_cmds, entry, i) {
        // printd("entry: %s, input: %s", entry->cmdname, inputbuff);
        if (strncmp(inputbuff, entry->cmdname, strlen(entry->cmdname)) == 0 &&
            strlen(entry->cmdname) > max) {
            match = entry;
            max = strlen(entry->cmdname);
        }
    }

    // long match
    if (match) {
        spcptr += strlen(match->cmdname);
        if (isblank(*spcptr)) {
            *spcptr = '\0';
            argsptr = spcptr + 1;
        } else {
            argsptr = NULL;
        }
    } else {
        vty_out(vty, "Unknown command '%s'.\n", cmdptr);
        return CMD_ERROR_UNKNOWN_COMMAND;
    }

    /* Parse arguments */
    cmd_args = cmd_parse_arguments(argsptr);

    /* Execute command. */
    retflag = vty->do_command(vty, cmdptr, cmd_args);
    switch (retflag) {
    case CMD_ERROR_UNKNOWN_COMMAND:
        vty_out(vty, "Unknown command '%s'.\n", cmdptr);
        break;
    case CMD_ERROR_ARGUMENT_ERROR:
        vty_out(vty, "'%s' argument error.\n", cmdptr);
        break;
    case CMD_ERROR_TOO_MANY_ARGS:
        vty_out(vty, "'%s' too many arguments.\n", cmdptr);
        break;
    case CMD_ERROR_TOO_FEW_ARGS:
        vty_out(vty, "'%s' too few arguments.\n", cmdptr);
        break;
    }

    /* Flush stream */
    if (vty->type == CMD_TERM || vty->type == CMD_FILE) {
        fflush(vty->vty_stdout);
    }

    /* Free arguments */
    cmd_free_arglist(cmd_args);
}

static void vty_init_command(vty_t* vty) {

    /* Register help callback */
    vty_register_command(cmd_default_do_help, "help", "[command]",
                         "Get information on a command or list commands.");

    /* Register exit callback, if required */
    if (vty->flags & CMD_FLAG_AUTOEXIT)
        vty_register_command(cmd_default_do_exit, "exit", NULL, "Exit the process.");

    // For test purpose
    if (vty->type == CMD_TERM) {
        vty_register_command(cmd_do_mask, "/mask", NULL, "Enable mask mode.");
        vty_register_command(cmd_do_unmask, "/unmask", NULL, "Disable mask mode.");
    }
}

vty_t* vty_new(int type, const char* prompt, int ifd, int ofd, int flags) {
    vty_t* vty = calloc(1, sizeof(*vty));

    vty->flags |= flags; // if (flags & CMD_FLAG_ASYNC)
    vty->type = type;
    vty->prompt = prompt ? prompt : vty_default_prompt;
    vty->intro = vty_default_intro;
    vty->doc_header = vty_default_doc_header;

    /* Set command callbacks */
    vty->do_command = cmd_default_do_command;
    vty->do_emptyline = cmd_default_do_emptyline;

    // register internal commands
    vty_init_command(vty);

    vty->entries = vty_cmds;

    // setting io
    switch (type) {
    case CMD_TERM:
        vty->ifd = STDIN_FILENO;
        vty->ofd = STDOUT_FILENO;
        vty->vty_stdin = stdin;
        vty->vty_stdout = stdout;
        break;
    case CMD_FILE:
        vty->ifd = ifd;
        vty->ofd = ofd;
        vty->vty_stdin = fdopen(ifd, "r");
        vty->vty_stdout = fdopen(ofd, "w");
        break;
    case CMD_NET:
        vty->ifd = ifd;
        vty->ofd = ofd;
        vty->vty_stdin = NULL;
        vty->vty_stdout = NULL;
        break;
    default:
        break;
    }

    linenoiseSetMultiLine(1);
    /* Set the completion callback.
     * This will be called every time the user uses the <tab> key. */
    linenoiseSetCompletionCallback(cmd_default_completion);
    linenoiseSetHintsCallback(cmd_default_hints);
    linenoiseSetFreeHintsCallback(cmd_default_free_hints);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
    return vty;
}

/* Synchronized stdin/stdout UI */
void vty_term(vty_t* vty) {

    char* inputbuff;
    char *cmdptr, *argsptr, *spcptr;
    int retflag;

    if (!vty) {
        vty = vty_new(CMD_TERM, 0, 0, 0, CMD_FLAG_AUTOEXIT);
    }

    /* Sort the commands */
    cmd_sort();

    /* Print intro, if any. */
    if (vty->intro)
        fprintf(vty->vty_stdout, "\n%s\n\n", vty->intro);

    while (!vty->exit_flag) {
        /* Get input and allocate inputbuff */
        inputbuff = linenoise(vty->prompt);

        /* EOF, or failure to allocate a buffer. Means we probably need to exit. */
        if (!inputbuff) {
            vty->exit_flag = 1;
            continue;
        }

        /* Process command then free inputbuff */
        retflag = vty_parse(vty, inputbuff);
    }
}

static struct linenoiseState ls;
#define LS_BUFLEN 1024
static char ls_buf[LS_BUFLEN];

/* Asynchronized stdin/stdout UI */
static void cmd_term_serv(aeEventLoop* loop, int fd, void* clientData, int mask) {

    vty_t* vty = (vty_t*)clientData;

    char* line = linenoiseEditFeed(&ls);
    if (line != linenoiseEditMore) {
        // Encounter ENTER, then command input should stop
        linenoiseEditStop(&ls);
        if (line == NULL) { // CTRL+C/D
            linenoiseHide(&ls);
            aeDeleteFileEvent(loop, fd, AE_READABLE);
            aeSetAfterSleepProc(loop, NULL);
            aeSetBeforeSleepProc(loop, NULL);
            linenoiseEditStop(&ls);
            printd("Exit the command line framework.");
            // aeDeleteEventLoop(loop);
            return;
        }
        // Process command
        vty_parse(vty, line);

        linenoiseFree(line);
        // Restart the next command input process
        linenoiseEditStart(&ls, 0, 1, ls_buf, LS_BUFLEN, vty->prompt);
    }
}

static void cmd_term_before(aeEventLoop* eventLoop) {
    linenoiseShow(&ls);
}

static void cmd_term_after(aeEventLoop* eventLoop) {
    linenoiseHide(&ls);
}

vty_t* vty_term_server(aeEventLoop* loop) {
    /* Create vty with AUTOEXIT and ASYNC flag */
    vty_t* vty = vty_new(CMD_TERM, NULL, 0, 0, CMD_FLAG_AUTOEXIT | CMD_FLAG_ASYNC);

    /* Init linenoise */
    linenoiseEditStart(&ls, 0, 1, ls_buf, LS_BUFLEN, vty->prompt);

    /* Set eventloop */
    aeSetBeforeSleepProc(loop, cmd_term_before);
    aeSetAfterSleepProc(loop, cmd_term_after);
    int ret = aeCreateFileEvent(loop, 0, AE_READABLE, cmd_term_serv, vty);
    assert(ret != ANET_ERR);

    /* Sort the commands */
    cmd_sort();

    return vty;
}

/* Asynchronized TCP Server UI */
static void cmd_on_recv(aeEventLoop* loop, int fd, void* clientData, int mask) {
    int buf_sz = 1024;
    char* buf = calloc(1, buf_sz);
    int size;
    vty_t* vty = (vty_t*)clientData;

    size = read(fd, buf, buf_sz);
    if (size == 0) { // disconnect
        char peeraddrstr[SOCKADDR_STRLEN] = {0};
        char localaddrstr[SOCKADDR_STRLEN] = {0};
        net_format_fd_addr(fd, peeraddrstr, SOCKADDR_STRLEN, FD_TO_PEER_NAME);
        net_format_fd_addr(fd, localaddrstr, SOCKADDR_STRLEN, FD_TO_SOCK_NAME);
        aeDeleteFileEvent(loop, fd, AE_READABLE);
        logi("client [%s] disconnected from server [%s]",
             peeraddrstr, localaddrstr);
        return;
    }

    vty_parse(vty, buf);
    vty_out(vty, "%s", vty->prompt);
}

static void cmd_on_accept(aeEventLoop* loop, int fd, void* clientData, int mask) {

    int client_fd, client_port, local_port, ret;
    char client_addr[SOCKADDR_STRLEN] = {0};
    client_fd = net_tcp_accept(NULL, fd, client_addr, 128, &client_port);
    // assert(client_fd != ANET_ERR);

    char localaddr[SOCKADDR_STRLEN] = {0};
    net_format_fd_addr(fd, localaddr, SOCKADDR_STRLEN, FD_TO_SOCK_NAME);
    logi("client [%s:%d] connected to server [%s]", client_addr, client_port, localaddr);

    net_nonblock(NULL, client_fd);

    vty_t* vty = vty_new(CMD_NET, NULL, client_fd, client_fd, CMD_FLAG_ASYNC);
    vty_out(vty, "\n%s\n\n%s", vty->intro, vty->prompt);

    aeCreateFileEvent(loop, client_fd, AE_READABLE, cmd_on_recv, vty);
}

int vty_tcp_server(aeEventLoop* loop, const char* host, int port) {
    char errbuf[32];
    int vtyfd = net_tcp_server(errbuf, port, host, 0);
    if (vtyfd == AE_ERR) {
        printe("%s", errbuf);
        return -1;
    }
    logi("create tcp command server, listenfd=%d", vtyfd);

    int ret = aeCreateFileEvent(loop, vtyfd, AE_READABLE, cmd_on_accept, NULL);
    assert(ret != AE_ERR);

    return 0;
}

/* shell UI */
static int tcpcli = -1;

// recv from stdin -> send to tcp server
static void cmd_term_cli(aeEventLoop* loop, int fd, void* clientData, int mask) {

    vty_t* vty = (vty_t*)clientData;

    char* line = linenoiseEditFeed(&ls);
    if (line != linenoiseEditMore) {
        // Encounter ENTER, then command input should stop
        linenoiseEditStop(&ls);
        if (line == NULL) { // CTRL+C/D
            linenoiseHide(&ls);
            aeDeleteFileEvent(loop, fd, AE_READABLE);
            aeSetAfterSleepProc(loop, NULL);
            aeSetBeforeSleepProc(loop, NULL);
            linenoiseEditStop(&ls);
            printd("Exit the command line framework.");
            // aeDeleteEventLoop(loop);
            return;
        }
        // Process command
        write(tcpcli, line, strlen(line));
        cmd_add_history(line);

        linenoiseFree(line);
        // Restart the next command input process
        linenoiseEditStart(&ls, 0, 1, ls_buf, LS_BUFLEN, vty->prompt);
    }
}

vty_t* vty_term_cli(aeEventLoop* loop) {
    /* Create vty with AUTOEXIT and ASYNC flag */
    vty_t* vty = vty_new(CMD_TERM, NULL, 0, 0, CMD_FLAG_AUTOEXIT | CMD_FLAG_ASYNC);

    /* Init linenoise */
    linenoiseEditStart(&ls, 0, 1, ls_buf, LS_BUFLEN, vty->prompt);

    /* Set eventloop */
    aeSetBeforeSleepProc(loop, cmd_term_before);
    aeSetAfterSleepProc(loop, cmd_term_after);
    int ret = aeCreateFileEvent(loop, 0, AE_READABLE, cmd_term_cli, vty);

    assert(ret != ANET_ERR);

    /* Sort the commands */
    cmd_sort();

    return vty;
}

// recv from tcp server -> print
static void cli_on_recv(aeEventLoop* eventLoop, int fd, void* clientData, int mask) {
    int buf_sz = 1024;
    char* buf = calloc(1, buf_sz);
    int size;

    size = net_readline(fd, buf, 1024);
    // printf("s:%d\n", size);
    if (size == 0) {
        aeDeleteFileEvent(eventLoop, fd, AE_READABLE);
    }

    if (size > 0) {
        printf("%s", buf);
        fflush(stdout);
    }
    free(buf);
    // assert(size != -1);
}

int vty_tcp_client(aeEventLoop* loop, const char* host, int port) {
    int ret;
    char errbuf[1024] = {0};
    tcpcli = net_tcp_nonblock_connect(errbuf, host, port);
    if (tcpcli == AE_ERR) {
        printe("%s", errbuf);
        return -1;
    }
    printd("%d", tcpcli);

    ret = aeCreateFileEvent(loop, tcpcli, AE_READABLE, cli_on_recv, NULL);
    assert(ret != AE_ERR);

    vty_term_cli(loop);

    return 0;
}
