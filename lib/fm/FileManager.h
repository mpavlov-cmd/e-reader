#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <set/Set.h>
#include <index/FileIndex.h>

struct DirIndexConf {

    static const DirIndexConf FULL;

    const boolean showDir;
    const boolean showHidden;
    const boolean filterByExt;
    const char* extNeedle;
};

struct FileManager
{
private:
    const uint8_t csPin;

    fs::FS &fs;
    boolean cardAvailable = false;
    uint64_t cardSize = 0;

public:

    FileManager(fs::FS &fs, const uint8_t csPin);

    /**
     * Initiates SD communication over over SPI and verifies if card is present;
     *
     * @returns size of the card in bytes
     */
    uint64_t begin();

    /**
     * Checks if file or directory exists
     * 
     * @param path to file or directory
     *
     * @returns true if exists, false if not
     */
    bool exists(const char* path);

    /**
     * @param path to the file
     * @param mode FILE_READ, FILE_WRITE, FILE_APPEND
     *
     * @returns file. File must to be CLOSED after usage by calling file.close()
     */
    File openFile(const char *path, const char *mode);

    /**
     * @param path to the directory where files should be counted
     * @param conf indexer configuration
     * @param result directory index result passed as a param for perfroamnce optimiztion
     * The implementation is not responsible for clearing the result before suage
     * 
     * @returns true in case of succesfull index, false otherwise
     */
    bool indexDirectory(const char *path, const DirIndexConf conf, Set<FileIndex>& result);

    /**
     * Fild file extention
     *
     * @param filename pointer to the string where extention needs to be found
     *
     * @returns pointer to the same string but 1 char after last dot ext or nullptr
     */
    const char *findFileExtension(const char *filename);

    /**
     * Allows to find directory name out of File::path()
     * Do not forget to free meemory when not needed uing free((void*)retval);
     *
     * @param path file path
     *
     * @returns path to the directory without ending slash or '/'
     */
    const char* getParentDir(const char *path);

    /**
     * Writes contents to the newrly created file 
     * 
     * @param path path to the new file
     * @param message file contents 
     * 
     * @returns true if write succesful
     */
    bool writeFile(const char *path, const char *message);

    /**
     * Creates directory under specified path
     * @param path
     */
    void createDir(const char *path);

    /**
     * Removes empty directory under specified path;
     * @param path
    */
    void removeDir(const char *path);

     /**
     * Removes directory and all its contents under specified path, 
     * @param path
    */
    void removeDirRecursive(const char *path);

    /**
     * Utility method that would accept directry patch and return one level up directory
     * Works without dynamic memory allocation
     * Example: 
     * 
     * "/books" → "/"
     * "/books/current" → "/books"
     * "/books/current/" → "/books"
     * "/" → "/"
     * 
     * @param directory path
     * @return prent level directory or "/" in case FS root
     * 
     */
    const char* getPreviousLevel(const char* path);

    /**
     * Reads file to buffer of a given size. 
     * 
     * @param path absolute path to the file
     * @param buffer pointer to the char array to store file contents
     * @param bufferSize size of the buffer, as buffer is a pointer
     * 
     * @returns true in case operation was succesful
     */
    bool readFileToBuffer(const char *path, char *buffer, size_t bufferSize);


    // TODO: Here as an example. Review what's needed
    void listDir(const char *dirname, uint8_t levels);
    void readFile(const char *path);
    void appendFile(const char *path, const char *message);
    void renameFile(const char *path1, const char *path2);
    void deleteFile(const char *path);

private:
    /**
     * Closes file file and assigns root.openNextFile
     *
     * @param root indexed directory
     * @param file file to close and assign next file from the directory
     */
    void closeAndOpenNext(File &root, File &file);
};

#endif