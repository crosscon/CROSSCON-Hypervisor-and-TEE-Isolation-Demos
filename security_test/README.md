# Cache attack test

## Preparation

Copy configuration files:

```sh
cp files/* armageddon/libflush/libflush/eviction/strategies/
```

## Build

```sh
BUILDROOT=$ROOT/buildroot/build-aarch64
export CROSS_COMPILE=$BUILDROOT/host/bin/aarch64-linux-
export DESTDIR=./to_buildroot-aarch64
export O=$PWD/out-aarch64
export TIME_SOURCE=perf
export ARCH=armv8
export DEVICE_CONFIGURATION=rpi4
export LLC_SIZE=0x100000

make
```

Additional configuration:

* TIME_SOURCE - possible values: `register`, `perf`, `monotonic_clock`,
  `thread_counter` with `register` being most accurate but requires access to
  specific registers e.g. `PMCCNTR_EL0` for ARMv8.
* LLC_SIZE - size of last level cache

## Run

`cache_test` works in 2 modes

* `time` in which it times how long it took to access memory addresses passed as
  arguments. Each index should map to different cache line (as long as number is
  smaller than number of cache lines e.g. 16384 for rpi4).
  `cache_test time 0 1 2` would measure time it took to access
  `addr`, `addr+64`, `addr+128` where `addr` is memory allocated by `cache_test`
  program.
* `evict` in which program accesses enough addresses to make sure all cache
  lines in cache were replaced by its own data. Accepts no arguments.

To run test:

1. In VM 1 run

    ```sh
    # cache_test time 0 100 56 23 73 12 19
    Libflush init
    Calculating median baseline. Don't evict.
    Calculated median time: 379
    Median time diff from baseline:  69
    ```

    Program should display median time to access all passed cache lines.
    Program calculates rolling median which should be fairly stable. If it isn't
    then test might not work. Wait couple seconds for time diff to stabilize.

2. In VM 2 run

    ```sh
    # cache_test evict
    Libflush init
    Eviction time: 291.416 ms
    ```

    This should evict everything it can from cache by accessing large amount of
    data. It allocates LLC_SIZE bytes of memory and writes to it with
    stride of LINE_LENGTH (cache line size).

Without cache coloring you should see change in median time diff in first VM
after a couple of seconds. How long it takes depends on `TIME_USLEEP` and
`TIMING_SAMPLES` defines in `cache_test.c`, where `TIME_USLEEP` depends on
eviction time (shouldn't be lower than eviction time reported by 2nd VM).
Increasing `TIMING_SAMPLES` will lower variability/spread in reported median
time, but it'll also increase time it takes to report changes.

## Results

Example results, acquired on RPI4 on commit
[8f58f780623c](https://github.com/3mdeb/CROSSCON-Hypervisor-and-TEE-Isolation-Demos/commit/8f58f780623c8691c5b8e9f7a434d02cb9b4f13c).
Commands used are exactly the same as in [Run](#run) section.

### Without cache coloring

* Before eviction

    ```text
    Median time diff from baseline:  48
    ```

* After eviction

    ```text
    Median time diff from baseline:  458
    ```

### With cache coloring

Eviction time jumped to about 600 ms. Tested with original and modified
`TIME_USLEEP` value.

* Before eviction

    ```text
    Median time diff from baseline:  149
    ```

* After eviction

    ```text
    Median time diff from baseline:  163
    ```

With cache coloring enabled median time had much bigger spread with time diff
sometimes jumping up to 240 (with and without evicting).
