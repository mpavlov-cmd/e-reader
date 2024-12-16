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

    // TODO: Take position into considearation
    virtual void print(ModelText &widgetData) override {

        const char* text = widgetData.text.c_str();
        // Serial.printf("Text to print inside of widget: %s\n", text);

        display.setCursor(box.x + box.padding, box.y + height +  box.padding);
        display.print(text);
    };

    // Restore default text size and font
    virtual void afterPrint(ModelText &widgetData) override {
       
    };

};

#endif