// Pass display as parameter
#define ENABLE_GxEPD2_GFX 1

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "SD.h"
#include "FS.h"

#include <ESP32Time.h>

#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_display_selection_new_style.h>
#include <Fonts/FreeSans9pt7b.h>

#include <DateTimeRtc.h>
#include <home/HomeIntent.h>
#include <PowerStatus.h>
#include <powerindicator/PowerIndicator.h>
#include <Battery.h>
#include <FileManager.h>
#include <ImageDrawer.h>

#define PIN_LED      GPIO_NUM_25 // LED
#define PIN_PWR_DET  GPIO_NUM_34 // CHECK IF HAS EXTERNAL POWER  
#define PIN_CHG_DET  GPIO_NUM_35 // HIGH OR LOW WHEN BATTERY CHARGING
#define PIN_BAT_STAT GPIO_NUM_36 // ANALOG BATTEY PERCENT 

#define PIN_CHG_ON   GPIO_NUM_15 // Set HIGH TO ENABLE CHARGER
#define PIN_CS_SD    GPIO_NUM_26 // SD Schip Select

#define PIN_BT_L GPIO_NUM_12
#define PIN_BT_C GPIO_NUM_13
#define PIN_BT_R GPIO_NUM_14

const uint16_t BAT_V_MIN_MILLIVOLTS = 3000;
const uint16_t BAT_V_MAX_MILLIVOLTS = 4200;

// put function declarations here
// void helloWorld();
void initDisplay();
void blink(void *pvParameters);
void indexText(const char* path, GxEPD2_GFX& display, FileManager& fileManager);

// TODO: 
void savePage(const char* path, const char* name, String contents);

// put constant definitions here

// var declarations 
ESP32Time rtc(0);
PowerStatus powerStatus(rtc, PIN_PWR_DET, PIN_CHG_DET, PIN_BAT_STAT, PIN_CHG_ON);
FileManager fileManager(SD, PIN_CS_SD);

// static uint8_t homeIntentBuffer[sizeof(HomeIntent)];
// HomeIntent* homeIntent = nullptr;
// PowerIndicator* powerInd = nullptr;

// mapping of Good Display ESP32 Development Kit ESP32-L, e.g. to DESPI-C02
// BUSY -> GPIO13, RES -> GPIO12, D/C -> GPIO14, CS-> GPIO27, SCK -> GPIO18, SDI -> GPIO23
void setup()
{
	Serial.begin(115200);
	while (!Serial) {
    	; // wait for serial port to connect.
  	}
	Serial.println("-------- BOOT SUCCESS --------");

	// Buttons test
	pinMode(PIN_BT_L, INPUT_PULLUP);
	pinMode(PIN_BT_C, INPUT_PULLUP);
	pinMode(PIN_BT_R, INPUT_PULLUP);

	// Indicate that I'm alive
	xTaskCreate(blink, "blinky", 1000, NULL, 5, NULL);

	// Set Time
	rtc.setTime(0, 0, 0, 15, 9, 2024);

	// Init File Manager
	fileManager.begin();

	// Init Display
	initDisplay();

	// TODO: Temp should go after init display
	// indexText("/books/japanese_homes.txt", display, fileManager);
	indexText("/books/water.txt", display, fileManager);

	// HomeIntent homeIntent(display, rtc);
	// homeIntent.onStartUp();

	// Get Random image for random() first argument inclusive, second exclusive
	// DirIndex dirIndex = fileManager.indexDirectory("/background", {false, false, true, "bmp"});
	// uint8_t randFileIndex = random(0, dirIndex.size());
	// const char* randoomPath = dirIndex.byIndex(randFileIndex).getPath();

	// // Create Image Viewer 
	// File image = fileManager.openFile(randoomPath, FILE_READ);
	// ImageDrawer imageDrawer(display);
	// imageDrawer.drawBitmapFromSD_Buffered(image, 0, 100, false, false, false);

	// // Power Indicator 
	// PowerIndicator powerInd(display, powerStatus);
	// powerInd.init();

	// xTaskCreate(powerBatteryStatus, "powerBatteryStatus", 1000, NULL, 5, NULL);
	// Sleep
	// display.hibernate();

	for (;;) {
		// // homeIntent.onFrequncy(10);
	 	// powerInd.refresh();
		// delay(5000);
	}
}

void loop()
{
	// powerInd->refresh();
}

void initDisplay()
{
	// Use this for Waveshare boards with "clever" reset circuit, 2ms reset pulse;
	// Use serial_diag_bitrate = 115200 for logging
	// display.init(0, true, 2, false); 

	display.init(0); 

	display.setRotation(3);
	display.setFont(&FreeSans9pt7b);
	display.setTextSize(1);
	display.setTextColor(GxEPD_BLACK);

	display.setFullWindow();
	display.firstPage();

	do
	{
		display.fillScreen(GxEPD_WHITE);
	} while (display.nextPage());

	// display.setPartialWindow(0, 0, display.width(), display.height());
}


void blink(void *pvParameters) {
	pinMode(PIN_LED, OUTPUT);
	for (;;) {
		digitalWrite(PIN_LED, HIGH);
		vTaskDelay(250 / portTICK_RATE_MS);
		digitalWrite(PIN_LED, LOW);
		vTaskDelay(250 / portTICK_RATE_MS);
		// Serial.println(digitalRead(PIN_BT_C));
	}
}

void indexText(const char *path, GxEPD2_GFX &display, FileManager &fileManager)
{
	const uint16_t textAreaWidth  = 480;
	const uint16_t textAreaHeight = 760;

	Serial.printf("Opening file %s\n", path);
	File file = fileManager.openFile(path, FILE_READ);
	if (!file) {
		Serial.printf("Unable to open file %s\n", path);
	}

	Serial.printf("File size: %i\n", file.size());

	// ----------------- BEGIN TEMP DIR CREATION ------------------
	const char* parentDir = fileManager.getParentDir(file.path());
	Serial.println("Dir Name:");
	Serial.println(parentDir);

	// Chreat temp dir name
	String filename = String(file.name());
	String fileSizeStr = String(file.size());
	filename.toLowerCase();

	String idxDirName = "._" + filename + "_" + fileSizeStr + "_idx";	
	String idxDirPath = String(parentDir) + "/" + idxDirName;

	const char* idxDirPathCharArr = idxDirPath.c_str();

	if (!fileManager.exists(idxDirPathCharArr)) {
		Serial.printf("Creating directory: %s\n", idxDirPathCharArr);
		fileManager.createDir(idxDirPathCharArr);
	}

	Serial.printf("Idx directory path: %s\n", idxDirPathCharArr);

	// ----------------- END TEMP DIR CREATION ------------------

	// Define variables for screen size
	int16_t x, y;
    uint16_t width = 0, height = 0, wordWidth = 0, linewidth = 0;

	// Get Space width
	display.getTextBounds("s w", 0, 0, &x, &y, &width, &height);
	uint8_t widthLarge = width;
	display.getTextBounds("ws", 0, 0, &x, &y, &width, &height);
	uint8_t widthSmall = width;
    uint8_t spaceWidth = widthLarge - widthSmall;

	// Find fine one line height 
	display.getTextBounds("LINE\n\nLINE", 0, 0, &x, &y, &width, &height);
	uint8_t lineHeight = height / 3 + 2;
	uint16_t numLines  = textAreaHeight / lineHeight - 1; 
	Serial.printf("Lines per page: %i, lineh eight: %i\n", numLines, lineHeight);
	
	String currentPage = "";
	String currentLine = ""; // Holds the text for the current line
	String currentWord = ""; // Holds the current word being built

	// int currentLineWidth = 0;	// Tracks the pixel width of the current line
	char c;
	bool skipLeadingSpaces = true;  // Skip leading spaces on new lines
	bool wrappedLine       = false; // Track if the last line was wrapped
	bool spaceAfterWrap    = false; 

	uint16_t lineIndex = 0;
	uint16_t pageIndex = 0;

	while (file.available())
	{
		c = file.read(); // Read the file character by character

		// If we hit a newline or space, treat it as a word boundary
		if (c == ' ' || c == '\n')
		{
			// Drop values in case word or line are empty
			wordWidth = linewidth = height = 0;
			display.getTextBounds(currentWord, 0, 0, &x, &y, &wordWidth, &height); // Get the pixel width of the current word
			display.getTextBounds(currentLine, 0, 0, &x, &y, &linewidth, &height); // Get the pixel width of the current word

			// Check if the current word fits on the line
			if (linewidth + spaceWidth + wordWidth >= textAreaWidth - 2)
			{
				// If the word doesn't fit, print the current line and start a new one
				currentPage.concat(currentLine);
				currentPage.concat('\n');
				currentLine = "";		  // Reset current line
				// currentLineWidth = 0;  // Reset width counter
				skipLeadingSpaces = true; // Skip leading spaces for the next line
				wrappedLine = true;		  // Mark that the line was wrapped
				spaceAfterWrap = true;

				lineIndex++;
			}

			// Add the word to the current line, but only if it's not an empty line
			if (!skipLeadingSpaces || currentWord.length() > 0)
			{
				currentLine += currentWord;
				// currentLineWidth += width;

				if (wrappedLine && spaceAfterWrap) {
					currentLine += ' ';
					// currentLineWidth += spaceWidth;
					spaceAfterWrap = false;
				}
			}

			// Add space if it's not the start of a new line
			if (c == ' ')
			{
				// Avoid more then one space in row
				if (!skipLeadingSpaces && (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' '))
				{
					currentLine += ' ';
					// currentLineWidth += spaceWidth;
				}
			}

			// If there's a newline, print the current line and move to the next line
			if (c == '\n')
			{
				if (!wrappedLine)  // Only print the line if it wasn't already wrapped
				{							 
					currentPage.concat(currentLine); // Display the current line
					currentPage.concat('\n');
					lineIndex++;

					currentLine = "";		  // Reset the line
					// currentLineWidth = 0;  // Reset width counter
					skipLeadingSpaces = true; // Skip leading spaces for the next line
				} else {
					// Add space instead of '\n' when line was wrapped
					if (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' ') {
						currentLine += ' ';
						// currentLineWidth += spaceWidth;
					}
				}

				wrappedLine = false;
			}

			// Clear the current word
			currentWord = "";
		}
		
		else if (c != ' ' && isPrintable(c))
		{
			// If it's a normal character, build the current word
			currentWord += c;
			skipLeadingSpaces = false; // We've found non-space content, so we stop skipping spaces
		}

		if (lineIndex >= numLines) {
			String pageFileName = "._" + String(pageIndex) + ".page.txt";
			savePage(idxDirPathCharArr, pageFileName.c_str(), currentPage);
			
			currentPage = "";
			pageIndex++;
			lineIndex = currentLine.isEmpty() ? 0 : 1;
		}

		if (pageIndex > 10) {
			break;
		}
	}

	if (!currentLine.isEmpty())
	{
		currentPage.concat(currentLine);
	}

	// Print Random page

	DirIndex dirIndex = fileManager.indexDirectory(idxDirPathCharArr, {false, true, true, "txt"});
	Serial.println(dirIndex.size());

	uint8_t randFileIdxNum = random(0, dirIndex.size());
	FileIndex randFileIdx  = dirIndex.byIndex(randFileIdxNum);

	Serial.println("----- File Index Data ------");
	Serial.println(randFileIdx.getPath());
	Serial.println(randFileIdx.getSize());

	File randomPageFile = fileManager.openFile(randFileIdx.getPath(), FILE_READ);

	String randomPage = "";
	while (randomPageFile.available())
	{
		char tc = randomPageFile.read();
		randomPage.concat(tc);
	}
	
	randomPageFile.close();

	Serial.println("Page:");
	Serial.println(randomPage);

	display.setFullWindow();
	display.setCursor(0, 20);
	display.firstPage();
	do {
		display.print(randomPage);
	} while (display.nextPage());

	file.close();
}

// TODO: Path just filename to avoid multiple back-n-force string conversion
void savePage(const char* path, const char* name, String contents) {
	String fileName = String(path) + '/' + name;
	bool result = fileManager.writeFile(fileName.c_str(), contents.c_str());
	if (result) {
		Serial.println("Success");
	} else {
		Serial.println("Failed");
	}
}