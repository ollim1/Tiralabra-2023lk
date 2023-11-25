# Design

This project is done as a part of the Computer Science study program of University of Helsinki (TKT). In this project two algorithms will be implemented for compression rate comparisons.

The programming language of choice is C. In addition to this I can use Java, C++, Haskell and Python. The project documentation is written in English

The algorithms to be implemented are Huffman and Lempel-Ziv-Storer-Szymanski (LZSS).

## Data structures
Priority queue for Huffman. LZSS does not require any abstract data structures.

### Huffman
The Huffman tree is implemented as a complete binary tree. This tree is serialized by performing a depth-first search in the tree and storing the node information in the order of traversal.  

A priority queue implemented as a linked list is used for sorting Huffman nodes.

### Lempel-Ziv

A ring buffer is used for the dictionary window.

## Time Complexity

Huffman uses a tree structure that is accessed on every character of input, seemingly leading to a time complexity of O($n$ log $n$).
However, due to there being only 256 different possible byte values the tree size comes out to 256 leaves + 255 nodes = 511.
This makes the algorithm's true time complexity O(n).

Lempel-Ziv progresses linearily in the input with a fixed size window for compression, resulting in a time complexity of O($n$).

## Compression Ratio

As maximum tree size for 8-bit Huffman coding is 256 bits + 256 bytes for 256 leaf nodes and 255 bits for parent nodes. In addition, the decoded length of the file is stored in byte-packed form that takes an additional bit for each byte. Decoded file length is limited to the maximum value of `size_t`, which is effectively $2^64$. This results in at most 328 bytes of overhead.

Huffman coding has a best case compression ratio of 8:1 when files consist of more than 1 kind of character.

## Input/Output

The program will use take input from binary or text files and stdin. The compression and decompression algorithms to be used may be specified through command line flags. The program will output to stdout or a file.

## Sources
[The Hitchhiker's Guide to Compression](https://go-compression.github.io/algorithms/huffman/)
[An efficient LZW implementation](http://warp.povusers.org/EfficientLZW/)
