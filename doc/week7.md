# Week 7

- write documentation
- start writing more complete documentation on functions
- fix LZSS on certain problem files
    - 10M lorem ipsum (6 kilobytes is fine)
    - 10 megabyte bliss.bmp
    - issue turned out to be caused by appending part of a buffer to itself, causing reference to deallocated memory after to a realloc call
- implement KMP algorithm in LZSS, relatively plug and play besides the algorithm searching from the beginning of the buffer instead of the end
- write benchmarking code

## Work Hours
| Date   | Time Spent |
| :----- | ---------: |
| 11.12. | 2.5h       |
| 13.12. | 3.5h       |
| 14.12. | 4.5h       |
| 15.12. | 2.5h       |
