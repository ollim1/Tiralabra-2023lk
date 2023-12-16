# Testing document

## Tested items

- functionality of algorithm components, e.g. PriorityQueue
- correctness of algorithm output (decompressed output is the same as input before compression)
- effectiveness of algorithms (compression ratio)

## Unit testing

Unit tests are implemented using the Check framework.
They cover expected basic functionality like adding to a queue or buffer and struct initialization.

Run unit tests with the command `make check`.
Algorithm-specific tests are accessible with `make check-huffman` and `make check-lzss`.
For this, the package `check` should be installed from your distribution's package manager.

## Comparison of algorithms

Comparison testing was done using multiple kinds of input files.
Image files have been compressed in PNG format. Convert them to BMP for testing.
Run `./compressor -ba <algorithm> -i <file>` to reproduce.

During testing the primary metric of comparison is compression ratio of input. LZSS is a slow algorithm even optimized, so time spent compressing should not matter.

Compression ratio means ratio of uncompressed file to compressed output. Measurements were performed on an Intel i7-4770K.

### Benchmark results

- bliss.bmp (12MiB), photograph downscaled from <https://www.reddit.com/r/windows/comments/y2h5q2/super_hires_blissbmp_ai_upscaled/>
    - Huffman
        - compression 0.0489 s, extraction 0.0849 s
        - compression ratio 107%
    - LZSS
        - compression 429 s, extraction 0.0171 s
        - compression ratio 114%
    - LZSS-byte
        - compression 428 s, extraction 0.0132 s
        - compression ratio 109%
    - LZHF (nested LZSS-byte inside Huffman)
        - compression 431 s, extraction 0.0864 s
        - compression ratio 121%
- linux.bmp (17MiB), infographic (<https://upload.wikimedia.org/wikipedia/commons/3/33/Linux_Distro_Timeline.png>)
    - Huffman
        - compression 0.0247 s, extraction 0.0215 s
        - compression ratio 622%
    - LZSS
        - compression 42.1 s, extraction 0.00390 s
        - compression ratio 646%
    - LZSS-byte
        - compression 43.0 s, extraction 0.00350 s
        - compression ratio 466%
    - LZHF
        - compression 43.0 s, extraction 0.0108 s
        - compression ratio 1604%
- loremipsum-100k.txt (99KiB), ASCII text (<https://www.lipsum.com>)
    - Huffman
        - compression 0.000291 s, extraction 0.000446 s
        - compression ratio 187%
    - LZSS
        - compression 1.06 s, extraction 0.0000660 s
        - compression ratio 294%
    - LZSS-byte
        - compression 1.06 s, extraction 0.0000615 s
        - compression ratio 216%
    - LZHF
        - compression 1.06 s, extraction 0.000374 s
        - compression ratio 273%
- ff.bin (99KiB), file consisting of just `0xff` bytes
    - Huffman
        - compression 0.0000372 s, extraction 0.0000908 s
        - compression ratio 2020000%
    - LZHF
        - compression 0.00138 s, extraction 0.0000361 s
        - compression ratio 550%
    - LZHF
        - compression 0.00135 s, extraction 0.0000377 s
        - compression ratio 366%
    - LZHF
        - compression 1.06 s, extraction 0.000374 s
        - compression ratio 1908%
