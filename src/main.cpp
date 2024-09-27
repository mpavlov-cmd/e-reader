#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <ESP32Time.h>
#include <Battery.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_display_selection_new_style.h>
#include <Fonts/FreeSans9pt7b.h>

#include <DateTimeRtc.h>
#include <home/HomeIntent.h>
#include <PowerStatus.h>
#include <powerindicator/PowerIndicator.h>

#define PIN_LED GPIO_NUM_25      // LED
#define PIN_POWR_DET GPIO_NUM_34 // CHECK IF HAS EXTERNAL POWER  
#define PIN_BAT_STAT GPIO_NUM_36 // ANALOG BATTEY PERCENT 
#define PIN_CHG_STAT GPIO_NUM_35 // HIGH OR LOW WHEN BATTERY CHARGING
#define PIN_CHG_ON   GPIO_NUM_15 // Set HIGH TO ENABLE CHARGER

const uint16_t BAT_V_MIN_MILLIVOLTS = 3000;
const uint16_t BAT_V_MAX_MILLIVOLTS = 4200;

// put function declarations here
// void helloWorld();
void initDisplay();
void blink(void *pvParameters);
void powerBatteryStatus(void *pvParameters);

// put constant definitions here

// var declarations 
ESP32Time rtc(0);
PowerStatus powerStatus(rtc, PIN_POWR_DET, PIN_CHG_STAT, PIN_BAT_STAT, PIN_CHG_ON);

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

	// Indicate that I'm alive
	xTaskCreate(blink, "blinky", 1000, NULL, 5, NULL);

	// Set Time
	rtc.setTime(0, 0, 0, 15, 9, 2024);

	// Init Display
	initDisplay();

	// HomeIntent homeIntent(display, rtc);
	// homeIntent.onStartUp();
	// To de-allocate memory use:
	// homeIntent->~HomeInetnt();
	// homeIntent = nullptr;
	HomeIntent homeIntent(display, rtc);
	// homeIntent.onStartUp();

	// Power Indicator 
	PowerIndicator powerInd(display, powerStatus);

	
	// xTaskCreate(powerBatteryStatus, "powerBatteryStatus", 1000, NULL, 5, NULL);
	// Sleep
	// display.hibernate();

	for (;;) {
		homeIntent.onFrequncy(10);
		powerInd.refresh();
	}
}

void loop()
{
	// powerInd->refresh();
}

void initDisplay()
{
	// USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
	display.init(115200, true, 2, false); 

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
	}
}

void powerBatteryStatus(void *pvParameters)
{
	for (;;) {
		
		Serial.print("Connected:");
		Serial.println(powerStatus.getConnected());

		Serial.print("Chaging Status:");
		Serial.println(powerStatus.getChargingStatus());

		Serial.print("Battery Level:");
		Serial.println(powerStatus.getBatteryLevelPercent());

		vTaskDelay(250 / portTICK_RATE_MS);
	}
}

// void helloWorld()
// {
// 	display.setRotation(3);
// 	display.setFont(&FreeMonoBold9pt7b);
// 	display.setTextColor(GxEPD_BLACK);
// 	int16_t tbx, tby;
// // 	uint16_t tbw, tbh;
// display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
// 	// center the bounding box by transposition of the origin:
// 	uint16_t x = ((display.width() - tbw) / 2) - tbx;
// 	uint16_t y = ((display.height() - tbh) / 2) - tby;
// 	display.setFullWindow();
// 	display.firstPage();
// 	do
// 	{
// 		display.fillScreen(GxEPD_WHITE);
// 		display.setCursor(x, y);
// 		display.print(HelloWorld);
// 	} while (display.nextPage());
// }