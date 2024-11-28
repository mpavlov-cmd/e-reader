#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "AbstractWidget.h"
#include "menu/Menu.h"

class MenuWidget : public AbstractWidget<Menu>
{
public:
    MenuWidget(GxEPD2_GFX &mDisplay) : AbstractWidget(mDisplay) {}

private:
    int16_t x = 0, y = 0;
    uint16_t width = 0, height = 0, lineStartX = 0;
    uint8_t linePadding = 3;
    DBox activeBox;

    DBox createBox() override
    {
        return {48, 584, 384, 160, 0, 0};
    }

    void initialize(Menu& widgetData) override {

        Set<MenuItem> &menuItems = widgetData.getItemsSet();
        if (menuItems.size() == 0)
        {
            return;
        }

        // Init Text bounds
        display.getTextBounds(menuItems.getItem(0)->getName(), 0, 0, &x, &y, &width, &height);

        // Do not do any calculations if menu items are empty
        lineStartX = box.x + 2;

        uint16_t activeBoxSide = height - 2;
        activeBox = {0, 0, activeBoxSide, activeBoxSide, 0, 0};
    }

    void beforePrint(Menu& widgetData) override {

    }

    void print(Menu& widgetData) override
    {
        Set<MenuItem> &menuItems = widgetData.getItemsSet();

        uint16_t boxColor, textLineStartY;
        for (uint16_t i = 0; i < menuItems.size(); i++)
        {
            MenuItem mi = *menuItems.getItem(i);
            boxColor = mi.getIsActive() ? GxEPD_BLACK : GxEPD_WHITE;

            // Text starts from bottom left corner of the cursor
            textLineStartY = box.y + ((i + 1) * (height + linePadding));

            display.fillRect(
                lineStartX,
                textLineStartY - activeBox.height,
                activeBox.width,
                activeBox.height,
                boxColor
            );

            display.setCursor(lineStartX + activeBox.width + 1, textLineStartY);
            display.print(mi.getName());
        }
    }
};

#endif