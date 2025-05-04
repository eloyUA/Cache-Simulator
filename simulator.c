/**
 * PROGRAM: Memory Cache Simulator
 * FILE: simulator.c
 * AUTHOR: Eloy Urriens Arpal
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "modules/trace.h"

#define CONFIG_FILE_PATH "./config"
#define TRACE_FILE_PATH "./trace"

typedef struct line {
	unsigned long long start_addr;
	unsigned long long end_addr;
} line;

typedef struct {
	line *cache;
	unsigned long long cache_size;
	unsigned long long line_size;
	unsigned long long n_ways;
	unsigned long long n_sets;
	unsigned long long n_lines;

	line *victimCache;
	unsigned long long n_lines_victim;
} CacheMemory;

/**
 * FUNCTION: rangeBits
 * DESCRIPTION:
 * 		Example:
 * 			n = 128 (00000000 00000000 00000000 00000000 00000000 00000000 00000000 01000000)
 * 			small_bit = 5
 * 			big_bit = 9
 * 			
 * 			Output: 2 (00 010), range([Bit 5, Bit 9])
 */
unsigned long long rangeBits(unsigned long long n, int small_bit, int big_bit) {
	unsigned long long bit1 = 1;
	unsigned long long bit2 = 1;
	unsigned long long n1;

	if (small_bit < 0 || small_bit > 63) {
		perror("rangeBits");
		exit(EXIT_FAILURE);
	}
	else if (big_bit < 0 || big_bit > 63) {
		perror("rangeBits");
		exit(EXIT_FAILURE);
	}
	else if(small_bit > big_bit) {
		perror("rangeBits");
		exit(EXIT_FAILURE);
	}
	else if (big_bit - small_bit + 1 < 64) {
		bit1 = bit1 << small_bit;
		bit2 = bit2 << ((big_bit - small_bit) + 1);

		n1 = n / bit1;
		n = n1 % bit2;
	}
	return n;
}

/**
 * FUNCTION: missVictimCache
 * INPUT: 
 * 		addr: A memory address
 * 		start_l: The position of the first line in the set (in cache)
 * 		cacheMem: The memory cache
 * REQUIREMENTS: The addr hasn't to be in the victim cache and cache
 * OUTPUT: The addr is put in cache and the last line in his set is put in victim cache
 */
void missVictimCache(unsigned long long addr, unsigned long long start_l, CacheMemory *cacheMem) {
	line new_line, evicted_line;
	new_line.start_addr = (addr / cacheMem->line_size) * cacheMem->line_size;
	new_line.end_addr = new_line.start_addr + cacheMem->line_size - 1;
	evicted_line = cacheMem->cache[start_l + cacheMem->n_ways - 1];

	for (unsigned long long i = start_l + cacheMem->n_ways - 1; i > start_l; i--) {
		cacheMem->cache[i] = cacheMem->cache[i - 1];
	}
	cacheMem->cache[start_l] = new_line;

	for (unsigned long long i = cacheMem->n_lines_victim - 1; i > 0; i--) {
		cacheMem->victimCache[i] = cacheMem->victimCache[i - 1];
	}
	cacheMem->victimCache[0] = evicted_line;
}

/**
 * FUNCTION: hitVictimCache
 * INPUT: 
 * 		hitVC_l: It's the position of the searched line (in victim cache)
 * 		start_l: The position of the first line in the set (in cache)
 * 		cacheMem: Cache memory
 * REQUIREMENTS:
 * 		Obviously 0 <= hitVC_l < Number of lines in the victim cache
 * OUTPUT: The line in victim cache is moved to cache (In the first position of the set) and
 * 		the last line in the set is moved to victim cache
 */
void hitVictimCache(unsigned char hitVC_l, unsigned long long start_l, CacheMemory *cacheMem) {
	line victim_line = cacheMem->victimCache[hitVC_l];
	line evicted_line = cacheMem->cache[start_l + cacheMem->n_ways - 1];
	for (unsigned long long i = start_l + cacheMem->n_ways - 1; i > start_l; i--) {
		cacheMem->cache[i] = cacheMem->cache[i - 1];
	}
	cacheMem->cache[start_l] = victim_line;

	for (unsigned long long i = hitVC_l; i > 0; i--) {
		cacheMem->victimCache[i] = cacheMem->victimCache[i - 1];
	}
	cacheMem->victimCache[0] = evicted_line;
}

/**
 * FUNCTION: missCache
 * INPUT: 
 * 		addr: A memory address
 * 		start_l: The position of the first line in the set that should contains addr
 * 		cacheMem: The memory cache
 * REQUIREMENTS: The addr isn't in the set
 * OUTPUT: True <=> Miss and False <=> Hit (Because the line could be in the victim cache)
 */
bool missCache(unsigned long long addr, unsigned long long start_l, CacheMemory *cacheMem) {
	// If there isn't victim cache, the addr is loaded in a new line
	if (cacheMem->n_lines_victim == 0) {
		line new_l;
		new_l.start_addr = (addr / cacheMem->line_size) * cacheMem->line_size;
		new_l.end_addr = new_l.start_addr + cacheMem->line_size - 1;

		for (unsigned long long i = start_l + cacheMem->n_ways - 1; i > start_l; i--) {
			cacheMem->cache[i] = cacheMem->cache[i - 1];
		}
		cacheMem->cache[start_l] = new_l;
		return true;
	}
	
	// There is Victim Cache, it used
	unsigned long long hitVC_l = 0;
	for (hitVC_l = 0; hitVC_l < cacheMem->n_lines_victim - 1; hitVC_l++) {
		if (addr >= cacheMem->victimCache[hitVC_l].start_addr && addr <= cacheMem->victimCache[hitVC_l].end_addr) {
			break;
		}
	}

	if (addr >= cacheMem->victimCache[hitVC_l].start_addr && addr <= cacheMem->victimCache[hitVC_l].end_addr) {
		hitVictimCache(hitVC_l, start_l, cacheMem);
		return false;
	}

	missVictimCache(addr, start_l, cacheMem);
	return true;
}

/**
 * FUNCTION: hitCache
 * INPUT:
 * 		hit_l: It's the position of the searched line (in cache)
 * 		start_l: It's the position of the first line in the set (in cache)
 * 		cacheMem: The cache memory
 * OUTPUT:
 * 		All lines in the positions [start_l, hit_l]
 * 		are moved to [start_l + 1, hit_l + 1] and the hit_l is moved to
 * 		start_l position (LRU)
 */
void hitCache(unsigned long long hit_l, unsigned long long start_l, CacheMemory *cacheMem) {
	line linea_hit = cacheMem->cache[hit_l];
	for (unsigned long long i = hit_l; i > start_l; i--) {
		cacheMem->cache[i] = cacheMem->cache[i - 1];
	}
	cacheMem->cache[start_l] = linea_hit;
}

/**
 * FUNCTION: accessMemoryCache
 * INPUT: A memory address and the cache memory
 * OUTPUT: True <=> Miss (The address wasn't in memory cache)
 */
bool accessMemoryCache(unsigned long long addr, CacheMemory *cacheMem) {	
	unsigned long long n_set;
	unsigned long long start_l;
	unsigned long long hit_l;

	int n_bits_byte = (int) log2((double) cacheMem->line_size);
	int n_bits_sets = (int) log2((double) cacheMem->n_sets);

	n_set = rangeBits(addr, n_bits_byte, n_bits_sets + n_bits_byte) % cacheMem->n_sets;
	start_l = n_set * cacheMem->n_ways;
	for (hit_l = start_l; hit_l < (n_set + 1) * cacheMem->n_ways - 1; hit_l++) {
		if (addr >= cacheMem->cache[hit_l].start_addr && addr <= cacheMem->cache[hit_l].end_addr) {
			break;
		}
	}
	
	if (addr >= cacheMem->cache[hit_l].start_addr && addr <= cacheMem->cache[hit_l].end_addr) {
		hitCache(hit_l, start_l, cacheMem);
		return false;
	}
	return missCache(addr, start_l, cacheMem);
}

/**
 * FUNCTION: runSimulation
 * DESCRIPTION: Executes the simulation
 */
void runSimulation(CacheMemory *cacheMem, const char *name_trace) {
	// The cache and victim cache are cleared
	for (unsigned long long i = 0; i < cacheMem->n_lines; i++) {
		cacheMem->cache[i].start_addr = 0;
		cacheMem->cache[i].end_addr = 0;
	}

	for (unsigned long long i = 0; i < cacheMem->n_lines_victim; i++) {
		cacheMem->victimCache[i].start_addr = 0;
		cacheMem->victimCache[i].end_addr = 0;
	}
	
	// Start simulation. The number of miss are counted
	Trace trace;
	if (initTrace(&trace, name_trace, "r") == -1) {
		perror("trace");
		exit(EXIT_FAILURE);
	}
	
	int res;
	unsigned long long addr;
	unsigned long long n_access = 0;
	unsigned long long n_miss = 0;
	
	res = readMemoryAddressTrace(&addr, trace);
	while (res != -1) {
		if (accessMemoryCache(addr, cacheMem)) {
			n_miss++;
		}
		n_access++;
		res = readMemoryAddressTrace(&addr, trace);
	}
	closeTrace(trace);

	printf("\n");
	printf("Results of the simulation:\n");
	printf("   Number of miss: %llu\n", n_miss);
	printf("   Number of access: %llu\n", n_access);
	printf("   Miss rate: %.2f%%\n", ((double) n_miss / n_access) * 100.0);
	printf("   Hit rate: %.2f%%\n", ((double) (n_access - n_miss) / n_access) * 100.0);
}

/**
 * FUNCTION: readConfigFile
 * OUTPUT: The cache memory loaded with config file
 */
void readConfigFile(CacheMemory *cacheMem) {
	FILE *f;
	if ((f = fopen(CONFIG_FILE_PATH,"r")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	char data[32];
	if (fscanf(f, "%s %llu\n", data, &cacheMem->cache_size) == EOF) { perror("fscanf"); exit(EXIT_FAILURE); }
	if (fscanf(f, "%s %llu\n", data, &cacheMem->line_size) == EOF) { perror("fscanf"); exit(EXIT_FAILURE); }
	if (fscanf(f, "%s %llu\n", data, &cacheMem->n_ways) == EOF) { perror("fscanf"); exit(EXIT_FAILURE); }
	if (fscanf(f, "%s %llu\n", data, &cacheMem->n_lines_victim) == EOF) { perror("fscanf"); exit(EXIT_FAILURE); }
	if (fclose(f) == EOF) {
		perror("fclose");
		exit(EXIT_FAILURE); 
	}

	if (cacheMem->cache_size % (cacheMem->line_size * cacheMem->n_ways) != 0) {
		printf("Error: The cache size must be a multiple of the line size");
		exit(EXIT_FAILURE);
	}
	
	cacheMem->n_sets = cacheMem->cache_size / (cacheMem->n_ways * cacheMem->line_size);
	cacheMem->n_lines = cacheMem->cache_size / cacheMem->line_size;
	if ((cacheMem->cache = (line*) malloc(sizeof(line) * cacheMem->n_lines)) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	if ((cacheMem->victimCache = (line*) malloc(sizeof(line) * cacheMem->n_lines_victim)) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
}

// ./simulator <trace_file_name>
int main (int argc, char *argv[]) {
	if (argc != 2) {
		perror("Invalid arguments");
		exit(EXIT_FAILURE);
	}
	const char *name_trace = argv[1];

	printf("-------------------------------------\n");
	printf("   Memory Cache Simulator\n");
	printf("-------------------------------------\n");

	CacheMemory cacheMem;
	readConfigFile(&cacheMem);

	printf("\n");
	printf("Description of memory:\n");
	printf("   Cache (set-associative):\n");
	printf("      Size: %llu bytes\n", cacheMem.cache_size);
	printf("      Size line: %llu bytes\n", cacheMem.line_size);
	printf("      Number of ways: %llu\n", cacheMem.n_ways);
	printf("      Number of lines: %llu\n", cacheMem.n_lines);
	printf("      Number of sets: %llu\n", cacheMem.n_sets);
	printf("      Replacement LRU.\n");

	printf("\n");
	printf("   Victim Cache (Associative):\n");
	if (cacheMem.n_ways == 0) {
		printf("      There isn't.\n");
	}
	else {
		printf("      Number of lines: %llu\n", cacheMem.n_lines_victim);
		printf("      Replacement LRU.\n");
	}
	
	runSimulation(&cacheMem, name_trace);
	return 0;
}