extern "C" {
#include "error.h"
#include "lowlevel.h"
}

#include <stdio.h>

int main(void) {
    printf("Looking for brick...\n");

    nxt_t       *nxt;

    char boh[] = "boh";

    nxt_err_check(nxt_init(&nxt), "Init");
    nxt_err_check(nxt_find(nxt), "Find");
    nxt_err_check(nxt_open(nxt), "Open");
    nxt_err_check(nxt_send_str(nxt, boh), "Send");

    nxt_err_check(nxt_close(nxt), "Close");

    printf("Finished OK\n");
    return 0;
}
