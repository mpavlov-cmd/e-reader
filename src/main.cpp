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
	indexText("/books/japanese_homes.txt", display, fileManager);

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
	// DirIndex di = fileManager.indexDirectory("/books", fileManager.DIR_IDX_FULL);
	File file = fileManager.openFile(path, FILE_READ);
	if (!file) {
		Serial.printf("Unable to open file %s\n", path);
	}

	Serial.printf("File size: %i\n", file.size());

	// Define variables for screen size
	int16_t x, y;
    uint16_t width, height, batteryStart;

	uint64_t globalIndex = 0;
	uint16_t lineIndex   = 0;
	uint8_t wordIndex    = 0;
		
	char prevChar;
	String word = "";
	String line = "";
	String page = "";

	boolean skipNextLineBreak = false;
	boolean isCharPrintable   = false;
	boolean endOfWord         = false;
	boolean endOfLine         = false;

	while (file.available()) {
		
		char c = file.read();

		// End of word and line
		if (c == ' ' || c == '\n') {
			// Mafk end of word
			endOfWord = true;

			// Mesaure line length
			String tempLine = line;
			tempLine.concat(word);
			tempLine.trim();

			display.getTextBounds(tempLine, 0, 0, &x, &y, &width, &height);

			if (width >= textAreaWidth - 20) {
				endOfLine = true;
				skipNextLineBreak = true;
			}
		}

		if (endOfLine) {
			// Add Line to the page
			line.trim();
			page.concat(line);
			page.concat('\n');
			lineIndex++;

			line.clear();
			endOfLine = false;
		}

		if (endOfWord) {
			word.trim();
			line.concat(word);
			wordIndex++;

			line.concat(' ');
			word.clear();
			endOfWord = false;
		}

		// if (prevChar == '\n')
		// {
		// 	word.concat(' ');
		// }

		// Add char to word
		if (c != ' ' && isPrintable(c)) {
			word.concat(c);
		}

		if (c == '\n') {
			if (!skipNextLineBreak) {
				word.trim();
				line.concat(word);

				line.trim();
				page.concat(line);
				page.concat('\n');
				lineIndex++;

			line.clear();

			} else {
				skipNextLineBreak = false;
			}
		}
		
		// Save prev. char before quit 
		prevChar = c;
		globalIndex++;
		
		if (lineIndex > 100) {
			break;
		}

		if (globalIndex >= 2048) {
			break;
		}
	}
	
	Serial.println("Page:");
	Serial.println(page);

	display.setFullWindow();
	display.setCursor(0, 20);
	display.firstPage();
	do {
		display.print(page);
	} while (display.nextPage());

	file.close();
}
