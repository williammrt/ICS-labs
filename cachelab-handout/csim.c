/*
1. get the 3 parameter
2. set up the cache
3. read suitable address
4. check whether in cache
5. if in, hit, and check for M
6. if not, find an empty line first, fill in and check for m
7. if no emptyline, evict one line, fill in and check for m

8. exit procedure: all lru += 1, hit line = 1

*/
#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int valid;
    int LRU;
    long long tag;
} cacheLine;

long long getSet(long long address, int numOfSetBits, int numOfBlockBit) {
    return (address >> numOfBlockBit) & (0xffffffff >> (64-numOfSetBits));
}

long long getTag(long long address, int numOfSetBits, int numOfBlockBit) {
    return address >> (numOfSetBits + numOfBlockBit);
}

#define BUFFER_LEN 256
int DEBUG = 0;

int main(int argc, char **argv)
{
	int i = 1;
    int verbose = 0;
    if (strcmp(argv[1], "-v") == 0) {
        verbose = 1;
        i += 1;
    }

    int numOfSetBits = 0;
    int numOfSet = 0;
    int numOfLine = 0;
    int numOfBlockBit = 0;
    const char* tracefile_path = NULL;

    for (; i < argc; i+=2) {
        if (strcmp(argv[i], "-s") == 0) {
            numOfSetBits = strtol(argv[i+1], NULL, 10);
            numOfSet = 1 << numOfSetBits; 
        } else if (strcmp(argv[i], "-E") == 0) {
            numOfLine = strtol(argv[i+1], NULL, 10);
        } else if (strcmp(argv[i], "-b") == 0) {
            numOfBlockBit = strtol(argv[i+1], NULL, 10);
        } else if (strcmp(argv[i], "-t") == 0) {
            tracefile_path = argv[i+1];
        } else {
            printf("Error in argument! \n");
            return 1;
        }
    }
    if (DEBUG) { printf("numOfSetBits= %d, numOfSet= %d, numOfLine= %d, numOfBlockBit = %d",numOfSetBits,numOfSet,numOfLine,numOfBlockBit ); }
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    FILE *tracefile;
    char buffer[BUFFER_LEN];
    char optype;
    char addrBuffer[16];
    long long address = 0;
    int datasize = 0; // aligned properly, do we really need this? Mysterious bug in reading "dave"
    // but we don't know numOfBlockBit, may be less than 8 bytes?

    if ((tracefile = fopen(tracefile_path, "r")) == NULL) {
        printf("Error in opening tracefile!\n");
        return 1;
    }
    cacheLine* cache = (cacheLine*) calloc(numOfSet*numOfLine, sizeof(cacheLine));

    while (fgets(buffer, BUFFER_LEN, tracefile)) {
    	int cache_hit = 0;
    	sscanf(buffer, "%s %[^,] %d", &optype, addrBuffer, &datasize);
        if (optype == 'I') { // skip I
            continue;
        } 
        
        address = strtol(addrBuffer, NULL, 16);
        datasize = strtol(&buffer[strlen(buffer)-2], NULL, 10);
        if (verbose == 1) {
            printf("%c %llx,%d ", optype, address, datasize);
        }

        int set_index = getSet(address, numOfSetBits, numOfBlockBit);
        long long tag = getTag(address, numOfSetBits, numOfBlockBit);
        if (DEBUG) {
            printf(" set_index = %x, tag = %llx ", set_index, tag);
        }

        // check whether hit or not first
        for (int i = 0; i < numOfLine; i += 1) {
        	int cacheIndex = set_index*numOfLine + i;
        	if ((cache[cacheIndex].valid == 1)&& (cache[cacheIndex].tag == tag)) {
        		if (verbose == 1) { printf("hit "); }
        		hits += 1;
        		cache[cacheIndex].LRU = 0; // we add 1 at last
        		if (optype == 'M') {
                    if (verbose == 1) { printf("hit "); }
                    hits += 1;
                }
                if (verbose == 1) { printf("\n"); }
                cache_hit = 1;
        	}
        }
        if (cache_hit == 1) { 
            for (int i = 0; i < numOfLine; i += 1) { 
                int cacheIndex = set_index*numOfLine + i;
                if (cache[cacheIndex].valid == 1) {
                    cache[cacheIndex].LRU += 1;
                }
            }
            continue; 
        }

        // now not hit, we find an empty line first
        for (int i = 0; i < numOfLine; i += 1) { 
        	int cacheIndex = set_index*numOfLine + i;
        	if (cache[cacheIndex].valid == 0) {
        		cache[cacheIndex].valid = 1;
        		cache[cacheIndex].LRU = 0;
        		cache[cacheIndex].tag = tag;
        		cache_hit = 1;
        		break;
        	}
        }
        if (verbose == 1) { printf("miss "); } 
        misses += 1;
       	
        
        
        if (cache_hit == 1) { 
        	 if (optype == 'M') {
                if (verbose == 1) { printf("hit\n");for (int i = 0; i < numOfLine; i += 1) { 
        	int cacheIndex = set_index*numOfLine + i;
        	if (cache[cacheIndex].valid == 1) {
        		cache[cacheIndex].LRU += 1;
        	}
        	
        } }
                hits += 1;
            }
        	continue; }

        int evictIndex = 0;
        int LRU = -1;
        for (int i = 0; i < numOfLine; i += 1) { 
        	int cacheIndex = set_index*numOfLine + i;
        	if (cache[cacheIndex].valid == 1 && cache[cacheIndex].LRU > LRU) {
        		LRU =  cache[cacheIndex].LRU;
        		evictIndex = cacheIndex;
        	}
        }
        cache[evictIndex].LRU = 0;
        cache[evictIndex].tag = tag;
        if (verbose == 1) { printf("eviction "); }
        evictions += 1;
        if (optype == 'M') {
                if (verbose == 1) { printf("hit "); }
                hits += 1;
            }
        if (verbose == 1) { printf("\n"); }

        for (int i = 0; i < numOfLine; i += 1) { 
        	int cacheIndex = set_index*numOfLine + i;
        	if (cache[cacheIndex].valid == 1) {
        		cache[cacheIndex].LRU += 1;
        	}
        }


    }


    printSummary(hits, misses, evictions);
    return 0;
}
