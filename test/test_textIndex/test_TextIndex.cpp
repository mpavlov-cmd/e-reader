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

TextIndex::Conf indexConf = {432, 704, 0, true};

const char* PATH_TEST_DIR   = "/.test";
const char* PATH_SHORT_TEXT = "/.test/text_short.txt";
const char* PATH_LONG_TEXT  = "/.test/text_long.txt";

String text = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.";

FileManager fileManager(SD, PIN_CS_SD);
TextIndex testSubject(display, fileManager);

void setUp(void)
{   
    Serial.begin(115200);

    // Initalize display as it is used by text index
    initDisplay();

    fileManager.begin();
    fileManager.createDir(PATH_TEST_DIR);

    // Prepare files
    fileManager.writeFile(PATH_SHORT_TEXT, text.c_str());

    String longText;
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        longText.concat(text);
    }

    longText.concat(" Long!");
    fileManager.writeFile(PATH_LONG_TEXT, longText.c_str());

    Serial.println("---------------- SETUP DONE ----------------");
    Serial.println();
}

void tearDown(void)
{
    Serial.println();
    Serial.println("---------------- TEST DONE ----------------");

    // clean stuff up here
    fileManager.removeDirRecursive(PATH_TEST_DIR);
}

void testSmallFileIndexed_nonEmpty(void) {

    // Given 
    testSubject.configure(indexConf);

    // When
    String filePath = testSubject.generateIdx(PATH_SHORT_TEXT);
    const char* filePathCharArr = filePath.c_str();

    // Then
    // Index dir created
    TEST_ASSERT_EQUAL_STRING("/.test/._text_short.txt_21a3_idx", filePathCharArr);
    File fileTestTemp = fileManager.openFile(filePathCharArr, FILE_READ);
    
    TEST_ASSERT_TRUE(fileTestTemp.isDirectory());

    // There is one index fie
    Set<FileIndex> resultindex(8);
    bool success = fileManager.indexDirectory(fileTestTemp.path(), DirIndexConf::FULL, resultindex);
    TEST_ASSERT_EQUAL_INT(1, resultindex.size());
    fileTestTemp.close();

    // It has expecetd name
    FileIndex fileIndex = *resultindex.getItem(0);
    TEST_ASSERT_EQUAL_STRING("._0.page.txt", fileIndex.getName());
    
    // It contains data and ends with the same word origial text ends
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    fileManager.readFileToBuffer(fileIndex.getPath(), buffer, bufferSize);

    String fileData(buffer);  
    TEST_ASSERT_TRUE(fileData.endsWith("dolor."));
}

void testMultiPageFileIndexed_hasFullData(void) {

    // Given 
    testSubject.configure(indexConf);

    // When
    String filePath = testSubject.generateIdx(PATH_LONG_TEXT);
    const char* filePathCharArr = filePath.c_str();

    TEST_ASSERT_TRUE(fileManager.exists(filePathCharArr));
    TEST_ASSERT_EQUAL_STRING("/.test/._text_long.txt_841f_idx", filePathCharArr);

    // There are multiple index fies
    Set<FileIndex> resultindex(32);
    bool success = fileManager.indexDirectory(filePathCharArr, DirIndexConf::FULL, resultindex);
    uint16_t indexSize = resultindex.size();
    TEST_ASSERT_EQUAL_INT(17, indexSize);

    FileIndex lastFileIndex = *resultindex.getItem(indexSize - 1);
     
    // It contains data and ends with the same word origial text ends
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    fileManager.readFileToBuffer(lastFileIndex.getPath(), buffer, bufferSize);

    // Verify last index ends the same as the original text
    String fileData(buffer);  
    TEST_ASSERT_TRUE(fileData.endsWith("Long!"));
}


// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testSmallFileIndexed_nonEmpty);
    RUN_TEST(testMultiPageFileIndexed_hasFullData);

    UNITY_END(); // stop unit testing
}

void loop()
{
}