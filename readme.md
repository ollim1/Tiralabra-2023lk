# Tiralabra 2023/LK
[![codecov](https://codecov.io/gh/ollim1/Tiralabra_2023lk/branch/main/graph/badge.svg?token=1M4X0W2FRS)](https://codecov.io/gh/ollim1/Tiralabra_2023lk)
[![Tests](https://github.com/ollim1/Tiralabra_2023lk/actions/workflows/build.yml/badge.svg)](https://github.com/ollim1/Tiralabra_2023lk/actions/workflows/build.yml)

A file compressor with two algorithms available: Huffman, Lempel-Ziv. Implemented in C. A software project done for the [Data Structures and Algorithms project course](https://tiralabra.github.io/2023_loppukesa/index) at University of Helsinki as part of the Computer Science study program (TKT).

## Documentation links

### [Design document](doc/design.md)
### [Testing document](doc/testing.md)

## Weekly reports
### [Week 1](doc/week1.md)
### [Week 2](doc/week2.md)
### [Week 3](doc/week3.md)

## How to build
### Requirements
- C compiler (clang or gcc)
- Make
- Check
- `gcovr` on Linux for coverage reports (broken on Apple Silicon Macs)
- Python for auto-formatting

### Compilation
To build the software, run `make` in the root directory of the repository. This produces the binary `compressor`, which can be run by executing `./compressor`.

### Testing
Unit tests are set up in the make file under the command `make check`.

### Code formatting
Code auto-formatting can be applied with `make format`. This requires `clang-format` and `python` to be installed (Debian-based systems may need `python-is-python3`).

### Code coverage
Run `make coverage-html`. The report will be written to `coverage.html`

## Usage
Running `compressor` with no arguments will cause usage information to be printed.
