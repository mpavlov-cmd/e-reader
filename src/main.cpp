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
#include <text/TextIndex.h>

#define PIN_LED      GPIO_NUM_25 // LED
#define PIN_PWR_DET  GPIO_NUM_34 // CHECK IF HAS EXTERNAL POWER  
#define PIN_CHG_DET  GPIO_NUM_35 // HIGH OR LOW WHEN BATTERY CHARGING
#define PIN_BAT_STAT GPIO_NUM_36 // ANALOG BATTEY PERCENT 

#define PIN_CHG_ON   GPIO_NUM_15 // SET HIGH TO ENABLE CHARGER
#define PIN_CS_SD    GPIO_NUM_26 // SD CHIP SELECT

#define PIN_BT_L GPIO_NUM_12
#define PIN_BT_C GPIO_NUM_13
#define PIN_BT_R GPIO_NUM_14

const uint16_t BAT_V_MIN_MILLIVOLTS = 3000;
const uint16_t BAT_V_MAX_MILLIVOLTS = 4200;

// put function declarations here
void initDisplay();
void blink(void *pvParameters);

// put constant definitions here

// service instantiation declarations 
ESP32Time rtc(0);
PowerStatus powerStatus(rtc, PIN_PWR_DET, PIN_CHG_DET, PIN_BAT_STAT, PIN_CHG_ON);
FileManager fileManager(SD, PIN_CS_SD);
TextIndex textIndex(display, fileManager);

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

	// indexText("/books/japanese_homes.txt", display, fileManager);
	textIndex.configure({480, 760, 0, false});
	File pagesDirFile = textIndex.generateIdx("/books/water.txt");

	// TODO: Temp: showing random page
	unsigned long startIndexMills = millis();
	Serial.println("--- Index Dir Start ---"); 
	DirIndex dirIndex = fileManager.indexDirectory(pagesDirFile.path(), {false, true, true, "txt"});
	Serial.printf("Index dir ended in %i\n", millis() - startIndexMills);
	
	uint8_t randFileIdxNum = random(0, dirIndex.size());
	FileIndex randFileIdx  = dirIndex.byIndex(randFileIdxNum);
	Serial.println("--- File Index Data ---");
	Serial.println(randFileIdx.getPath());

	File randomPageFile = fileManager.openFile(randFileIdx.getPath(), FILE_READ);
	String randomPage = "";
	while (randomPageFile.available())
	{
		char tc = randomPageFile.read();
		randomPage.concat(tc);
	}
	
	randomPageFile.close();
	pagesDirFile.close();

	Serial.println("Page:");
	Serial.println(randomPage);

	display.setFullWindow();
	display.setCursor(0, 20);
	display.firstPage();
	do {
		display.print(randomPage);
	} while (display.nextPage());


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
}


void blink(void *pvParameters) {
	pinMode(PIN_LED, OUTPUT);
	for (;;) {
		digitalWrite(PIN_LED, HIGH);
		vTaskDelay(250 / portTICK_RATE_MS);
		digitalWrite(PIN_LED, LOW);
		vTaskDelay(250 / portTICK_RATE_MS);
	}
}
