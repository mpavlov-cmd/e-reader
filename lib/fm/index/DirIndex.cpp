#include "DirIndex.h"

boolean DirIndex::add(FileIndex* fileIndex)
{
    if (index >= UINT8_MAX)
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

FileIndex DirIndex::byIndex(uint8_t idx)
{
    if (idx >= UINT8_MAX) {
        Serial.println("Error, no such index");
        FileIndex emptyFileIndex;
        return emptyFileIndex;
    }

    FileIndex* fi = fileIndexArray[idx];
    return *fi;
}

uint8_t DirIndex::size()
{
    return index;
}
