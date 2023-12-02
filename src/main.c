#include "error.h"
#include "fileread.h"
#include "huffman.h"
#include "lzss.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage();

int main(int argc, char **argv)
{
    int ch;
    char *infile = "-", *outfile = "-";
    enum algorithm { HUFFMAN = 0, LZSS } algorithm = HUFFMAN;
    enum mode { COMPRESS = 0, EXTRACT } mode = COMPRESS;
    if (argc == 1)
        usage();

    while ((ch = getopt(argc, argv, "a:cei:o:")) != -1) {
        switch (ch) {
        case 'a':
            if (strcmp(optarg, "huffman") == 0 || strcmp(optarg, "huff") == 0) {
                algorithm = HUFFMAN;
            } else if (strcmp(optarg, "lzss") == 0) {
                algorithm = LZSS;
            } else
                fprintf(stderr, "Unknown algorithm: %s\n", optarg);
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
    Buffer *processed = NULL;
    switch (mode) {
    case COMPRESS:
        fprintf(stderr, "will compress\n");
        switch (algorithm) {
            case HUFFMAN:
                fprintf(stderr, "using huffman algorithm\n");
                processed = huffman_compress(data);
                break;
            case LZSS:
                fprintf(stderr, "using lzss algorithm\n");
                processed = lzss_compress(data);
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
                processed = huffman_extract(data);
                break;
            case LZSS:
                fprintf(stderr, "using lzss algorithm\n");
                processed = lzss_extract(data);
                break;
            default:
                break;
        }
        break;
    default:
        err_quit("no mode set, exiting");
        break;
    }
    fprintf(stderr, "writing to file %s\n", outfile);
    writeFile(processed, outfile);
    delete_buffer(data);
}

void usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "-a [algorithm]: compression algorithm; huff or lzss\n");
    fprintf(stderr, "-e: extract (default huffman)\n");
    fprintf(stderr, "-c: compress (default huffman)\n");
    fprintf(stderr, "-i [infile]: set input file, - for stdin (default)\n");
    fprintf(stderr, "-o [outfile]: set output file, - for stdout (default)\n");
    exit(EXIT_FAILURE);
}
