
#include "libvty.h"
#include <stdio.h>
#include <stdlib.h>
#include <liblog.h>
#include <libposix.h>

static int on_timer(aeEventLoop* eventLoop, long long id, void* clientData) {
    printf("In the raw mode, two consecutive lines ended with \\n will cause \n");
    printf("the second line not to start from the beginning.\n");
    printf(" Please use \\r\\n in the end instead of \\n.\r\n");
    printf("Or use printd or log function.\r\n");
    printd("%lld - Hello Timer", id);
    printd("%lld - Hello Timer", id);
    logi("%lld - Hello Timer", id);
    logw("%lld - Hello Timer", id);

    return 1 * 1000;
}

static int cmd_do_hello(vty_t* vty, cmd_arglist_t* arglist) {

    if (arglist->count > 1) {
        return CMD_ERROR_TOO_MANY_ARGS;
    }

    vty_out(vty, "Hello '%s'\n", arglist->args[0]);

    return CMD_OK;
}

int main(int argc, char** argv) {

    vty_register_command(cmd_do_hello, "hello", "<world>", "Hello world.");
    
    if (argc == 2) {
        aeEventLoop* loop = aeCreateEventLoop(10);
        vty_tcp_client(loop, "127.0.0.1", 8888);
        aeMain(loop);
    } else {
        // log_set_handler(file_logger);
        log_enable_color();

        aeEventLoop* loop = aeCreateEventLoop(10);
        // aeCreateTimeEvent(loop, 1000, on_timer, NULL, NULL);
        vty_tcp_server(loop, "0.0.0.0", 8888);
        vty_term_server(loop);
        aeMain(loop);
    }

    return 0;
}
