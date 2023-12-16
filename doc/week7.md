# Week 7

- write documentation
- start writing more complete documentation on functions
- fix LZSS on certain problem files
    - 10M lorem ipsum (6 kilobytes is fine)
    - 10 megabyte bliss.bmp
    - issue turned out to be caused by appending part of a buffer to itself, causing reference to deallocated memory after a buffer resize
- implement KMP algorithm in LZSS, relatively plug and play besides the algorithm searching from the beginning of the buffer instead of the end
- add benchmarking code
- refactor main.c
- refactor string lookup into its own common file
- perform benchmarks
- fix possible bitarray to buffer conversion bug ((length / 8) + 1)

## Work Hours
| Date   | Time Spent |
| :----- | ---------: |
| 11.12. | 2.5h       |
| 13.12. | 3.5h       |
| 14.12. | 4.5h       |
| 15.12. | 2.5h       |
| 16.12. | 7h         |
