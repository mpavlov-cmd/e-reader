#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>

struct FileManager
{
private:
    const uint8_t csPin;

    fs::FS& fs;
    boolean cardAvailable = false;
    uint64_t cardSize = 0;

public:
    FileManager(fs::FS& fs, const uint8_t csPin);

    uint64_t begin();
    boolean isAvailable();

    /**
     * @param path to the file
     * @param mode FILE_READ, FILE_WRITE, FILE_APPEND
     * 
     * @returns file. File must to be CLOSED after usage by calling file.close()
     */
    File openFile(const char *path, const char* mode);

    void listDir(const char *dirname, uint8_t levels);
    void createDir(const char *path);
    void removeDir(const char *path);
    void readFile(const char *path);
    void writeFile(const char *path, const char *message);
    void appendFile(const char *path, const char *message);
    void renameFile(const char *path1, const char *path2);
    void deleteFile(const char *path);
};

#endif