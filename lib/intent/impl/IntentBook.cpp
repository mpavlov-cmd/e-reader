#include "IntentBook.h"

void IntentBook::bookLoadingTask()
{
    Serial.println("Book loading task stated");

    // Indicate processing start
    xEventGroupSetBits(bookEventGroup, BIT0);
 
      // Index Book
    TextIndex::Conf conf{(uint16_t)(textBox.width - textBox.padding), (uint16_t)(textBox.height - textBox.padding), 50, false};
    textIndex.configure(conf);

    // Configure and run text index
    xEventGroupSetBits(bookEventGroup, BIT1);
    String textIndexDirPath = textIndex.index(bookPath);
    const char *textIndexDirPathC = textIndexDirPath.c_str();
    Serial.printf("-- Index Generated! Dir: %s --\n", textIndexDirPathC);
    xEventGroupSetBits(bookEventGroup, BIT2);

    // Check if directory index exists
    // TODO: Injet as a dependency?
    DirectoryCache directoryCahe(fileManager);

    DirectoryCache::Model dirCacheModel;
    bool hasDirCache = directoryCahe.read(textIndexDirPathC, dirCacheModel);

    if (!hasDirCache)
    {
        Set<FileIndex> fileIndex(1024);
        fileManager.indexDirectory(textIndexDirPathC, DirIndexConf::FIRST_FILE, fileIndex);

        if (fileIndex.size() == 0)
        {
            Serial.println("Coud not index directory TODO: Handle this case");
            return;
        }

        FileIndex *firstPage = fileIndex.getItem(0);
        Serial.printf("First page file name: %s\n", firstPage->getName());

        dirCacheModel.curFileIdx = 0;
        dirCacheModel.totalFiles = fileIndex.size();
        dirCacheModel.lastOpened = millis();

        strlcpy(
            dirCacheModel.curFileNme,        // <- destination
            firstPage->getName(),            // <- source
            sizeof(dirCacheModel.curFileNme) // <- destination's capacity
        );

        bool writeResult = directoryCahe.write(textIndexDirPathC, dirCacheModel);
        if (!writeResult)
        {
            Serial.println("Failed to write directory cache");
            return;
        }
    }

    String pagePath = textIndexDirPath + "/" + dirCacheModel.curFileNme;
    const char *pagePacthC = pagePath.c_str();

    // TODO: Resut ignored
    fileManager.readFileToBuffer(pagePacthC, bookPage, PAGE_BUFFER_SIZE);

    Serial.println("Page:");
    Serial.println(bookPage);

    xEventGroupSetBits(bookEventGroup, BIT3);

    // TODO: Bug here, move vTaskDelete to the function wrapper
    vTaskDelete(NULL);
}

void IntentBook::bookDiaplayTask()
{
    Serial.println("Book display task stated");

    widgetText = new WidgetText(display);
    ModelText modelTextObj = {textBox, CENTER_CENTER, "Loading book..."};

    EventBits_t uxBits;
    while (true)
    {
        Serial.println("UI thread waiting for evernts");
        uxBits = xEventGroupWaitBits(
            bookEventGroup, 
            BIT0 | BIT1 | BIT2 | BIT3,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY
        );

        // Book loading started
        if ((uxBits & BIT0) != 0)
        {
            Serial.println("BIT0 set");

            xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
            widgetText->upgrade(modelTextObj);
            xSemaphoreGive(displaySemaphoreHandle);

            xEventGroupClearBits(bookEventGroup, BIT0);
            continue;
        }

        // Stargetd book idex
        if ((uxBits & BIT1) != 0)
        {
            Serial.println("BIT1 set");

            // Check if indexing coompleted and exit
            if ((uxBits & BIT2) != 0)
            {
                Serial.println("BIT2 set, clearing all bites");
                xEventGroupClearBits(bookEventGroup, BIT1 | BIT2);
                continue;
            }

            // Create widget output
            TextIndex::StatusValue indexStatus = textIndex.status();
            String statusValue = String(indexStatus.value);
            String statusText;

            switch (indexStatus.status)
            {
            case TextIndex::STATUS_CHECKSUM:
                statusText = "Calculatig checksum:";
                break;
            
            case TextIndex::STATUS_CLEANUP:
                statusText = "Cleaning-up files:";
                break;
            
            case TextIndex::STATUS_INDEXING:
                statusText = "Indexing Pages:";
                break;

            default:
                statusText = "Loading Book...";
                break;
            }

            Serial.printf("%s %s\n",statusText.c_str(), statusValue.c_str());
            modelTextObj.text = statusText + " " + statusValue;

            xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
            widgetText->upgrade(modelTextObj);
            xSemaphoreGive(displaySemaphoreHandle);

            vTaskDelay(250 / portTICK_RATE_MS);
            continue;
        }

        if ((uxBits & BIT3) != 0)
        {

            Serial.println("BIT3 set, printing book and exiting");
            modelTextObj.text = bookPage;

            xSemaphoreTake(displaySemaphoreHandle, portMAX_DELAY);
            widgetText->upgrade(modelTextObj);
            xSemaphoreGive(displaySemaphoreHandle);

            xEventGroupClearBits(bookEventGroup, BIT3);
            // Exit UI thread loop
            break;
        }
    }

    vTaskDelete(NULL);
}

IntentBook::IntentBook(GxEPD2_GFX &display, SemaphoreHandle_t& displaySemaphoreHandle, TextIndex &textIndex, FileManager &fileManager) :
    AbstractDisplayIntent(display), displaySemaphoreHandle(displaySemaphoreHandle), textIndex(textIndex), fileManager(fileManager) { }

void IntentBook::onStartUp(IntentArgument arg)
{
    Serial.printf("Book intent started with arg: %s\n", arg.strValue);
    strlcpy(bookPath, arg.strValue, sizeof(bookPath));

    bookEventGroup = xEventGroupCreate();

    // Ensure UI task is runnng with higher priority
    if (bookEventGroup != NULL) {
        xTaskCreate(bookLoadingEntry, "bookLoading", 1024 * 10, this, 1, &bookLoadingHandle);
        xTaskCreate(bookDisplayEntry, "bookDisplay", 1024 *  5, this, 5, &bookDisplayHandle);
    }
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
