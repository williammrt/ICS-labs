#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_LEN 256

typedef struct {
    int valid;
    int LRU;
    long long tag;
} cacheLine;

long long getSet(long long address, int setNum_b, int bitNum) {
    return (address >> bitNum) & (0xffffffff >> (64-setNum_b));
}

long long getTag(long long address, int setNum_b, int bitNum) {
    return address >> (setNum_b + bitNum);
}

int DEBUG = 1;

int main(int argc, char **argv)
{
    if (strcmp(argv[1], "-h") == 0) {
        printf("%s\n", "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>");
        printf("%s\n", "Options:");
        printf("%s\n", "  -h         Print this help message.");
        printf("%s\n", "  -v         Optional verbose flag.");
        printf("%s\n", "  -s <num>   Number of set index bits.");
        printf("%s\n", "  -E <num>   Number of lines per set.");
        printf("%s\n", "  -b <num>   Number of block offset bits.");
        printf("%s\n", "  -t <file>  Trace file.");
        printf("%s\n", "");
        printf("%s\n", "Examples:");
        printf("%s\n", "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace");
        printf("%s\n", "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace");
        return 0;
    }

    int i = 1;
    int verbose = 0;

    if (strcmp(argv[1], "-v") == 0) {
        verbose = 1;
        i += 1;
    }

    int setNum_b = 0;
    int lineNum_b = 0;
    int bitNum = 0;

    int setNum = 0;
    int lineNum = 0;

    const char* tracefile_path = NULL;

    for (; i < argc; i+=2) {
        if (strcmp(argv[i], "-s") == 0) {
            setNum_b = strtol(argv[i+1], NULL, 10);
            setNum = 1 << setNum_b; 
        } else if (strcmp(argv[i], "-E") == 0) {
            lineNum_b = strtol(argv[i+1], NULL, 10);
            lineNum =  lineNum_b;
        } else if (strcmp(argv[i], "-b") == 0) {
            bitNum = strtol(argv[i+1], NULL, 10);
        } else if (strcmp(argv[i], "-t") == 0) {
            tracefile_path = argv[i+1];
        } else {
            printf("Error in argument! \n");
            return 1;
        }
    }
    if (DEBUG) { printf("setNum = %d, lineNum = %d\n", setNum, lineNum); }
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    FILE *tracefile;
    char buffer[BUFFER_LEN];
    char optype;
    char addrBuffer[16];
    long long address = 0;
    int datasize = 0; // aligned properly, do we really need this? Mysterious bug in reading "dave"
    // but we don't know bitNum, may be less than 8 bytes?

    if ((tracefile = fopen(tracefile_path, "r")) == NULL) {
        printf("Error in opening tracefile!\n");
        return 1;
    }
    cacheLine* cache = (cacheLine*) calloc(setNum*lineNum, sizeof(cacheLine));
    int emptyLinePos = -1;
    int LRU = 0;
    int LRU_pos = 0;
    long long tag = 0;
    int cacheIndex = 0;
    int hitFlag = 0;
    while (fgets(buffer, BUFFER_LEN, tracefile)) {
        
        sscanf(buffer, "%s %[^,] %d", &optype, addrBuffer, &datasize);
        if (optype == 'I') { // skip I
            continue;
        } 
        
        address = strtol(addrBuffer, NULL, 16);
        datasize = strtol(&buffer[strlen(buffer)-2], NULL, 10);
        if (verbose == 1) {
            printf("%c %llx,%d ", optype, address, datasize);
        }

        // assume no invalid optype
        int set_index = getSet(address, setNum_b, bitNum);
        tag = getTag(address, setNum_b, bitNum);
        if (DEBUG) {
            printf(" set_index = %x, tag = %llx ", set_index, tag);
        }

        for (int i = 0; i < lineNum; i += 1) {
            cacheIndex = set_index*lineNum + i;
            if ((emptyLinePos == -1) && cache[cacheIndex].valid == 0) {
                emptyLinePos = cacheIndex;
            }
            if (DEBUG) { printf("ccv = %d ccL = %d ", cache[cacheIndex].valid, cache[cacheIndex].LRU); }
            if ((cache[cacheIndex].valid == 1) && (cache[cacheIndex].LRU > LRU)) {
                LRU = cache[cacheIndex].LRU;
                LRU_pos = cacheIndex;
            }
            if (cache[cacheIndex].valid == 1 && cache[cacheIndex].tag == tag) { // hit
                if (verbose == 1) { printf("hit "); }
                if (DEBUG) { printf(" cacheIndex = %d ", cacheIndex); }
                hits += 1;
                cache[cacheIndex].LRU = 1;
                if (optype == 'M') {
                    if (verbose == 1) { printf("hit "); }
                    hits += 1;
                }
                if (verbose == 1) { printf("\n"); }

                emptyLinePos = -1;
                LRU = 0;
                LRU_pos = 0;
                hitFlag = 1; // go to next line
            }
        }
        if (hitFlag == 1) {
            hitFlag = 0;
            continue;
        }
        // now no hit, so miss
        if (verbose == 1) { printf("miss "); }
        misses += 1;
        if (emptyLinePos != -1) {
            // if (DEBUG) { printf("emptyLinePos = %d\n", emptyLinePos); }
            cache[emptyLinePos].tag = tag;
            cache[emptyLinePos].valid = 1;
            cache[emptyLinePos].LRU = 1;
            if (optype == 'M') {
                if (verbose == 1) { printf("hit "); }
                hits += 1;
            }
            if (DEBUG) { printf(" emptyLinePos = %d ",  emptyLinePos); }
        } else {
            for (int i = 0; i < lineNum; i += 1) {
                cacheIndex = set_index*lineNum + i;
                if (cache[cacheIndex].valid == 1) {
                    cache[cacheIndex].LRU += 1;
                }
            }
            if (verbose == 1) { printf("eviction "); }
            evictions += 1;
            if (DEBUG) { printf(" LRU_pos = %d ",  LRU_pos); }
            cache[LRU_pos].tag = tag;
            cache[LRU_pos].LRU = 1;
            if (optype == 'M') {
                if (verbose == 1) { printf("hit "); }
                hits += 1;
            }
        }
        // no empty line.

        if (verbose == 1) { printf("\n"); }
        // re-init index and values
        emptyLinePos = -1;
        LRU = 0;
        LRU_pos = 0;
        /*
        check hit
            if hit
                load hit and/or store hit
            else:
                print miss
                check with space
                    if no:
                        eviction

                if with write, then hit
        */
    }

    fclose(tracefile);
    printSummary(hits, misses, evictions);
    return 0;
}
