#include "FileManager.h"

const DirIndexConf DirIndexConf::FULL   = {true, true, false, nullptr};
const DirIndexConf DirIndexConf::NO_DIR = {false, true, false, nullptr};

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

bool FileManager::exists(const char *path)
{
    return fs.exists(path);
}

File FileManager::openFile(const char *path, const char *mode)
{
    Serial.printf("Opening file: %s\n", path);
    return fs.open(path, mode);
}

bool FileManager::indexDirectory(const char *path, const DirIndexConf conf, Set<FileIndex>& result)
{
    Serial.printf("Indexing directory: %s\n", path);
    File root = fs.open(path, FILE_READ);

    // TODO: Duplicate code
    if (!root)
    {
        Serial.println("Failed to open directory");
        return false;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        root.close();
        return false;
    }

    // Init Dir Index
    File file = root.openNextFile();
    while (file)
    {
        // Serial.printf("Opening file: %s\n", file.name());
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
        const char* fileExt = getFileExtension(file.name());

        // Filter by ext. See correct way to compare 2 strings (const char *)
        if (conf.filterByExt && strcmp(conf.extNeedle, fileExt) != 0)
        {
            closeAndOpenNext(root, file);
            continue;
        }

        // Serial.printf("Found file ext: %s\n", fileExt);
        FileIndex *fileIndex = new FileIndex(file.name(), file.path(), fileExt, file.size(), file.isDirectory());
        result.addItem(fileIndex);

        closeAndOpenNext(root, file);
    }

    root.close();
    return true;
}

bool FileManager::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return false;
    }
    bool result = file.print(message);
    file.close();

    return result;
}

void FileManager::removeDirRecursive(const char *path)
{
    File root = fs.open(path);

    Serial.printf("Attempt for recursively remove directory: %s\n", path);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }

    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        root.close();
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            // Recursively delete contents of this directory
            removeDirRecursive(file.path());

            Serial.printf("Deleted directory: %s\n", file.path());
        }
        else
        {
            // It's a file, delete it
            fs.remove(file.path());
            Serial.printf("Deleted file: %s\n", file.path());
        }
        // Move to the next file or directory in the folder
        file.close();
        file = root.openNextFile();
    }

    // Once all files and folders are deleted, delete the root directory itself
    file.close();
    root.close();
    fs.rmdir(path);
    Serial.printf("Deleted root directory: %s\n", path);
}

bool FileManager::readFileToBuffer(const char *path, char *buffer, size_t bufferSize)
{
    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return false;
    }

    size_t index = 0;

    while (file.available() && index < bufferSize - 1) // Leave space for null terminator
    {
        buffer[index++] = file.read();
    }
    buffer[index] = '\0'; // Null terminate the string

    file.close();

    return true;
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

    Serial.println("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    Serial.println();
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

void FileManager::closeAndOpenNext(File &root, File &file)
{
    file.close();
    file = root.openNextFile();
}
