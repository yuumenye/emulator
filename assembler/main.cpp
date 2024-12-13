#include <stdlib.h>
#include <stdio.h>
#include "assembler.h"

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "error: source file not specified\n");
                exit(1);
        }

        run_assembler(argv[1]);
        return 0;
}
