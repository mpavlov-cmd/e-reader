#include "DirIndex.h"

boolean DirIndex::add(FileIndex* fileIndex)
{
    if (index >= CAPACITY)
    {
        return false;
    }

    fileIndexArray[index++] = fileIndex;

    return true;
}

boolean DirIndex::removeLast()
{
    delete fileIndexArray[index];
    if (index == 0)
    {
        return false;
    }

    index--;
    return true;
}

FileIndex DirIndex::byIndex(uint16_t idx)
{
    if (idx >= CAPACITY) {
        Serial.println("Error, no such index");
        FileIndex emptyFileIndex;
        return emptyFileIndex;
    }

    FileIndex* fi = fileIndexArray[idx];
    return *fi;
}

uint16_t DirIndex::size()
{
    return index;
}
