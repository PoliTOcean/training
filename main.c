#include <stdio.h>
#include <stdlib.h>

void vision(void);

int main(int argc, const char *argv[])
{
    fprintf(stdout, "%s\n", "hello world!\n");
    return EXIT_SUCCESS;
}

void vision(void){

    fprintf(stdout, "%s\n", "Feature vision added to the ROV");
    return;
}