#include "FileManager.h"

FileManager::FileManager(fs::FS &fs, const uint8_t csPin) : fs(fs), csPin(csPin)
{
}

uint64_t FileManager::begin()
{
    cardAvailable = SD.begin(csPin);
    if (!cardAvailable)
    {
        Serial.println("Card Mount Failed");

        this->cardSize = 0;
        return cardSize;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");

        this->cardSize = 0;
        return cardSize;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    return cardSize;
}

File FileManager::openFile(const char *path, const char *mode)
{
    return fs.open(path, mode);
}

DirIndex FileManager::indexDirectory(const char *path, const DirIndexConf& conf)
{
    DirIndex dirIndex;

    File root = fs.open(path);

    // TODO: Duplicate code
    if (!root)
    {
        Serial.println("Failed to open directory");
        return dirIndex;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return dirIndex;
    }

    // Init Dir Index
    File file = root.openNextFile();
    while (file)
    {
        if (!conf.showDir && file.isDirectory())
        {
            closeAndOpenNext(root, file);
            continue;
        }

        if (!conf.showHidden && file.name()[0] == '.')
        {
            closeAndOpenNext(root, file);
            continue;
        }

        // Finding extention
        // Determine the length needed for the destination (including null terminator)
        // Allocate enough space for the destination array
        const char* filename = file.name();
        size_t length = strlen(filename) + 1; 
        char nonConstFilename[length]; 
        strcpy(nonConstFilename, filename);  

        char* nullableFileExt = findFileExtension(nonConstFilename);
        const char* fileExt = nullableFileExt == nullptr ? "" : nullableFileExt; 

        // Filter by ext. See correct way to compare 2 strings (const char *)
        if (conf.filterByExt && strcmp(conf.extNeedle, fileExt) != 0)
        {
            closeAndOpenNext(root, file);
            continue;
        }

        FileIndex *fileIndex = new FileIndex(file.name(), file.path(), fileExt, file.size(), file.isDirectory());
        dirIndex.add(fileIndex);

        closeAndOpenNext(root, file);
    }

    root.close();
    return dirIndex;
}

char *FileManager::findFileExtension(char *filename)
{
    char *dot = nullptr;

    // Traverse the string to find the last dot '.'
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') {
            dot = &filename[i + 1];  // Point to the character after the dot
        }
    }

    // Check if an extension was found
    return dot;
}

void FileManager::listDir(const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(file.path(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void FileManager::createDir(const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}

void FileManager::removeDir(const char *path)
{
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path))
    {
        Serial.println("Dir removed");
    }
    else
    {
        Serial.println("rmdir failed");
    }
}

void FileManager::readFile(const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    Serial.println();
    file.close();
}

void FileManager::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

void FileManager::appendFile(const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

void FileManager::renameFile(const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        Serial.println("File renamed");
    }
    else
    {
        Serial.println("Rename failed");
    }
}

void FileManager::deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
}

void FileManager::closeAndOpenNext(File &root, File &file)
{
    file.close();
    file = root.openNextFile();
}
