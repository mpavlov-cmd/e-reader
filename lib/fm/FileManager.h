#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <index/DirIndex.h>

struct DirIndexConf {
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
    const DirIndexConf DIR_IDX_FULL = {false, false, false, nullptr};

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

    // TODO: Pass struct as a parameter which would include showDir, showHidden, and ext. filter
    /**
     * @param path to the directory where files should be counted
     * @param showDir indicates if DirIndex should contain only files or directories as well
     * @param showHidden indicates wether hidden files should be shown
     *
     * @returns DirIndex object. The object will be copied to the calling code. This is fine
     */
    DirIndex indexDirectory(const char *path, const DirIndexConf& conf);

    /**
     * Fild file extention
     *
     * @param filename pointer to the string where extention needs to be found
     *
     * @returns pointer to the same string but 1 char after last dot ext or nullptr
     */
    char *findFileExtension(char *filename);

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

    // TODO: Here as an example. Review what's needed
    void listDir(const char *dirname, uint8_t levels);
    void createDir(const char *path);
    void removeDir(const char *path);
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