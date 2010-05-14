#include "error.h"
#include "lowlevel.h"
#include <stdio.h>

int main(int argc, char **argv) {
    printf("Looking for brick...\n");

    nxt_t       *nxt;
    nxt_error_t err;

    nxt_err_check(nxt_init(&nxt));
    
    return 0;
}
