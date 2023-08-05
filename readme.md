# Tiralabra 2023/LK

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
- Python for style checking

### Compilation
To build the software, run `make` in the root directory of the repository. This produces the binary `compressor`, which can be run by executing `./compressor`.

### Testing
Unit tests are set up in the make file under the command `make check`.

### Code formatting

