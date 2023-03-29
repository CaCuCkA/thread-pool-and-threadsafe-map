# Thread pool & Threadsafe map

## Prerequisites

❯ CMake

❯ GCC

❯ Git

❯ C++ IDE (optional). CLion is recommended.


> **Note**
>
> To install `CMake` you should run this code on your computer: <br>
> **Important this code works only on Ubuntu!**
> ```bash
> $ sudo apt install cmake
> ```
> To find out how to download to your operating system follow this
> <a href="https://cmake.org/install/">link</a>.
>
> For compiler installation, please, refer to the official documentation of your compiler.
> For example GCC <a href="https://gcc.gnu.org/">link</a>.

## Compilation

1. You should compile our project run the `compile.sh` file with `-c` flag:

```bash
$ ./compile.sh -c # compile project
# or
$ ./compile.sh --compile # compile project
```

2. If you want to get rid of executable files, run the `compile.sh` file with `--clean` flag:

```bash
$ ./compile.sh --clean
```
> **Note**
>
> If you get this error when running `compile.sh`:
> ```bash
> $ -bash: ./compile.sh: /bin/bash^M: bad interpreter: No such file or directory
> ``` 
> enter that code:
> ```bash
> $ sed -i -e 's/\r$//' compile.sh
> ```

## Installation

To install our project, you need to clone the repository first:

```bash
$ mkdir ~/workdir
$ cd ~/workdir
$ git clone https://github.com/CaCuCkA/thread-pool-and-threadsafe-map.git
$ cd thread-pool-and-threadsafe-map
```

> Replace `~/workdir` with the path to your workspace.


## Important 
A description of how `thread pool` and `threadsafe map` work can be found in `THREAD_POOL.md` and `THREADSAFE_MAP.md`. 









