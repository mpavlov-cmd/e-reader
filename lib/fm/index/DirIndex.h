#ifndef DIRINDEX_H
#define DIRINDEX_H

#include <Arduino.h>
#include <index/FileIndex.h>

struct DirIndex
{

private:
    uint8_t index;
    FileIndex **fileIndexArray;

public:

    /**
     * Adds object to array
     * 
     * @param fileIndex a pointer to the dynamically created object 
     * @returns boolean true if object succesfully added
     */
    boolean add(FileIndex* fileIndex);

    /**
     * Delete last object from the index
     * 
     * @returns true if deleted, false in case nothing to delete
     */
    boolean removeLast();

    /**
     * Get FileIndex by idx
     * 
     * @param idx index of the file to return
     * @returns copy of the stored FileIndex object to avoid a need for manual memory deallocation
     */
    FileIndex byIndex(uint8_t idx);

    /**
     * @returns size of the index 
     */
    uint8_t size();

    DirIndex() {
        fileIndexArray = new FileIndex*[UINT8_MAX];
        index = 0;
        for (int i = 0; i < UINT8_MAX; ++i) {
            fileIndexArray[i] = nullptr;
        }
    }

    ~DirIndex()
    {
        // Destructor to free allocated memory
        for (uint8_t i = 0; i < index; i++)
        {
            delete fileIndexArray[i];
        }
    }
};

#endif
