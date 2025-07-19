/*---------------------------------------------------------------------------
            backtrace - support for application self-debugging
----------------------------------------------------------------------------*/

#ifndef LIBBT_H
#define LIBBT_H

#define LIBBT_VERSION "0.0.1"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize backtrace handler, once "Segmentation fault" occured,
 * the backtrace info will be show like "(gdb) bt"
 * @param progname please use argv[0] if the program is not in the current working dir.
 * @return -1 if error occurs
 */
int backtrace_init(const char* progname);

/**
 * @brief prints formated stack trace with most information as possible
 * @param calledFromSigInt indicates if the function is called by the signal handler or not
 * (to hide the call to the signal handler)
 */
void backtrace_dump(int calledFromSigInt);

#ifdef __cplusplus
}
#endif
#endif
