#ifndef INTENTBOOK_H
#define INTENTBOOK_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <widget/WidgetText.h>
#include <text/TextIndex.h>
#include <cache/DirectoryCache.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_task_wdt.h>

struct IntentBook : public AbstractDisplayIntent
{
private:

    static constexpr const uint16_t PAGE_BUFFER_SIZE = 5120;

    SemaphoreHandle_t& displaySemaphoreHandle;
    TextIndex& textIndex;
    FileManager& fileManager;

    char bookPath[512];
    char bookPage[PAGE_BUFFER_SIZE];
    DBox textBox{24, 48, 432, 704, 2, 0};

    ModelText* modelText = nullptr;
    WidgetText* widgetText = nullptr;

    EventGroupHandle_t bookEventGroup = NULL;
    TaskHandle_t bookLoadingHandle = NULL;
    TaskHandle_t bookDisplayHandle = NULL;

    void bookLoadingTask();
    void bookDiaplayTask();

    // Static wrappers function for xTaskCreate
    static void bookLoadingEntry(void *param) {
        IntentBook *self = static_cast<IntentBook *>(param);
        self->bookLoadingTask(); // Call the private member function
    }

    static void bookDisplayEntry(void *param) {
        IntentBook *self = static_cast<IntentBook *>(param);
        self->bookDiaplayTask(); // Call the private member function
    }

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_BOOK;

    IntentBook(GxEPD2_GFX &display, SemaphoreHandle_t& displaySemaphoreHandle, TextIndex &textIndex, FileManager &fileManager);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;

    ~IntentBook()
    {
        delete widgetText;
    }
};

#endif