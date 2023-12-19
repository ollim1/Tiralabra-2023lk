#define _DEFAULT_SOURCE
#include "../include/error.h"
#include "../include/fileops.h"
#include "../include/huffman.h"
#include "../include/lzss.h"
#include "../include/lzss_byte.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

enum algorithm_enum { HUFFMAN = 0, LZSS, LZSS_BYTE, LZHF };
enum mode_enum { COMPRESS = 0, EXTRACT, BENCHMARK };

void usage();
Buffer *processData(Buffer *data, Buffer *(*algorithmFunction)(Buffer *), enum mode_enum mode);
void benchmark(Buffer *data, enum algorithm_enum algorithm);

Buffer *lzhf_compress(Buffer *data);
Buffer *lzhf_extract(Buffer *data);

int main(int argc, char **argv)
{
    int ch;
    char *infile = "-", *outfile = "-";
    enum algorithm_enum algorithm = HUFFMAN;
    enum mode_enum mode = BENCHMARK;
    Buffer *(*algorithmFunction)(Buffer *) = NULL;
    if (argc == 1)
        usage();

    while ((ch = getopt(argc, argv, "a:bcei:o:")) != -1) {
        switch (ch) {
        case 'a':
            if (strcmp(optarg, "huffman") == 0 || strcmp(optarg, "huff") == 0) {
                algorithm = HUFFMAN;
            } else if (strcmp(optarg, "lzss") == 0) {
                algorithm = LZSS;
            } else if (strcmp(optarg, "lzss-byte") == 0) {
                algorithm = LZSS_BYTE;
            } else if (strcmp(optarg, "lzhf") == 0 || strcmp(optarg, "lzss-huff") == 0 ||
                       strcmp(optarg, "lzss-huffman") == 0) {
                algorithm = LZHF;
            } else
                fprintf(stderr, "Unknown algorithm: %s\n", optarg);
            break;
        case 'b':
            mode = BENCHMARK;
            break;
        case 'c':
            mode = COMPRESS;
            break;
        case 'e':
            mode = EXTRACT;
            break;
        case 'i':
            infile = optarg;
            break;
        case 'o':
            outfile = optarg;
            break;
        default:
            usage();
        }
    }
    fprintf(stderr, "reading from file %s\n", infile);
    Buffer *data = readFile(infile);
    if (data->len == 0) {
        err_quit("file is empty, skipping");
    }
    Buffer *processed = NULL;
    switch (mode) {
    case COMPRESS:
        fprintf(stderr, "will compress\n");
        switch (algorithm) {
        case HUFFMAN:
            fprintf(stderr, "using huffman algorithm\n");
            algorithmFunction = huffman_compress;
            break;
        case LZSS:
            fprintf(stderr, "using lzss algorithm\n");
            algorithmFunction = lzss_compress;
            break;
        case LZSS_BYTE:
            fprintf(stderr, "using lzss-byte algorithm\n");
            algorithmFunction = lzss_byte_compress;
            break;
        case LZHF:
            fprintf(stderr, "using nested lzss-byte and huffman algorithms\n");
            algorithmFunction = lzhf_compress;
            break;
        default:
            break;
        }
        break;
    case EXTRACT:
        fprintf(stderr, "will extract\n");
        switch (algorithm) {
        case HUFFMAN:
            fprintf(stderr, "using huffman algorithm\n");
            algorithmFunction = huffman_extract;
            break;
        case LZSS:
            fprintf(stderr, "using lzss algorithm\n");
            algorithmFunction = lzss_extract;
            break;
        case LZSS_BYTE:
            fprintf(stderr, "using lzss-byte algorithm\n");
            algorithmFunction = lzss_byte_extract;
            break;
        case LZHF:
            fprintf(stderr, "using nested lzss-byte and huffman algorithms\n");
            algorithmFunction = lzhf_extract;
            break;
        default:
            break;
        }
        break;
    case BENCHMARK:
        fprintf(stderr, "will benchmark\n");
        break;
    default:
        err_quit("no mode set, exiting");
        break;
    }

    if (mode != BENCHMARK) {
        if (!algorithmFunction)
            err_quit("no algorithm set, exiting");

        processed = processData(data, algorithmFunction, mode);
        fprintf(stderr, "writing to file %s\n", outfile);
        writeFile(processed, outfile);
        delete_buffer(processed);
    } else {
        benchmark(data, algorithm);
    }
    delete_buffer(data);
}

void benchmark(Buffer *data, enum algorithm_enum algorithm)
{
    if (data->len == 0)
        err_quit("file is empty, skipping");
    Buffer *(*compressFunction)(Buffer *) = NULL;
    Buffer *(*extractFunction)(Buffer *) = NULL;
    Buffer *compressed;
    Buffer *decompressed;
    switch (algorithm) {
    case HUFFMAN:
        fprintf(stderr, "testing huffman algorithm\n");
        compressFunction = huffman_compress;
        extractFunction = huffman_extract;
        break;
    case LZSS:
        fprintf(stderr, "testing lzss algorithm\n");
        compressFunction = lzss_compress;
        extractFunction = lzss_extract;
        break;
    case LZSS_BYTE:
        fprintf(stderr, "testing lzss-byte algorithm\n");
        compressFunction = lzss_byte_compress;
        extractFunction = lzss_byte_extract;
        break;
    case LZHF:
        fprintf(stderr, "testing nested lzss-byte and huffman algorithms\n");
        compressFunction = lzhf_compress;
        extractFunction = lzhf_extract;
        break;
    default:
        err_quit("no valid algorithm set for benchmark");
        break;
    }
    compressed = processData(data, compressFunction, COMPRESS);
    decompressed = processData(compressed, extractFunction, EXTRACT);

    if (!buffer_equals(data, decompressed))
        err_quit("mismatch between original and decompressed data");

    delete_buffer(compressed);
    delete_buffer(decompressed);
}

Buffer *processData(Buffer *data, Buffer *(*algorithmFunction)(Buffer *), enum mode_enum mode)
{
    struct timeval before, after, difference;
    gettimeofday(&before, NULL);

    Buffer *processed = algorithmFunction(data);

    gettimeofday(&after, NULL);
    timersub(&after, &before, &difference);
    fprintf(stderr, "%s took %ld.%07ld seconds\n", mode == COMPRESS ? "compression" : "extraction",
            (long int) difference.tv_sec, (long int) difference.tv_usec);
    if (mode == COMPRESS)
        fprintf(stderr, "compression ratio is %.1lf%%\n",
                ((double) data->len) / ((double) processed->len) * 100.0);
    else
        fprintf(stderr, "compression ratio is %.1lf%%\n",
                ((double) processed->len) / ((double) data->len) * 100.0);

    return processed;
}

Buffer *lzhf_compress(Buffer *data)
{
    Buffer *lzss_compressed = lzss_byte_compress(data);
    Buffer *output = huffman_compress(lzss_compressed);
    delete_buffer(lzss_compressed);
    return output;
}

Buffer *lzhf_extract(Buffer *data)
{
    Buffer *huffman_extracted = huffman_extract(data);
    Buffer *output = lzss_byte_extract(huffman_extracted);
    delete_buffer(huffman_extracted);
    return output;
}

void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "-a [algorithm]: compression algorithm; huff, lzss, lzss-byte or lzhf\n");
    fprintf(stderr, "-e: extract (default algorithm huffman)\n");
    fprintf(stderr, "-c: compress (default algorithm huffman)\n");
    fprintf(stderr, "-i [infile]: set input file, - for stdin (default)\n");
    fprintf(stderr, "-o [outfile]: set output file, - for stdout (default)\n");
    fprintf(stderr, "-b: benchmark algorithm performance without saving output\n");
    fprintf(stderr, "Example: ./compressor -ba lzhf -i samples/loremipsum-100k.txt\n");
    exit(EXIT_FAILURE);
}
