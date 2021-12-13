#ifndef __LEAK_DETECTOR_H_
#define __LEAK_DETECTOR_H_

#ifdef __cpluscplus
extern "C"
{
#endif // __cpluscplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define LEAK_MEM_SIZE 1000
#define _leak_warn(file, line, msg) \
    printf("WARNING:: (%s:%d) %s\n", file, line, msg)

static bool initialized = false;

typedef struct {
    size_t address;
    size_t size;
    char file[255];
    uint32_t line;
} Mem;

static struct MemData {
    Mem mem[LEAK_MEM_SIZE];
    uint32_t current;
    uint32_t allocations;
    uint32_t free;
    size_t total_allocated;
    size_t total_freed;
} memoryData;

static bool _insert(void *ptr, size_t size, int line, char *file) {
    uint32_t i;
    const size_t address = (size_t)ptr;
    if ((i = memoryData.current) < LEAK_MEM_SIZE) {
        memoryData.mem[i].address = address;
        memoryData.mem[i].size = size;
        memoryData.mem[i].line = line;
        strcpy(memoryData.mem[i].file, file);

        memoryData.current++;
        memoryData.allocations++;
        memoryData.total_allocated += size;
        return true;
    }
    return false;
}

/**
 * @return: 0 if success else -1
*/
static uint8_t _delete(void *ptr) {
    const size_t address = (size_t)ptr;

    if (ptr != NULL) {
        for (int i=0; i<LEAK_MEM_SIZE; i++) {
            if (address == memoryData.mem[i].address) {
                memoryData.mem[i].address = 0;

                memoryData.free++;
                memoryData.total_freed += memoryData.mem[i].size;
                return true;
            }
        }
    }

    memoryData.free++;
    return false;
}

void _generate_report() {
    printf("/*========= SUMMARY =========*/\n");
    printf("  Total allocations      %d  \n", memoryData.allocations);
    printf("  Total Free             %d  \n", memoryData.free);
    printf("  Total Memory allocated %lu bytes \n", memoryData.total_allocated);
    printf("  Total Memory freed     %lu bytes \n", memoryData.total_freed);
    printf("  Memory Leaked          %lu bytes \n", memoryData.total_allocated - memoryData.total_freed);

    if (memoryData.total_freed == memoryData.total_allocated) return;
    printf("\n/*===== DETAILED REPORT =====*/\n");

    for (int i=0; i<LEAK_MEM_SIZE; i++) {
        if (memoryData.mem[i].address != 0) {
            printf("Memory leak at %s:%d (%lu bytes)\n", 
                memoryData.mem[i].file,
                memoryData.mem[i].line,
                memoryData.mem[i].size);
        }
    }
    printf("==============================\n");
}

void init() {
    if (!initialized) {
        // printf("initializing...\n");
        atexit(_generate_report);
        initialized = true;
    }
}

void *_malloc(size_t size, char *file, int line) {
    init();

    void *ptr = malloc(size);

    if (ptr == NULL) {
        // do something
        _leak_warn(file, line, "Memory allocation failed");
        return ptr;
    }

    _insert(ptr, size, line, file);
    return ptr;
}

void *_calloc(size_t num, size_t size, char *file, int line) {
    init();

    void *ptr = calloc(num, size);
    if (ptr == NULL) {
        _leak_warn(file, line, "Memory allocation failed");
        return ptr;
    }

    _insert(ptr, num * size, line, file);
    return ptr;
}

void *_realloc(void *ptr, size_t size, char *file, int line) {
    if (ptr == NULL) {
        _leak_warn(file, line, "Tried to free a 'NULL' pointer");
    }

    void *new_ptr = realloc(ptr, size);

    if (new_ptr == NULL) {
        _leak_warn(file, line, "Memory allocation failed");
        return ptr;
    }

    if (!_delete(ptr)) {
        _leak_warn(file, line, "Double free detected");
        exit(EXIT_FAILURE);
    }
    _insert(new_ptr, size, line, file);

    return new_ptr;
}

void _free(void *ptr, char *file, int line) {
    if (ptr == NULL) {
        _leak_warn(file, line, "Tried to free a 'NULL' pointer");
    }

    free(ptr);
    if (!_delete(ptr)) {
        _leak_warn(file, line, "Double free detected");
        exit(EXIT_FAILURE);
    }
}

// Redefine allocator functions
#define malloc(size) _malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) _realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) _free(ptr, __FILE__, __LINE__)
#define generate_report() _generate_report(__FILE__)

#ifdef __cpluscplus
}
#endif // __cpluscplus

#endif // __LEAK_DETECTOR_H_
