# Design

This project is done as a part of the Computer Science study program of University of Helsinki (TKT). In this project two algorithms will be implemented for compression rate comparisons.

The programming language of choice is C. In addition to this I can use Java, C++, Haskell and Python. The project documentation is written in English

The algorithms to be implemented are Huffman and Lempel-Ziv-Storer-Szymanski (LZSS).

## Data structures

Huffman: Huffman tree. Implemented as a priority queue.

Lempel-Ziv: Array.

## Time Complexity

Lempel-Ziv progresses linearily in the input with a fixed size window for compression, resulting in a time complexity of O($n$).

Huffman uses a tree structure that is accessed on every character of input, seemingly leading to a time complexity of O($n$ log $n$).
However, due to there being only 256 different possible byte values the tree size comes out to 256 leaves + 255 nodes = 511.
This makes the algorithm's true time complexity O(n).

## Input/Output

The program will use take input from binary or text files and stdin. The compression and decompression algorithms to be used may be specified through command line flags. The program will output to stdout or a file.

## Sources
[The Hitchhiker's Guide to Compression](https://go-compression.github.io/algorithms/huffman/)
[An efficient LZW implementation](http://warp.povusers.org/EfficientLZW/)
