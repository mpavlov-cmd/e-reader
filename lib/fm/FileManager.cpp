#include "FileManager.h"

const DirIndexConf DirIndexConf::FULL = {true, true, false, nullptr};

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
    File root = fs.open(path);

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
        Serial.printf("Opening file: %s\n", file.name());
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
        const char* fileExt = findFileExtension(file.name());

        // Filter by ext. See correct way to compare 2 strings (const char *)
        if (conf.filterByExt && strcmp(conf.extNeedle, fileExt) != 0)
        {
            closeAndOpenNext(root, file);
            continue;
        }

        Serial.printf("Found file ext: %s\n", fileExt);

        FileIndex *fileIndex = new FileIndex(file.name(), file.path(), fileExt, file.size(), file.isDirectory());
        result.addItem(fileIndex);

        closeAndOpenNext(root, file);
    }

    root.close();
    return true;
}

// TODO: Potentially move to file utils
const char *FileManager::findFileExtension(const char *filename)
{
    const char *dot = "";

    // Traverse the string to find the last dot '.'
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '.') {
            dot = &filename[i + 1];  // Point to the character after the dot
        }
    }

    // Check if an extension was found
    return dot;
}

// TODO: Rewrite to avoid static char allocation findFileExtension method 
const char* FileManager::getParentDir(const char *path)
{
    const char *lastSlash = strrchr(path, '/'); // Find the last occurrence of '/'
	if (lastSlash != nullptr)
	{
		size_t dirLength = lastSlash - path;			 // Calculate the length of the directory part
		char *directory = (char *)malloc(dirLength + 1); // Allocate memory for the directory string
		strncpy(directory, path, dirLength);			 // Copy the directory part
		directory[dirLength] = '\0';					 // Null-terminate the string
		return directory;
	}
	return "/";
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
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            // Recursively delete contents of this directory
            removeDirRecursive(file.path());
            // Remove the directory itself after its contents are deleted
            fs.rmdir(file.path());
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
    root.close();
    fs.rmdir(path);
    Serial.printf("Deleted root directory: %s\n", path);
}

// TODO: Rewrite to avoid static char allocation findFileExtension method 
const char *FileManager::getPreviousLevel(const char *path)
{
    // Check if the input path is root "/"
    if (strcmp(path, "/") == 0)
    {
        return "/";
    }

    // Determine the length of the path
    size_t length = strlen(path);

    // Remove trailing slash if present (but ensure root "/" is unchanged)
    if (length > 1 && path[length - 1] == '/')
    {
        length--;
    }

    // Find the last slash in the adjusted path
    for (size_t i = length; i > 0; --i)
    {
        if (path[i - 1] == '/')
        {
            // If the last slash is at the beginning, return root
            if (i == 1)
            {
                return "/";
            }

            // Allocate a new string for the previous level
            // When static variable allocation is used, the space will be reserverd in static mameory once and forever. 
            // Thus, no need to do a cleanup
            static char previousLevel[256]; // Adjust size as needed for your environment
            strncpy(previousLevel, path, i - 1);
            previousLevel[i - 1] = '\0';

            return previousLevel;
        }
    }

    // If no slash is found, return root (this shouldn't occur with valid input)
    return "/";
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
