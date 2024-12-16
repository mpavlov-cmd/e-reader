#ifndef WIDGETIMAGE_H
#define WIDGETIMAGE_H

#include "AbstractWidget.h"
#include "ImageDrawer.h"
#include "model/DPosition.h"

struct ImageModel
{
    const char* filePath;
    DPosition position;
};

class WidgetImage : public AbstractWidget<ImageModel>
{

public:
    WidgetImage(GxEPD2_GFX &mDisplay, ImageDrawer &imageDrawer, FileManager& fileManager) : 
        AbstractWidget(mDisplay), imageDrawer(imageDrawer), fileManager(fileManager) {}
    ~WidgetImage() {}

private:

    ImageDrawer& imageDrawer;
    FileManager& fileManager;

    BmpData bmpData;
    int16_t startX = 0, startY = 0; 

    virtual DBox createBox(ImageModel &widgetData) override {
        // Open file and find image data
        File file = fileManager.openFile(widgetData.filePath, FILE_READ);
        bmpData = imageDrawer.getImageData(file, true);

        // TODO: Handle case when file cannot be opened
        // TODO: Handle case when file is bigger that screen
        // TODO: Make sure width and height are increased so when %8 the remainder is 0
        if (widgetData.position == CENTER_CENTER) {
            startX = display.width() / 2 - bmpData.width / 2;
            startY = display.height() / 2 - bmpData.height / 2;
        }

        return {startX, startY, (uint16_t) bmpData.width, (uint16_t) bmpData.height, 0, 0};
    };

    virtual void initialize(ImageModel &widgetData) override {

    };

    virtual void beforePrint(ImageModel &widgetData) override {

    };

    virtual void print(ImageModel &widgetData) override {
        // To keep image drawer stateless, print method will do nothing
        // Actual image rendering will be don in afterPrint method
    };

    virtual void afterPrint(ImageModel &widgetData) override {
        File file = fileManager.openFile(widgetData.filePath, FILE_READ);
        imageDrawer.drawBitmapFromSD_Buffered(file, startX, startY, false, true);
    };
};

#endif