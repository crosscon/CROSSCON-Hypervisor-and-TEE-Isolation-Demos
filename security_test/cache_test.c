// set in Makefile
#include DEVICE_CONFIGURATION
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <libflush/libflush.h>
#include <asm/unistd.h>

// should be at least as long as the time it takes evict to finish one loop
#define TIME_USLEEP 400*1000
// how many samples to keep (and calculate median from). More samples results in
// timing jumping less but it takes longer for it to register changes
#define TIMING_SAMPLES 25

enum OP {
    EVICT,  // try to evict everything from cache
    TIME,   // time accesses
};

struct Params {
    enum OP op;
    size_t *cache_lines;
    size_t count;
} params;

// Moves cursor to beginning of the previous line
const char CPL[] = "\033[F";
static volatile atomic_bool stop = false;
libflush_session_t* libflush_session;
// printed at the end to stop compiler from optimizing out code without any
// visible use.
uint8_t dummy_value;

/** Parse params and save them to 'params' global struct */
int parse_params(int argc, char **argv);
/** Prepare program, initialize libflush library */
int prepare();
/** Cleanup everything prepared in prepare() */
void cleanup();
/** Writes random byte to <addr, addr+size> each LINE_LENGTH bytes */
void access_range(volatile uint8_t *addr, size_t size);
/**
 * Access all cache lines in params.cache_lines and return how long it took.
 * Cache line <n> == addr[n*LINE_LENGTH]
 */
uint64_t time_access(volatile uint8_t *addr);
/** Handle CTRL+C */
void intHandler(int dummy);
/** Push new element to the top of array, remove last */
void push(void *arr, size_t count, size_t el_size, void *element);
/** Compare 2 uin64_t values */
int cmp_uint64(const void* a, const void* b);
/** Return biggest value in array */
uint64_t max_st(size_t *arr, size_t size);
/** Push new_time to timings array and then return median value */
uint64_t add_element_get_median(uint64_t *timings, uint64_t new_time);

int main(int argc, char **argv) {
    signal(SIGINT, intHandler);
    if (parse_params(argc, argv)) {
        return -1;
    }
    if (prepare()) {
        return -1;
    }

    uint64_t timings[TIMING_SAMPLES] = { 0 };
    uint64_t median_baseline = 0;
    uint64_t median = 0;
    uint64_t time = 0;

    volatile uint8_t *vm_mem;
    if (params.op == EVICT) {
        // allocate LLC_SIZE bytes. Reading all of it should put our data into
        // all/most of cache lines in cache
        vm_mem = malloc(LLC_SIZE);
    }
    else {
        // allocate enough memory to access highest index passed to program
        size_t max_line = max_st(params.cache_lines, params.count);
        size_t malloc_size = (max_line + 1)*LINE_LENGTH;
        vm_mem = malloc(malloc_size);
    }

    // calculate baseline time it takes to access addresses
    if (params.op == TIME) {
        printf("Calculating median baseline. Don't evict.\n");
        for (int i = 0; i < TIMING_SAMPLES; ++i) {
            median_baseline = add_element_get_median(timings, time_access(vm_mem));
        }
        printf("Calculated median time: %lu\n", median_baseline);
    }

    while(!stop) {
        switch(params.op) {
            case EVICT:
                time = libflush_get_timing(libflush_session);
                access_range(vm_mem, LLC_SIZE);
                time = libflush_get_timing(libflush_session) - time;
                printf("\rEviction time: %0.3f ms   ", (float)time / 1000);
                break;
            case TIME:
                time = time_access(vm_mem);
                median = add_element_get_median(timings, time);
                if (median > median_baseline)
                    printf("\rMedian time diff from baseline:  %lu    ", median - median_baseline);
                else
                    printf("\rMedian time diff from baseline: -%lu    ", median_baseline - median);
                fflush(NULL);
                usleep(TIME_USLEEP);
                break;
        }
    }

    printf("\nStopping\n");
    cleanup();
    free((void*)vm_mem);
    printf("Dummy value: %u\n", (unsigned int)dummy_value);
    return 0;
}

void intHandler(int dummy) {
    if (stop) {
        exit(1);
    }
    stop = true;
}

int parse_params(int argc, char **argv) {
    char usage_str[] = "%s <evict|time> <cache_line> [cache_line]...\n";
    if (argc < 2) {
        fprintf(stderr, usage_str, argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "evict") == 0) {
        params.op = EVICT;
    }
    else if (strcmp(argv[1], "time") == 0) {
        if (argc < 3) {
            fprintf(stderr, usage_str, argv[0]);
            return -1;
        }
        params.op = TIME;
    }
    else {
        fprintf(stderr, usage_str, argv[0]);
        return -1;
    }
    params.count = argc - 2;
    params.cache_lines = malloc((params.count) * sizeof(size_t));
    for (int i = 0; i < params.count; ++i) {
        params.cache_lines[i] = (size_t)strtoul(argv[i + 2], NULL, 10);
    }

    return 0;
}

int prepare() {
    printf("Libflush init\n");
    if (libflush_init(&libflush_session, NULL) == false) {
        return -1;
    }

    return 0;
}

void cleanup() {
    free(params.cache_lines);
    if (libflush_terminate(libflush_session) == false) {
        fprintf(stderr, "libflush_terminate failed\n");
    }
}

void access_range(volatile uint8_t *addr, size_t size) {
    uint8_t rnd = rand() % 256;
    for (int i = 0; i < size; i += LINE_LENGTH) {
        addr[i] = rnd;
    }
}

uint64_t time_access(volatile uint8_t *addr) {
    uint64_t time = libflush_get_timing(libflush_session);
    for (int i=0; i<params.count; i++) {
        dummy_value ^= *(addr + params.cache_lines[i]*LINE_LENGTH);
    }
    time = libflush_get_timing(libflush_session) - time;
    return time;
}

void push(void *arr, size_t count, size_t el_size, void *element) {
    void *end = arr + (count - 1)*el_size;
    while (end > arr) {
        memcpy(end, end-el_size, el_size);
        end -= el_size;
    }
    memcpy(arr, element, el_size);
}

int cmp_uint64(const void* a, const void* b)
{
    uint64_t arg1 = *(const uint64_t*)a;
    uint64_t arg2 = *(const uint64_t*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

uint64_t add_element_get_median(uint64_t *timings, uint64_t new_time) {
    uint64_t timings_sorted[TIMING_SAMPLES];
    push(timings, TIMING_SAMPLES, sizeof(new_time), &new_time);
    memcpy(timings_sorted, timings, TIMING_SAMPLES*sizeof(new_time));
    qsort(timings_sorted, TIMING_SAMPLES, sizeof(new_time), cmp_uint64);
    return timings_sorted[TIMING_SAMPLES/2];
}

uint64_t max_st(size_t *arr, size_t size) {
    size_t max_val = 0;
    for (int i = 0; i < size; ++i) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}
