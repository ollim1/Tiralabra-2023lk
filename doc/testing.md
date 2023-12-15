# Testing document

## Tested items

- functionality of algorithm components, e.g. PriorityQueue
- correctness of algorithm output (decompressed output is the same as input before compression)
- effectiveness of algorithms (compression ratio)

## Unit testing

Unit tests are implemented using the Check framework. They cover expected basic functionality like adding to a queue or buffer and struct initialization.

Run unit tests with the command `make check`. For this, the package `check` should be installed from your distribution's package manager.

## Comparison of algorithms

Comparison testing is implemented using different kinds of input files and averaged over multiple iterations. Files are omitted in order to keep the repository size small.

During testing the primary metric of comparison is compression ratio of input. LZSS is a slow algorithm even optimized, so time spent compressing should not matter.

Compression ratio means ratio of uncompressed file to compressed output.

### Test results

- bliss.bmp (12MB), photograph
    - Huffman
        - compression 0.049 s, extraction 0.085 s
        - compression ratio 107.0%
    - LZSS
        - compression 458.076 s, extraction 0.019 s
        - compression ratio 114.0%
    - LZSS-byte
        - compression 49 ms, extraction 85 ms
        - compression ratio 107.0%
