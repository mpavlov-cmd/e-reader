#ifndef DIRINDEX_H
#define DIRINDEX_H

#include <Arduino.h>
#include <index/FileIndex.h>

struct DirIndex
{

private:
    const uint16_t CAPACITY = 2048;

    uint16_t index;
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
    FileIndex byIndex(uint16_t idx);

    /**
     * @returns size of the index 
     */
    uint16_t size();

    DirIndex() {
        Serial.println("--- Start index space allocation ---");
        fileIndexArray = new FileIndex*[CAPACITY];
        index = 0;
        for (int i = 0; i < CAPACITY; ++i) {
            fileIndexArray[i] = nullptr;
        }
        Serial.println("--- Finished index space allocation ---");
    }

    ~DirIndex()
    {
        // Destructor to free allocated memory
        for (uint16_t i = 0; i < index; i++)
        {
            delete fileIndexArray[i];
        }
    }
};

#endif
