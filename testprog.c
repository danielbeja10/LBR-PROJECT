#include <stdio.h>  // for printf
#include <stdlib.h> // for atoi

int main(int argc, char** argv) {
    int x = (argc > 1) ? atoi(argv[1]) : 0;

    if (x > 10) {
        printf("x is big\n");
    } else if (x > 5) {
        printf("x is medium\n");
    } else {
        printf("x is small\n");
    }

    return 0;
}
