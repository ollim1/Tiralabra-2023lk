## How to build
### Requirements

- C compiler (clang or gcc)
- Make
- Check framework (`libcheck`)
- `gcovr` on Linux for coverage reports (broken on Apple Silicon Macs)
- Python for auto-formatting

### Compilation

To build the software, run `make` in the root directory of the repository. This produces the binary `compressor`, which can be run by executing `./compressor`.

### Testing

Unit tests are set up in the make file under the command `make check`.

Algorithm-specific tests can be run with the commands `make check-huffman` and `make check-lzss`.

### Code formatting

Code auto-formatting can be applied with `make format`. This requires `clang-format` and `python` to be installed (Debian-based systems may need `python-is-python3`).

### Code coverage

Run `make coverage-html`. The report will be written to `coverage.html` in the project root directory.

## Usage

Running `compressor` with no arguments will cause usage information to be printed.

```Usage:
-a [algorithm]: compression algorithm; huff, lzss, lzss-byte or lzhf
-e: extract (default huffman)
-c: compress (default huffman)
-i [infile]: set input file, - for stdin (default)
-o [outfile]: set output file, - for stdout (default)
-b: benchmark algorithm performance without saving output
Example: ./compressor -ba lzhf -i samples/loremipsum-100k.txt```
