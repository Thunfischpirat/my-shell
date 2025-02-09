# C++ Shell Implementation
A C++ implementation of a simple shell interpreter based on the [Build your own Shell](https://app.codecrafters.io/courses/shell/introduction) challenge from CodeCrafters.

## Features

- Basic command execution such as `echo`, `ls`, `pwd`, `cd`, `exit`
- Redirection of standard output and error


## Requirements

- CMake (>= 3.22)
- C++23 compatible compiler
- Catch2 (for testing)

## Building

```bash
cmake -B build
cmake --build build
```

## Running

```bash
./build/shell
```
