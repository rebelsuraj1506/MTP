#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <list>
#include <string>

class Cache
{
private:
    std::vector<std::list<int>> cache;
    int capacity;
    int numSets;
    int associativity;
    int hits, misses;
    int blockSize;

public:
    Cache(int numSets, int associativity, int blockSize) 
        : capacity(numSets * associativity), hits(0), misses(0), numSets(numSets), associativity(associativity), blockSize(blockSize)
    {
        cache.resize(numSets);
    }

    std::list<int>::iterator searchInList(std::list<int> &set, int tag)
    {
        for (auto it = set.begin(); it != set.end(); it++)
        {
            if (*it == tag)
            {
                return it;
            }
        }
        return set.end();
    }

    void accessAddress(int address)
    {
        int setIndex = (address / blockSize) % numSets;
        int tag = address / (blockSize * numSets);

        std::list<int> &set = cache[setIndex];
        std::list<int>::iterator it = searchInList(set, tag);
        if (it != set.end())
        {
            set.erase(it);
            set.push_back(tag);
            // Hit
            hits++;
        }
        else
        {
            // Miss
            misses++;
            if (set.size() == associativity)
            {
                set.pop_front();
            }
            set.push_back(tag);
        }
    }

    double getHitRate() const
    {
        return static_cast<double>(hits) / (hits + misses);
    }

    double getMissRate() const
    {
        return static_cast<double>(misses) / (hits + misses);
    }

    int getCapacity() const
    {
        return capacity;
    }

    int getTagBits() const
    {
        int addressBits = 32; // Assuming a 32-bit address
        return addressBits - getSetBits() - getOffsetBits();
    }

    int getSetBits() const
    {
        return std::log2(numSets);
    }

    int getOffsetBits() const
    {
        return std::log2(blockSize);
    }

    int getCacheSize() const
    {
        return capacity * blockSize;
    }

    int getBlockSize() const
    {
        return blockSize;
    }
};

int main()
{
    std::ifstream inputFile("memory_addresses.txt");
    int address;
    int numSets = 16;
    int associativity = 2;
    int blockSize = 64; // 64 bytes per block

    Cache cache(numSets, associativity, blockSize);

    while (inputFile >> std::hex >> address)
    {
        cache.accessAddress(address);
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Cache Capacity in number of Blocks: " << cache.getCapacity() << std::endl;
    std::cout << "Cache Size in bytes: " << cache.getCacheSize() << std::endl;
    std::cout << "Block Size in bytes: " << cache.getBlockSize() << std::endl;
    std::cout << "Tag Bits: " << cache.getTagBits() << std::endl;
    std::cout << "Set Bits: " << cache.getSetBits() << std::endl;
    std::cout << "Offset Bits: " << cache.getOffsetBits() << std::endl;
    std::cout << "Hit Rate: " << cache.getHitRate() * 100 << "%" << std::endl;
    std::cout << "Miss Rate: " << cache.getMissRate() * 100 << "%" << std::endl;

    return 0;
}
