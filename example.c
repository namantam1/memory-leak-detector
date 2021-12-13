#include <stdio.h>
#include "leak.h"

int main() {
    char *a = malloc(100);
    free(a);

    char *b = malloc(120);
    // forget to free         //--> LEAK (120)

    char *c = calloc(10, 8);
    // refrence to memory lost
    c = malloc(10);           //--> LEAK (10 * 8)

    char *d = realloc(c, 15); // Here memory will be free first
    free(d);                  // then allocated so 1 free will add here also

    // TOTAL --> 120 + 10 * 8 = 200 bytes Leaked
    //       --> 5 allocations
    //       --> 3 free

    return EXIT_SUCCESS;
}
