#include "MenuDrawer.h"

MenuDrawer::MenuDrawer(GxEPD2_GFX &mDisplay) : display(mDisplay)
{
}

void MenuDrawer::drawMenu(Menu& menu, Box& box, bool fullWindow)
{
    // Get reference to items set stored in the menu
    Set<MenuItem>& menuItems = menu.getItemsSet();
    if (menuItems.size() == 0)
    {
        return;
    }

    // Init Text bounds
    display.getTextBounds(menuItems.getItem(0)->getName(), 0, 0, &x, &y, &width, &height);

    Box mB = box;
    Box activeBox(0, 0, height - 2, height - 2, 0, 0);

    uint8_t linePadding = 3;
    uint16_t lineStartX = mB.getX() + 2;

    uint16_t boxColor, textLineStartY;

    // Set either full or small window
    if (fullWindow)
    {
        display.setFullWindow();
    }
    else
    {
        display.setPartialWindow(mB.getX(), mB.getY(), mB.getWidth(), mB.getHeight());
    }

    display.firstPage();
    do
    {
        // Clear menu page
        display.fillRect(mB.getX(), mB.getY(), mB.getWidth(), mB.getHeight(), GxEPD_WHITE);
        display.drawRect(mB.getX(), mB.getY(), mB.getWidth(), mB.getHeight(), GxEPD_BLACK);
        for (uint16_t i = 0; i < menuItems.size(); i++)
        {
            MenuItem mi = *menuItems.getItem(i);
            boxColor = mi.getIsActive() ? GxEPD_BLACK : GxEPD_WHITE;

            // Text starts from bottom left corner of the cursor
            textLineStartY = mB.getY() + ((i + 1) * (height + linePadding));

            display.fillRect(
                lineStartX,
                textLineStartY - activeBox.getHeight(),
                activeBox.getWidth(),
                activeBox.getHeight(),
                boxColor
            );
            display.setCursor(lineStartX + activeBox.getWidth() + 1, textLineStartY);
            display.print(mi.getName());
        }
    } while (display.nextPage());
}
