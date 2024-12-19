#ifndef WIDGETTEXT_H
#define WIDGETTEXT_H

#include <Arduino.h>
#include <widget/AbstractWidget.h>
#include <model/text/ModelText.h>

class WidgetText : public AbstractWidget<ModelText>
{

private:
    const char TEXT_SAMPLE[7] = "09AZ%^"; 

    int16_t x, y;
    uint16_t width, height;

public:
    WidgetText(GxEPD2_GFX &mDisplay) : AbstractWidget(mDisplay) {}
    ~WidgetText() {}

    virtual DBox createBox(ModelText &widgetData) override {
         return widgetData.box;
    };

    virtual void initialize(ModelText &widgetData) override {

    };

    // Add text size and font to the model
    virtual void beforePrint(ModelText &widgetData) override {
        display.getTextBounds(TEXT_SAMPLE, 0, 0, &x, &y, &width, &height);
    };

    // TODO: Take position into considearation,
    virtual void print(ModelText &widgetData) override
    {
        // Init variables for cursor
        const uint8_t lineSpacing = 2;
        const char *text = widgetData.text.c_str();

        int cursorX = box.x + box.padding;
        int cursorY = box.y + height + box.padding;

        display.setCursor(box.x + box.padding, box.y + height + box.padding);

        for (size_t i = 0; i < widgetData.text.length(); ++i)
        {
            if (text[i] == '\n')
            {
                cursorY += height + lineSpacing; // Move cursor to the next line
                cursorX = box.x + box.padding;   // Reset cursor to the start of the line
                display.setCursor(cursorX, cursorY);
            }
            else
            {
                display.print(text[i]);
            }
        }
    };

    // Restore default text size and font
    virtual void afterPrint(ModelText &widgetData) override {
       
    };

};

#endif