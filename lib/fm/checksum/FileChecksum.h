#ifndef FILECHECKSUM_C
#define FILECHECKSUM_C

#include <Arduino.h>

class FileChecksum
{
    public:
        /**
         * Allows to calculate checksum for a file by reading it in chunks
         * 
         * @param path Path to the file
         * @param bufferSize Size of the buffer to read file in chunks
         * 
         * @returns Checksum of the file as string
         */
        virtual String checksum(const char *path, uint16_t bufferSize) = 0;
};

#endif