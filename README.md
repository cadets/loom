# Loom: LLVM instrumentation library

Loom is a general-purpose library for adding instrumentation to software in the LLVM intermediate representation (IR) format. It is currently capable of generating static instrumentation points that expose values (e.g., function parameters and return values) to software-defined instrumentation functions.

Loom is part of the [CADETS](https://www.cl.cam.ac.uk/research/security/cadets) (Causal, Adaptive, Distributed, and Efficient Tracing System) project as part of DARPA's [Transparent Computing](http://www.darpa.mil/program/transparent-computing) program.

## Get it

Loom itself is entirely contained within this GitHub repository, but it requires that you build [LLVM](http://llvm.org) from source. In order to run Loom's test suite, you must also build a matching version of [Clang](http://clang.llvm.org).

```sh
$ git clone https://github.com/cadets/llvm -b cadets llvm
$ git clone https://github.com/cadets/clang -b cadets llvm/tools/clang
$ git clone https://github.com/cadets/loom loom
```


## Build it

### Building Clang/LLVM

First, build LLVM using [CMake](https://cmake.org) and [Ninja](https://ninja-build.org). The directions below assume a `Release` build, but a `Debug` build is also possible.

```sh
$ mkdir llvm/Release
$ cd llvm/Release
$ cmake -G Ninja -D CMAKE_BUILD_TYPE=Release ..
$ ninja
[go get some coffee]
```

### Building LOOM

1. Set your PATH to include the version of LLVM you just built:

    ```sh
    $ export PATH=/path/to/LLVM/Release/bin:$PATH
    ```
    or:
    ```csh
    $ setenv PATH /path/to/LLVM/Release/bin:$PATH
    ```

1. Configure Loom build:
    ```sh
    $ cd /path/to/Loom
    $ mkdir Release
    $ cmake -G Ninja -D CMAKE_BUILD_TYPE=Release ..
    ```

1. Build Loom:
    ```sh
    $ ninja
    ```

1. (optional) run test suite:
    a. Install [libxo](https://github.com/Juniper/libxo), which is used by
       Loom to emit structured
    ```sh
    $ pkg install libxo    # or whatever your package manager command is
    $ ninja check
    ```


## Use it

### `opt` pass

Loom is structured as a set of libraries for LLVM instrumentation, but can be run most straightforwardly as an LLVM [opt](http://llvm.org/docs/CommandGuide/opt.html) pass. This requires creating an instrumentation policy file such as:

```yaml
hook_prefix: __test_hook

functions:
    - name: foo
      caller: [ entry, exit ]

    - name: bar
      caller: [ entry ]
      callee: [ exit ]

structures:
  - name: baz
    fields:
      - id: 0
        operations: [ read, write ]
```

and then running `opt`:

```sh
$ opt -load /path/to/LLVMLoom.so -loom -loom-file /path/to/instr.policy
```

Loom's `opt` pass has options that can be seen in the help/usage output:

```sh
$ opt -load /path/to/LLVMLoom.so -help | grep loom
```


### Instrumenting FreeBSD

To build FreeBSD with Loom, you need to clone our [IR-augmented version of FreeBSD](https://github.com/cadets/freebsd/branches/loom). Then, you can use our `loom-fbsdmake` script as a drop-in wrapper around `make`:

```sh
$ git clone https://github.com/cadets/freebsd -b loom freebsd-loom
$ cd freebsd-loom
$ export LLVM_PREFIX=/path/to/LLVM/Release
$ export LOOM_LIB=/path/to/Loom/Release/lib/LLVMLoom.so
$ /path/to/Loom/scripts/loom-fbsdmake buildworld buildkernel
$ /path/to/Loom/scripts/loom-fbsdmake buildenv   # etc.
```
