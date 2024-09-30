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
    boolean add(FileIndex* fileIndex);
    boolean removeLast();
    FileIndex& byIndex(uint8_t idx);
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
