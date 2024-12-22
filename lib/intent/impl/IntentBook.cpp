#include "IntentBook.h"

void IntentBook::bookLoadingTask()
{
    Serial.println("Book loading task stated");
    xEventGroupSetBits(bookEventGroup, BIT0);
    vTaskDelete(NULL);
}

void IntentBook::bookDiaplayTask()
{
    Serial.println("Book display task stated");

    EventBits_t uxBits;
    while(true) {
        uxBits = xEventGroupWaitBits(bookEventGroup, BIT0 | BIT4, pdTRUE, pdFALSE, portMAX_DELAY);
        if( (uxBits & BIT0) != 0)
        {
            Serial.println("BIT0 set");
            break;
        }
    }
    
    widgetText = new WidgetText(display);
    ModelText modelTextObj = {textBox, CENTER_CENTER, "Loading book..."};

    xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
    widgetText->upgrade(modelTextObj);
    xSemaphoreGive(displaySemaphoreHandle);
   
    vTaskDelete(NULL);
}

IntentBook::IntentBook(GxEPD2_GFX &display, SemaphoreHandle_t& displaySemaphoreHandle, TextIndex &textIndex, FileManager &fileManager) :
    AbstractDisplayIntent(display), displaySemaphoreHandle(displaySemaphoreHandle), textIndex(textIndex), fileManager(fileManager) { }

void IntentBook::onStartUp(IntentArgument arg)
{
    Serial.printf("Book intent started with arg: %s\n", arg.strValue);
    bookEventGroup = xEventGroupCreate();

    if (bookEventGroup != NULL) {
        xTaskCreate(bookLoadingEntry, "bookLoadingTask", 1024 * 5, this, 1, NULL);
        xTaskCreate(bookDisplayEntry, "bookDisplayTask", 1024 * 5, this, 1, NULL);
    }

    // // Index Book
    // TextIndexConf conf{
    //     (uint16_t)(textBox.width - textBox.padding),
    //     (uint16_t)(textBox.height - textBox.padding),
    //     0,
    //     false};
    // textIndex.configure(conf);

    // // Configure and run text index
    // String textIndexDirPath = textIndex.generateIdx(arg.strValue);
    // const char *textIndexDirPathC = textIndexDirPath.c_str();
    // Serial.printf("-- Index Generated! Dir: %s --\n", textIndexDirPathC);

    // // Check if directory index exists
    // // TODO: figure out why in case passed as argument: fails
    // DirectoryCache directoryCahe(fileManager);

    // DirectoryCache::Model dirCacheModel;
    // bool hasDirCache = directoryCahe.read(textIndexDirPathC, dirCacheModel);

    // if (!hasDirCache)
    // {
    //     Set<FileIndex> fileIndex(1024);
    //     fileManager.indexDirectory(textIndexDirPathC, DirIndexConf::NO_DIR, fileIndex);

    //     if (fileIndex.size() == 0)
    //     {
    //         Serial.println("Coud not index directory TODO: Handle this case");
    //         return;
    //     }

    //     FileIndex *firstPage = fileIndex.getItem(0);
    //     Serial.printf("First page file name: %s\n", firstPage->getName());

    //     dirCacheModel.curFileIdx = 0;
    //     dirCacheModel.totalFiles = fileIndex.size();
    //     dirCacheModel.lastOpened = millis();

    //     strlcpy(
    //         dirCacheModel.curFileNme,        // <- destination
    //         firstPage->getName(),            // <- source
    //         sizeof(dirCacheModel.curFileNme) // <- destination's capacity
    //     );

    //     bool writeResult = directoryCahe.write(textIndexDirPathC, dirCacheModel);
    //     if (!writeResult)
    //     {
    //         Serial.println("Failed to write directory cache");
    //         return;
    //     }
    // }

    // String pagePath = textIndexDirPath + "/" + dirCacheModel.curFileNme;
    // const char *pagePacthC = pagePath.c_str();

    // File firstPageFile = fileManager.openFile(pagePacthC, FILE_READ);
    // String firstPageString;
    // while (firstPageFile.available())
    // {
    //     char tc = firstPageFile.read();
    //     firstPageString.concat(tc);
    // }

    // firstPageFile.close();

    // Serial.println("Page:");
    // Serial.println(firstPageString);

    // ModelText modelTextObj = {textBox, CENTER_CENTER, firstPageString};
    // modelText = &modelTextObj;

    // // Serial.pintf does not work with Srings, it will work as text.c_str()
    // // Serial.printf("--- Model text inside of statup: %s ---- \n", modelTextObj.text.c_str());
    // widgetText = new WidgetText(display);
    // widgetText->upgrade(*modelText);
}

void IntentBook::onFrequncy()
{
    Serial.println("IntentBook on frequency");
}

void IntentBook::onExit()
{
    Serial.println("IntentBook on exit");
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    // In case input hed -> go home
    if (arg.held)
    {
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    // TODO: Implemet loading, implement next page / previous page

    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
