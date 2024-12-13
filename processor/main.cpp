#include <stdlib.h>
#include <stdio.h>
#include "processor.h"

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "error: binary file not specified\n");
                exit(1);
        }

        run_processor(argv[1]);
        return 0;
}
