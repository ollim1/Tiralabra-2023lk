#include "../include/lzss_common.h"

/**
 * Find string needle from ringbuffer haystack in reverse
 * @param haystack the RingBuffer to check
 * @param needle string to find in the the RingBuffer
 * @return index starting from the end of the array, or -1 if no match
 */
int findMatch(RingBuffer *haystack, Buffer *needle)
{

    if (!haystack || !needle)
        err_quit("null pointer in findMatch");

    if (needle->len < 1 || haystack->len < 1)
        return -1;
    
    for (int length = 0, distance = 0; distance < haystack->len && distance < WINDOW_SIZE; distance++) {
        unsigned char c = ringbuffer_getRev(haystack, distance);
        if (c == needle->data[needle->len - length - 1]) {
            length++;
        } else {
            distance -= length - 1;
            length = 0;
        }
        if (length >= needle->len)
            return distance + 1;
    }
    return -1;
}

/**
 * Find string needle from ringbuffer haystack in order. Optimized using KMP
 * (Knuth-Morris-Pratt) algorithm.
 * @param haystack the RingBuffer to check
 * @param needle string to find in the the RingBuffer
 * @return index starting from the end of the array, or -1 if no match
 */
int findMatchKMP(RingBuffer *haystack, Buffer *needle)
{
    if (!haystack || !needle)
        err_quit("null pointer in findMatch");

    if (needle->len < 1 || haystack->len < 1)
        return -1;

    int ringBufferPosition = 0;
    int matchPosition = 0;

    int KMPTable[TOKEN_MAXLEN] = {0};
    genKMPTable(needle, KMPTable);
    
    while (ringBufferPosition < haystack->len && ringBufferPosition < WINDOW_SIZE) {
        unsigned char c = ringbuffer_get(haystack, ringBufferPosition);
        if (c == needle->data[matchPosition]) {
            ringBufferPosition++;
            matchPosition++;
            if (matchPosition == needle->len)
                return haystack->len - (ringBufferPosition - needle->len);
        } else {
            matchPosition = KMPTable[matchPosition];
            if (matchPosition < 0) {
                // match was reset, 
                ringBufferPosition++;
                matchPosition++;
            }
        }
    }
    return -1;
}

/**
 * Generate Knuth-Morris-Pratt table for a string.
 * Assuming result table is the same size as the string.
 * @param needle string to analyze
 * @param table pointer to table to use
 */
void genKMPTable(Buffer *needle, int *table)
{
    // set initial value
    table[0] = -1;
    // walk through string
    for (int pos = 1, cnd = 0; pos < needle->len; pos++, cnd++) {
        if (needle->data[pos] == needle->data[cnd]) {
            table[pos] = table[cnd];
        } else {
            table[pos] = cnd;
            while (cnd >= 0 && needle->data[pos] != needle->data[cnd])
                cnd = table[cnd];
        }
    }
}

