# Tiralabra 2023/II
[![codecov](https://codecov.io/gh/ollim1/Tiralabra_2023lk/branch/main/graph/badge.svg?token=1M4X0W2FRS)](https://codecov.io/gh/ollim1/Tiralabra_2023lk)
[![Test](https://github.com/ollim1/Tiralabra_2023lk/actions/workflows/test.yml/badge.svg)](https://github.com/ollim1/Tiralabra_2023lk/actions/workflows/test.yml)

A file compressor with two algorithms implemented: Huffman, Lempel-Ziv (LZSS). Written in C. A software project done for the [Algorithms and AI lab course](https://tiralabra.github.io/2023_loppukesa/index) at University of Helsinki as part of the Computer Science study program (TKT).

Continued from 2023/LK.

## Documentation links

### [Design document](doc/design.md)
### [Testing document](doc/testing.md)
### [Manual](doc/manual.md)

## Weekly reports
### [Week 1](doc/week1.md)
### [Week 2](doc/week2.md)
### [Week 3](doc/week3.md)
### [Week 4](doc/week4.md)
### [Week 5](doc/week5.md)
### [Week 6](doc/week6.md)
### [Week 7](doc/week7.md)

## Usage

`cd` to the root directory of the project and run `make` to build it.

Running `compressor` with no arguments will cause usage information to be printed.

```Usage:
-a [algorithm]: compression algorithm; huff, lzss, lzss-byte or lzhf
-e: extract (default huffman)
-c: compress (default huffman)
-i [infile]: set input file, - for stdin (default)
-o [outfile]: set output file, - for stdout (default)
-b: benchmark algorithm performance without saving output```
