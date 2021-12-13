Yet another C/C++ header only Memory leak detector library for programs using basic allocators like 
`malloc`, `calloc`, `realloc` and `free`.

## Features

1. Detect any memory leak and tell the exact line and filename where the allocation was done.
2. Warn if any `NULL` value is tried to free.
3. Detect and warn if try to free a already deallocated pointer.

## Platform supported

I developed and tested this program in `wsl` having ubuntu 20 installed and also tested on `windows 11`, 
so may be this program works well for other operating systems.

In case you get any error using this in your system please considering making an issue.

## Usage

```cpp
// example.c
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
```

*Output*
```bash
root:~leak-detector$ ./example 
/*========= SUMMARY =========*/
  Total allocations      5  
  Total Free             3  
  Total Memory allocated 325 bytes 
  Total Memory freed     125 bytes 
  Memory Leaked          200 bytes 

/*===== DETAILED REPORT =====*/
Memory leak at example.c:8 (120 bytes)
Memory leak at example.c:11 (80 bytes)
==============================
```

## TODO

- Write test.
- Try to figure out if it is possible to implement all these feature with same verbosity in `shim` so that this library can be used to test existing program without editing a single line of code. By implementing so we can use the magic of `LD_PRELOAD` environment variable to lead it dynamically to intercept default allocator.

## Contributions
Any advice, improvement or bug fix is welcome. Just make a PR.

## Reference
- https://github.com/bibhas2/Memd

## License

```
The MIT License

Copyright (c) 2021-2022 Naman Tamrakar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
