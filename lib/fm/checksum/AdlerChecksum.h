#ifndef ADLERCHECKSUM_C
#define ADLERCHECKSUM_C

#include <Arduino.h>
#include <checksum/FileChecksum.h>
#include <state/StateAware.h>
#include <FileManager.h>
#include <Adler32.h>

class AdlerChecksum : public FileChecksum
{
private:
    FileManager &fileManager;

public:
    AdlerChecksum(FileManager &fileManager) : fileManager(fileManager) {}

    /**
     *
     * Allows to get file hash using Adler32 algorithm with a givn buffer size
     *
     * @param path absolute path to the file
     * @param bufferSize size of the buffer to read file contents
     * @param state optonal pointer to the state variable, if provided it will be updated with the current state hile reading the file
     * 
     * @returns checksum of the file as a hex string
     */
    String checksum(const char *path, uint16_t bufferSize, size_t* state = nullptr) override
    {
        // Drop state value
        if (state != nullptr) {
            *state = 0;
        }
      
        if (bufferSize == 0 || bufferSize > UINT16_MAX)
        {
            Serial.printf(
                "Invalid buffer size of %i. Please choose value between 1 and %i\n", bufferSize, UINT16_MAX);

            return emptyString;
        }

        File file = fileManager.openFile(path, FILE_READ);
        if (!file)
        {
            Serial.printf("Failed to open file: %s\n", path);
            return emptyString;
        }

        if (file.isDirectory())
        {
            Serial.printf("Path is a directory, unable to calculate checksum: %s\n", path);
            file.close();
            return emptyString;
        }

        size_t fileSize = file.size();
        size_t choosenBufferSzie = bufferSize < fileSize ? bufferSize : fileSize;
        Serial.printf("Checksum: choosen buffer size: %i\n", choosenBufferSzie);

        Adler32 adler32;
        char buffer[choosenBufferSzie];
        size_t index = 0;

        while (file.available())
        {
            buffer[index++] = file.read();
            if (index == choosenBufferSzie || !file.available())
            {
                // Serial.printf("Checksum: addFast will be executed buffer: %s\n", buffer);
                adler32.addFast(buffer, choosenBufferSzie);
                index = 0;

                memset(buffer, '\0', choosenBufferSzie);

                if (state != nullptr)
                {
                    // Take care about state
                    if (!file.available())
                    {
                        *state = file.size();
                    }
                    else
                    {
                        *state += choosenBufferSzie;
                    }
                }
            }
        }

        uint16_t adlerInt = adler32.getAdler();
        String hexString = String(adlerInt, HEX);

        file.close();

        return hexString;
    }
};

#endif