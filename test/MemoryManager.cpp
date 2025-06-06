#include "MemoryManager.h"

MemoryManager::~MemoryManager() 
{
    while (allocatedBlocks.size())
        FreeMemory(&allocatedBlocks.back());
}

bool ADDIN_API MemoryManager::AllocMemory(void** pMemory, unsigned long ulCountByte) 
{
    *pMemory = malloc(ulCountByte);

    if (pMemory  && *pMemory)
        allocatedBlocks.push_back(*pMemory);

    return pMemory != nullptr;
}

void ADDIN_API MemoryManager::FreeMemory(void** pMemory) 
{
    if (!pMemory || !*pMemory)
        return;

    auto iterator = std::find(allocatedBlocks.begin(), allocatedBlocks.end(), *pMemory);
    if (iterator == allocatedBlocks.end())
        throw std::out_of_range("Value not found in allocated blocks of memory");

    allocatedBlocks.erase(iterator);
    
    free(*pMemory);
    *pMemory = nullptr;
}
