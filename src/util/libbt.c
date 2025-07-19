/*---------------------------------------------------------------------------
            backtrace - support for application self-debugging
                (need addr2line tool and gnu compiler)
----------------------------------------------------------------------------*/
#define _GNU_SOURCE 1

#include "libbt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <fcntl.h>

#define BT_SIZE    100
#define PATH_SPLIT '/'
#define PATH_MAX   4096

static const char* prog = NULL;

// static int signals_all[] = {
//     SIGHUP,    /* 1 Hangup (POSIX).  */
//     SIGINT,    /* 2 Interrupt (ANSI).  */
//     SIGQUIT,   /* 3 Quit (POSIX).  */
//     SIGILL,    /* 4 Illegal instruction (ANSI).  */
//     SIGTRAP,   /* 5 Trace trap (POSIX).  */
//     SIGABRT,   /* 6 Abort (ANSI).  */
//     SIGIOT,    /* 6 IOT trap (4.2 BSD).  */
//     SIGBUS,    /* 7 BUS error (4.2 BSD).  */
//     SIGFPE,    /* 8 Floating-point exception (ANSI).  */
//     SIGKILL,   /* 9 Kill, unblockable (POSIX).  */
//     SIGUSR1,   /* 10 User-defined signal 1 (POSIX).  */
//     SIGSEGV,   /* 11 Segmentation violation (ANSI).  */
//     SIGUSR2,   /* 12 User-defined signal 2 (POSIX).  */
//     SIGPIPE,   /* 13 Broken pipe (POSIX).  */
//     SIGALRM,   /* 14 Alarm clock (POSIX).  */
//     SIGTERM,   /* 15 Termination (ANSI).  */
//     SIGSTKFLT, /* 16 Stack fault.  */
//     SIGCLD,    /* 17 Same as SIGCHLD (System V).  */
//     SIGCHLD,   /* 17 Child status has changed (POSIX).  */
//     SIGCONT,   /* 18 Continue (POSIX).  */
//     SIGSTOP,   /* 19 Stop, unblockable (POSIX).  */
//     SIGTSTP,   /* 20 Keyboard stop (POSIX).  */
//     SIGTTIN,   /* 21 Background read from tty (POSIX).  */
//     SIGTTOU,   /* 22 Background write to tty (POSIX).  */
//     SIGURG,    /* 23 Urgent condition on socket (4.2 BSD).  */
//     SIGXCPU,   /* 24 CPU limit exceeded (4.2 BSD).  */
//     SIGXFSZ,   /* 25 File size limit exceeded (4.2 BSD).  */
//     SIGVTALRM, /* 26 Virtual alarm clock (4.2 BSD).  */
//     SIGPROF,   /* 27 Profiling alarm clock (4.2 BSD).  */
//     SIGWINCH,  /* 28 Window size change (4.3 BSD, Sun).  */
//     SIGPOLL,   /* 29 Pollable event occurred (System V).  */
//     SIGIO,     /* 29 I/O now possible (4.2 BSD).  */
//     SIGPWR,    /* 30 Power failure restart (System V).  */
//     SIGSYS,    /* 31 Bad system call.  */
// };

static int signals_trace[] = {
    SIGILL,  /* Illegal instruction (ANSI).  */
    SIGABRT, /* Abort (ANSI).  */
    SIGFPE,  /* Floating-point exception (ANSI).  */
    SIGSEGV, /* Segmentation violation (ANSI).  */
    SIGTERM,
    SIGPIPE, /* PIPE */
    // SIGBUS,  /* BUS error (4.2 BSD). (unaligned access) */
    SIGINT};

/**
 * @brief Resolve symbol name and source location given the path to the executable and an address
 * @param program_name
 * @param addr
 * @param lineNb
 * @return returns 0 if address has been resolved and a message has been printed; else returns 1
 */
static int addr2line(char const* const program_name, void const* const addr, int lineNb) {
    char addr2line_cmd[512] = {0};

/* have addr2line map the address to the relent line in the code */
#ifdef OS_DARWIN
    /* apple does things differently... */
    sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
#else
    sprintf(addr2line_cmd, "addr2line -f -e %.256s %p", program_name, addr);
#endif

    /* This will print a nicely formatted string specifying the
    function and source line of the address */

    FILE* fp;
    char outLine1[1035];
    char outLine2[1035];

    /* Open the command for reading. */
    fp = popen(addr2line_cmd, "r");
    if (fp == NULL)
        return 1;

    while (fgets(outLine1, sizeof(outLine1) - 1, fp) != NULL) {
        // if we have a pair of lines
        if (fgets(outLine2, sizeof(outLine2) - 1, fp) != NULL) {
            // if symbols are readable
            if (outLine2[0] != '?') {
                // only let func name in outLine1
                int i;
                for (i = 0; i < 1035; ++i) {
                    if (outLine1[i] == '\r' || outLine1[i] == '\n') {
                        outLine1[i] = '\0';
                        break;
                    }
                }

                // don't display the whole path
                int lastSlashPos = 0;

                for (i = 0; i < 1035; ++i) {
                    if (outLine2[i] == '\0')
                        break;
                    if (outLine2[i] == '/')
                        lastSlashPos = i + 1;
                }
                fprintf(stderr, "[%i] %p in %s at %s", lineNb, addr, outLine1, outLine2 + lastSlashPos);
                fflush(stderr);
            } else {
                pclose(fp);
                return 1;
            }
        } else {
            pclose(fp);
            return 1;
        }
    }

    /* close */
    pclose(fp);

    return 0;
}

static int get_proc_name(char* name, size_t len) {
    int i, ret;
    char proc_name[PATH_MAX];
    char* ptr = NULL;
    memset(proc_name, 0, sizeof(proc_name));
    if (-1 == readlink("/proc/self/exe", proc_name, sizeof(proc_name))) {
        fprintf(stderr, "readlink failed!\n");
        return -1;
    }
    ret = strlen(proc_name);
    for (i = ret, ptr = proc_name; i > 0; i--) {
        if (ptr[i] == PATH_SPLIT) {
            ptr += i + 1;
            break;
        }
    }
    if (i == 0) {
        fprintf(stderr, "proc path %s is invalid\n", proc_name);
        return -1;
    }
    if (ret - i > (int)len) {
        fprintf(stderr, "proc name length %d is larger than %d\n", ret - i, (int)len);
        return -1;
    }
    strncpy(name, ptr, ret - i);
    return 0;
}

void backtrace_dump(int calledFromSigInt) {
    void* buffer[BT_SIZE];
    char** strings;
    char progname[256] = {0};
    if (prog) {
        sprintf(progname, prog, 256);
    } else {
        get_proc_name(progname, 256);
    }

    int nptrs = backtrace(buffer, BT_SIZE);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
    unsigned int i = 1;
    if (calledFromSigInt != 0)
        ++i;
    for (; i < (unsigned int)(nptrs - 2); ++i) {
        // if addr2line failed, print what we can
        if (addr2line(progname, buffer[i], nptrs - 2 - i - 1) != 0)
            fprintf(stderr, "[%i] %s\n", nptrs - 2 - i - 1, strings[i]);
    }

    free(strings);
}

static void backtrace_signal_handler(int signo, siginfo_t* info, void* ucontext) {

    switch (signo) {
    case SIGABRT:
        fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
        break;
    case SIGFPE:
        fputs("Caught SIGFPE: arithmetic exception, such as divide by zero\n", stderr);
        break;
    case SIGILL:
        fputs("Caught SIGILL: illegal instruction\n", stderr);
        break;
    case SIGINT:
        fputs("Caught SIGINT: interactive attention signal, probably a ctrl+c\n", stderr);
        exit(EXIT_SUCCESS);
        break;
    case SIGSEGV:
        fputs("Caught SIGSEGV: segment fault\n", stderr);
        break;
    case SIGPIPE:
        fputs("Caught SIGPIPE: Broken pipe\n", stderr);
        break;
    case SIGTERM:
    default:
        fputs("Caught SIGTERM: a termination request was sent to the program\n", stderr);
        break;
    }

    backtrace_dump(1);

    // we set back to default signal handler instead of calling _Exit(EXIT_FAILURE) directly.
    // give the kernel a chance to generate a coredump when segfault.
    struct sigaction sa_default = {.sa_handler = SIG_DFL};
    sigaction(signo, &sa_default, NULL);
    raise(signo);
}

int backtrace_init(const char* progname) {

    prog = progname;

    int i;
    int ret = 0;
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = backtrace_signal_handler;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    for (i = 0; i < (sizeof(signals_trace) / sizeof(int)); ++i) {
        if (sigaction(signals_trace[i], &sa, NULL) == -1) {
            fprintf(stderr, "signal failed to set signal handler for %s(%d)!\n",
                    strsignal(signals_trace[i]), signals_trace[i]);
            ret = -1;
            break;
        }
    }

    return ret;
}