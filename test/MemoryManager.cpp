#include "MemoryManager.h"

MemoryManager::~MemoryManager() 
{
    for (void* block : allocatedBlocks)
        FreeMemory(&block);
}

bool ADDIN_API MemoryManager::AllocMemory(void** pMemory, unsigned long ulCountByte) 
{
    *pMemory = malloc(ulCountByte);

    if (pMemory  && *pMemory)
        allocatedBlocks.push_back(*pMemory);

    return *pMemory != nullptr;
}

void ADDIN_API MemoryManager::FreeMemory(void** pMemory) 
{
    if (!pMemory || !*pMemory)
        return;

    auto iterator = std::find(allocatedBlocks.begin(), allocatedBlocks.end(), *pMemory);
    allocatedBlocks.erase(iterator);
    
    free(*pMemory);
    *pMemory = nullptr;
}
