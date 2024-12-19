#define ENABLE_GxEPD2_GFX 1

#include <unity.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <set/Set.h>

#include <GxEPD2_dispaly_tools.h>
#include <widget/WidgetText.h>

DBox textBox{24, 48, 432, 704, 2, 0};

void setUp(void)
{   
    Serial.begin(115200);

    // Initalize display as it is used by text index
    initDisplay();
    clearDisplay();
}

void tearDown(void)
{
    // clean stuff up here
}

void testRendersOneLineOfText_void(void) {

    // Given
    WidgetText widgetText(display);
    ModelText modelText{textBox, TOP_LEFT, "Single line text"};

    // When
    widgetText.upgrade(modelText);

    // Then
    TEST_ASSERT_TRUE(true);
}

void testRendersMultileLinesOfText_void(void) {

    // Given
    String text = "This is multi-line text which should fit into the screen.\n";
    text += "Second line goes here, it is approxumately the lenght as 1\n";
    text += "and this is the third and the final line, looks good Yes?\n";
    text += "\n";
    text += "this line goes after double line break, see if it is good!";

    WidgetText widgetText(display);
    ModelText modelText{textBox, TOP_LEFT, text};

    // When
    widgetText.upgrade(modelText);

    // Then
    TEST_ASSERT_TRUE(true);
}


// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testRendersOneLineOfText_void);
    RUN_TEST(testRendersMultileLinesOfText_void);

    UNITY_END(); // stop unit testing
}

void loop()
{
}