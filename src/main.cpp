#define ENABLE_GxEPD2_GFX 1
#define SERIAL_DEBUG_ENABLED 1

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/rtc_io.h"
#include "SD.h"
#include "FS.h"

#include <ESP32Time.h>

#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_display_selection_new_style.h>
#include <Fonts/FreeSans9pt7b.h>

#include <DateTimeRtc.h>
#include <FileManager.h>
#include <Battery.h>
#include <SleepControl.h>

#include <home/HomeIntent.h>
#include <PowerStatus.h>
#include <ImageDrawer.h>
#include <text/TextIndex.h>
#include <SwithInputHandler.h>
#include <widget/PowerWidget.h>

#define PIN_LED      GPIO_NUM_2  // LED and BOOT MODE

#define PIN_PWR_DET  GPIO_NUM_25 // CHECK IF HAS EXTERNAL POWER  
#define PIN_BAT_STAT GPIO_NUM_26 // ANALOG BATTEY PERCENT 
#define PIN_CS_SD    GPIO_NUM_27 // SD CHIP SELECT

#define PIN_CHG_DET  GPIO_NUM_35 // HIGH OR LOW WHEN BATTERY CHARGING

// PIN definitions for input
#define BT_INPUT_0 GPIO_NUM_36 // SENSOR_VP PIN 4
#define BT_INPUT_1 GPIO_NUM_39 // SENSOR_VN PIN 5
#define BT_INPUT_2 GPIO_NUM_34 // PIN 6 INPUT ONLY

// put function declarations here
void IRAM_ATTR isr();
void initDisplay();
void blink(void* pvParameters);

void taskIntentFreq(void* pvParameters);
void taskStatusDspl(void* pvParameters);

volatile bool isrPending = false; 
volatile unsigned long isrStartedAt = 0;

// Sleep params
unsigned long lastUserInteraction;
RTC_DATA_ATTR unsigned int bootCount = 0;

// Service instantiation
SleepControlConf conf = {GPIO_SEL_34 | GPIO_SEL_36 | GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW};
SleepControl sleepControl(conf);

ESP32Time rtc(0);
FileManager fileManager(SD, PIN_CS_SD);
TextIndex textIndex(display, fileManager);
SwithInputHandler inputHandler(BT_INPUT_2, BT_INPUT_1, BT_INPUT_0);
ImageDrawer imageDrawer(display);
MenuWidget menuWidget(display);
ClockWidget clockWidget(display);
HomeIntent homeIntent(display, rtc, fileManager, imageDrawer, menuWidget, clockWidget);

PowerStatus powerStatus(PIN_PWR_DET, PIN_CHG_DET, PIN_BAT_STAT);
PowerWidget powerWidget(display);

// Semaphore to assure scheduled task
SemaphoreHandle_t semaphoreHandle;
// Task handle for intent refresh
TaskHandle_t intentFreqHandle = NULL;
TaskHandle_t statusDsplHandle = NULL;

// mapping of Good Display ESP32 Development Kit ESP32-L, e.g. to DESPI-C02
// BUSY -> GPIO13, RES -> GPIO12, D/C -> GPIO14, CS-> GPIO27, SCK -> GPIO18, SDI -> GPIO23
void setup()
{
	Serial.begin(115200);
	Serial.println("-------- BOOT SUCCESS --------");

	Serial.printf("Boot count: %i\n", ++bootCount);

	// Configure wake-up source
	sleepControl.configureExt1WakeUp();

	// Indicate that I'm alive
	lastUserInteraction = millis();
	xTaskCreate(blink, "blinky", 1000, NULL, 5, NULL);

	// Semaphore instancese to assure 2 tasks do not update display
	semaphoreHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(semaphoreHandle);

	// Init Inputs and 
	inputHandler.configure(isr, 100, 2500);
	rtc.setTime(0, 0, 0, 15, 9, 2024);

	fileManager.begin();
	initDisplay();

	homeIntent.onStartUp();

	xTaskCreatePinnedToCore(taskIntentFreq, "intentFreq", 2048, NULL, 1, &intentFreqHandle, 0);
	xTaskCreatePinnedToCore(taskStatusDspl, "statusDspl", 2048, NULL, 1, &statusDsplHandle, 0);
}

void loop()
{
	uint8_t switchInput = inputHandler.handleInput(isrPending, isrStartedAt);
	if (switchInput) {
		
		// Capture lest interaction 
		lastUserInteraction = millis();
		
		// Run action
		xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
		Serial.println(switchInput, BIN);
		homeIntent.onAction(switchInput);
		xSemaphoreGive(semaphoreHandle);
	}
}

void IRAM_ATTR isr()
{
	if (isrPending) {
		return;
	}

	isrStartedAt = millis();
	isrPending = true;
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
		vTaskDelay(500 / portTICK_RATE_MS);
		digitalWrite(PIN_LED, LOW);
		vTaskDelay(500 / portTICK_RATE_MS);

		unsigned long lastInputTime = millis() - lastUserInteraction;
		// Serial.printf("Last Input time %i\n", lastInputTime);
		if (lastInputTime >= 30000) {
			Serial.println("Idle timeout reached. Ready to sleep!");
			break;
		}
	}

	digitalWrite(PIN_LED, LOW);
	display.hibernate();
	sleepControl.sleepNow();
}

void taskIntentFreq(void *pvParameters)
{
	for(;;) {
		// Serial.println("Clock Task");
		xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
		homeIntent.onFrequncy();
		xSemaphoreGive(semaphoreHandle);
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

void taskStatusDspl(void *pvParameters)
{
	for (;;) {
		xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
		
		// Measure and print power metrics
		PowerMetrics powerMetrics = powerStatus.measure();
		powerWidget.upgrade(powerMetrics);

		xSemaphoreGive(semaphoreHandle);
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}
