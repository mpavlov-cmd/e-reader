#define ENABLE_GxEPD2_GFX 1

#include <unity.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <set/Set.h>

#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_dispaly_tools.h>

#include <PinDefinitions.h>

#include <index/FileIndex.h>
#include <FileManager.h>
#include <text/TextIndex.h>

FileManager fileManager(SD, PIN_CS_SD);
TextIndex testSubject(display, fileManager);

void setUp(void)
{   
    Serial.begin(115200);

    // Initalize display as it is used by text index
    initDisplay();

    // Files are required on SD card so there is a test data
    // 1. /.test/text_short.txt
    // 2. /.test/text_long.txt
    fileManager.begin();
}

void tearDown(void)
{
    // clean stuff up here
}

void testSmallFileIndexed_nonEmpty(void) {

    // Given 
    testSubject.configure(TextIndexConf::DFT);

    // When
    File resultFile = testSubject.generateIdx("/.test/text_short.txt");

    // Then
    // Index dir created
    TEST_ASSERT_EQUAL_STRING("._text_short.txt_83_idx", resultFile.name());

    // There is one index fie
    Set<FileIndex> resultindex(8);
    bool success = fileManager.indexDirectory(resultFile.path(), DirIndexConf::FULL, resultindex);

    TEST_ASSERT_TRUE(1 == resultindex.size());

    // TODO: File contents matches original file contnets

    resultFile.close();
}


// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testSmallFileIndexed_nonEmpty);

    UNITY_END(); // stop unit testing
}

void loop()
{
}