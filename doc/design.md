# Design

This project is done as a part of the Computer Science study program of University of Helsinki (TKT).
In this project two algorithms will be implemented for compression rate comparisons.

The programming language of choice is C. In addition to this I can use Java, C++, Haskell and Python.
The project documentation and code are written in English.

The algorithms to be implemented are Huffman and Lempel-Ziv-Storer-Szymanski (LZSS).
LZSS is implement in two different variants: bit-level in which every byte is prefixed by a "token" bit,
and byte-level where tokens are marked by `0xff` escape values.

## Data structures
Priority queue for Huffman. LZSS uses a ring buffer.

### Huffman
The Huffman tree is implemented as a complete binary tree.
This tree is serialized by performing a depth-first search in the tree and storing the node information in the order of traversal.  

A priority queue implemented as a linked list is used for sorting Huffman nodes.

### Lempel-Ziv

A ring buffer is used for the dictionary window.
String searches in the dictionary are optimized using the Knuth-Morris-Pratt algorithm.

## Time Complexity

Huffman uses a tree structure that is accessed on every character of input, seemingly leading to a time complexity of O($n$ log $n$).
However, due to there being only 256 different possible byte values the tree size comes out to 256 leaves + 255 nodes = 511.
This makes the algorithm's true time complexity O(n).

Lempel-Ziv progresses linearily in the input with a fixed size window for compression.
This implementation performs a KMP search in O($d \times w$) time but has a best case of &Theta;($w$),
where $w$ is the lmaximum word length and $d$ the maximum length of the sliding window dictionary.
Overall time complexity is O($n \times d \times w).

## Compression Ratio

### Huffman

As maximum tree size for 8-bit Huffman coding is 256 bits + 256 bytes for 256 leaf nodes and 255 bits for parent nodes.
In addition, the decoded length of the file is stored in byte-packed form that takes an additional bit for each byte.
Decoded file length is limited to the maximum value of `size_t`, which is effectively $2^64$.
This results in at most 328 bytes of overhead.

Huffman coding has a best case compression ratio of 800% when files consist of more than one byte value. Files consisting of only a single value contain only the file length and a single tree node.

### LZSS
Bit-level: tokens take 17 bits to encode while literals take 9 bits. Worst case space compression ratio is therefore 89%, while best case is 706%, calculated by taking the compression ratio between 15 literals and the 17-bit token representation.

Byte-level: tokens take 3 bytes to encode while literals take 1-2 bytes. Tokens are marked with a `0xff` byte followed by either `0x00` meaning a `0xff` literal or a value containing the lower 8 bytes of the token, which is always nonzero. Best case compression ratio is 500%, calculated by taking the compression ratio between 15 literals and the respective 3-byte token.

## Input/Output

The program will use take input from binary or text files and stdin.
The compression and decompression algorithms to be used may be specified through command line flags.
The program will output to stdout or a file.

## Sources
[The Hitchhiker's Guide to Compression](https://go-compression.github.io/algorithms/huffman/)
[An efficient LZW implementation](http://warp.povusers.org/EfficientLZW/)
[Knuth–Morris–Pratt algorithm](https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm)
[LZSS (LZ77) Discussion and Implementation](https://michaeldipperstein.github.io/lzss.html)
