#include "DirectoryCache.h"

DirectoryCache::DirectoryCache(FileManager &fileManager) : fileManager(fileManager)
{
}

bool DirectoryCache::read(const char *path, Model &model)
{
    String fullPath;
    const char *fullPathC = getCacheFilePath(path, fullPath);
    if (fullPathC == nullptr)
    {
        return false;
    }

    if (!fileManager.exists(fullPathC))
    {
        return false;
    }

    File cacheFile = fileManager.openFile(fullPathC, FILE_READ);
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, cacheFile);
    if (error)
    {
        Serial.printf("Unable to read json from file %s\n", fullPathC);

        cacheFile.close();
        return false;
    }

    model.curFileIdx = doc["curFileIdx"] | 0;
    model.totalFiles = doc["totalFiles"] | 0;
    model.lastOpened = doc["lastOpened"] | 0;

    strlcpy(
        model.curFileNme,        // <- destination
        doc["curFileNme"] | "",  // <- source
        sizeof(model.curFileNme) // <- destination's capacity
    );

    cacheFile.close();
    return true;
}

bool DirectoryCache::write(const char *path, Model &model)
{
    String fullPath;
    const char *fullPathC = getCacheFilePath(path, fullPath);
    if (fullPathC == nullptr)
    {
        return false;
    }

    const char* cacheDirPathC = getParentDir(fullPathC);
    if (!fileManager.exists(cacheDirPathC))
    {
        // TODO: create dir should be bool
        fileManager.createDir(cacheDirPathC);
    }

    File cacheFile = fileManager.openFile(fullPathC, FILE_WRITE);
    if (!cacheFile)
    {
        Serial.printf("Failed to open file: %s for writing", fullPathC);
        return false;
    }

    JsonDocument doc;

    doc["curFileIdx"] = model.curFileIdx;
    doc["totalFiles"] = model.totalFiles;
    doc["lastOpened"] = model.lastOpened;
    doc["curFileNme"] = model.curFileNme;

    if (serializeJson(doc, cacheFile) == 0) {
        Serial.printf("Failed to write json file: %s\n", fullPathC);
    }

    cacheFile.close();
    return true;
}

const char *DirectoryCache::getCacheFilePath(const char *path, String &cacheFilePath)
{
     if (path == nullptr)
    {
        return nullptr;
    }

    cacheFilePath.clear();
    cacheFilePath.concat(String(path));
    if (!cacheFilePath.endsWith("/"))
    {
        cacheFilePath.concat("/");
    }

    cacheFilePath.concat(chacheDirName);
    cacheFilePath.concat("/");
    cacheFilePath.concat(chacheFileName);

    return cacheFilePath.c_str();
}
