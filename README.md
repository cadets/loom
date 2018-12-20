# Loom: LLVM instrumentation library

<img align="right"
src="https://raw.githubusercontent.com/cadets/loom/master/doc/images/loom-small.png"/>

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

2. Configure Loom build:
```sh
$ cd /path/to/Loom
$ mkdir Release
$ cd Release
$ cmake -G Ninja -D CMAKE_BUILD_TYPE=Release ..
```

3. Build Loom:
```sh
$ ninja
```

4. (optional) run test suite:

    a. Install [libxo](https://github.com/Juniper/libxo), which is used by
       Loom to emit structured output:
```sh
$ pkg install libxo    # or whatever your package manager command is
```
    b. Run the Loom test suite:
```sh
$ ninja check
```


## Use it

### `opt` pass

Loom is structured as a set of libraries for LLVM instrumentation,
but can be run most straightforwardly as an LLVM
[opt](http://llvm.org/docs/CommandGuide/opt.html) pass.
This requires creating an instrumentation policy file such as:
```yaml
#
# There are two instrumentation strategies:
#  * callout:  call an instrumentation function like __loom_called_foo
#  * inline    inject instrumentation inline with instrumented events
#
strategy: callout

#
# When using callout instrumentation, the generated instrumentation functions
# are named __loom_{event-specific-name} by default. This configuration value
# allows the `__loom` prefix to be overridden.
#
hook_prefix: __test_hook

#
# Loom can automatically log events and their immediate values (e.g., when
# logging a call to foo(1, 2, 3.1415), emit "foo", 1, 2 and 3.1415) without
# any processing. Strategies include:
#
#  * none      do not use the simple logger
#  * printf    just print the event using printf()
#  * xo        use libxo to emit a structured representation (e.g., json)
#
logging: printf

#
# Loom can report events via FreeBSD's ktrace(1) mechanism, either from
# the kernel ("kernel") or from userspace via the utrace(2) system call.
#
ktrace: utrace

#
# Loom currently serializes event information for, e.g., ktrace reporting
# using libnv. Support for other serialization schemes can be added.
#
serialization: nv

#
# Specify how/when functions should be instrumented.
#
# Functions can be instrumented on the caller side (before/after the call)
# or the callee side (function prelude / return sites). This configuration
# value should be a list of entries, each specifying:
#
#  * `name`: the name of the function being instrumented (language-mangled)
#  * `caller`: (optional) list of directions to instrument calls (entry/exit)
#  * `callee`: (optional) list of directions to instrument functions
#
functions:
    - name: foo
      caller: [ entry, exit ]

    - name: bar
      caller: [ entry ]
      callee: [ exit ]

#
# Specify how/when structure fields should be instrumented.
#
# We can instrument both reads and writes to structure fields.
# These fields must currently be identified by number rather than name.
# The `structures` config value is a list of entries containing:
#
#  * `name`: the structure name
#  * `fields`: a list of structure field descriptions:
#    * `name`: field name
#    * `operations`: list of operations to instrument (`read` or `write`)
#
structures:
  - name: baz
    fields:
      - name: refcount
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

To build FreeBSD with Loom, you need to clone our IR-augmented version of FreeBSD. Then, you can use our `loom-fbsdmake` script as a drop-in wrapper around `make`:

```sh
$ git clone https://github.com/cadets/freebsd -b cadets freebsd-cadets
$ cd freebsd-cadets
$ export LLVM_PREFIX=/path/to/LLVM/Release
$ export LOOM_LIB=/path/to/Loom/Release/lib/LLVMLoom.so
$ /path/to/Loom/scripts/loom-fbsdmake buildworld buildkernel
$ /path/to/Loom/scripts/loom-fbsdmake buildenv   # etc.
```
