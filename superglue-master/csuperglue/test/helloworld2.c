#include "csuperglue.h"
#include <stdio.h>

struct my_args {
    int i;
};

void my_task(void *args_) {
    struct my_args *args = (struct my_args *) args_;
    /* args here is a private copy */
    printf("Hello world! Private copy of args = %d\n", args->i);
    ++args->i;
}

int main() {
    struct my_args args;
    args.i = 32;

    sg_init();
    sg_submit(my_task, &args, sizeof(struct my_args), 0, 0);
    sg_barrier();
    sg_submit(my_task, &args, sizeof(struct my_args), 0, 0);
    sg_destroy();
    return 0;
}
