#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>
#include <iomanip>

struct CacheBlock {
    unsigned long tag;
    bool valid;
    int RRPV;
};

class Cache {
private:
    int numSets;
    int associativity;
    int blockSize;
    int RRPV_max;
    int RRPV_long;
    std::vector<std::vector<CacheBlock>> sets;

public:
    Cache(int cacheSize, int blockSize, int associativity, int M) : blockSize(blockSize), associativity(associativity) {
        numSets = cacheSize / (blockSize * associativity);
        RRPV_max = (1 << M) - 1;  // RRPV = 2^M - 1
        RRPV_long = RRPV_max - 1; // RRPV = 2^M - 2 (Long re-reference interval)
        sets.resize(numSets, std::vector<CacheBlock>(associativity, {0, false, RRPV_max})); //Resizes the sets vector to have numSets sets, each containing an associativity number of CacheBlock objects initialized with default values (tag = 0, valid = false, RRPV = RRPV_max).
    }

    bool accessCache(unsigned long address, int &hitCount, int &missCount) {
        unsigned long index = (address / blockSize) % numSets;
        unsigned long tag = address / (blockSize * numSets);

        for (auto &block : sets[index]) {
            if (block.valid && block.tag == tag) { //Checks if the block is valid and the tags match.
                block.RRPV = 0;  // Set RRPV to 0 on hit (near-immediate re-reference)
                hitCount++;
                return true;
            }
        }

        // Miss Handling
        missCount++;
        int victimIndex = -1;
        for (int i = 0; i < associativity; i++) { //Searches for a block with the maximum RRPV (i.e., the least likely to be used soon).
            if (sets[index][i].RRPV == RRPV_max) { //If such a block is found, its index is saved in victimIndex.
                victimIndex = i;
                break;
            }
        }

        if (victimIndex == -1) {
            // No block with RRPV_max found, increment all RRPVs
            for (auto &block : sets[index]) block.RRPV++;
            // Retry access after incrementing RRPVs
            return accessCache(address, hitCount, missCount);
        } else {
            // Replace the victim block with the new block, setting its RRPV to RRPV_long.
            sets[index][victimIndex] = {tag, true, RRPV_long};
            return false;
        }
    }

    void printCacheInfo() const {
        std::cout << "Cache Size: " << numSets * blockSize * associativity << " Bytes" << std::endl;
        std::cout << "Block Size: " << blockSize << " Bytes" << std::endl;
        std::cout << "Number of Sets: " << numSets << std::endl;
        std::cout << "Associativity: " << associativity << "-way" << std::endl;

        int offsetBits = log2(blockSize);
        int setBits = log2(numSets);
        int tagBits = 32 - offsetBits - setBits;

        std::cout << "Tag Bits: " << tagBits << std::endl;
        std::cout << "Set Index Bits: " << setBits << std::endl;
        std::cout << "Block Offset Bits: " << offsetBits << std::endl;
    }
};

int main() {
    int address;
    int cacheSize = 32 * 1024 ;  // 32KB
    int blockSize = 64;  // 64B
    int associativity = 2;  // 2-way
    int M = 2;  // 2-bit RRIP
    Cache cache(cacheSize, blockSize, associativity, M);

    cache.printCacheInfo();

    std::ifstream infile("memory_addresses.txt");
    int hitCount = 0, missCount = 0;
    while (infile >> std::hex >> address) {
        cache.accessCache(address, hitCount, missCount);
    }

    int totalAccesses = hitCount + missCount;
    double hitRate = (double)hitCount / totalAccesses;
    double missRate = (double)missCount / totalAccesses;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Hit Rate: " << hitRate * 100 << "%" << std::endl;
    std::cout << "Miss Rate: " << missRate * 100 << "%" << std::endl;

    return 0;
}
