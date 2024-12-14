#ifndef ABSTRACTWIGDET_H
#define ABSTRACTWIGDET_H

#include <Arduino.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <box/DBox.h>

template <typename T>
class AbstractWidget
{
    protected:
        GxEPD2_GFX& display;
        DBox box;

    private:
        bool initialized = false; 

        virtual DBox createBox(T& widgetData) = 0;
        virtual void initialize(T& widgetData) = 0;
        virtual void beforePrint(T& widgetData) = 0;
        virtual void print(T& widgetData) = 0;
        virtual void afterPrint(T& widgetData) = 0;

    public:
        AbstractWidget(GxEPD2_GFX& mDisplay) : display(mDisplay), box(DBox()) {};

        virtual ~AbstractWidget() {}

        void upgrade(T& widgetData)
        {

            if (!initialized)
            {
                #if defined(SERIAL_DEBUG_ENABLED)
                Serial.println("Initializing widget");
                #endif
                
                // Create box
                box = createBox(widgetData);

                initialize(widgetData);
                initialized = true;
            }

            beforePrint(widgetData);

            // Draw box 
            display.setPartialWindow(box.x, box.y, box.width, box.height);
            display.firstPage();
            do {
                // Diaplay box
                display.fillRect(box.x, box.y, box.width, box.height, GxEPD_WHITE);
                display.drawRect(box.x, box.y, box.width, box.height, GxEPD_BLACK);

                // Display widget contents
                print(widgetData);
            }
            while (display.nextPage());

            afterPrint(widgetData);
        }
};


#endif
