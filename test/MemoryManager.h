#pragma once

#include <stdlib.h> 
#include <vector>
#include "types.h"
#include "IMemoryManager.h"

class MemoryManager :
    public IMemoryManager
{
public:
    ~MemoryManager();

    bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte) override;
    void ADDIN_API FreeMemory(void** pMemory) override;

private:
    std::vector<void *> allocatedBlocks;

};

