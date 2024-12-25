#define ENABLE_GxEPD2_GFX 1
#define SERIAL_DEBUG_ENABLED 1

#include <Arduino.h>
#include <PinDefinitions.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "driver/rtc_io.h"
#include "SD.h"
#include "FS.h"

#include <ESP32Time.h>

#include <GxEPD2_dispaly_tools.h>

#include <DateTimeRtc.h>
#include <FileManager.h>
#include <Battery.h>
#include <SleepControl.h>

#include <IntentIdentifier.h>
#include <impl/IntentHome.h>
#include <impl/IntentSleep.h>
#include <impl/IntentFileSelector.h>
#include <impl/IntentBook.h>


#include <PowerStatus.h>
#include <ImageDrawer.h>
#include <text/TextIndex.h>
#include <SwithInputHandler.h>
#include <ButtonActions.h>
#include <widget/WidgetPower.h>

#define SLEEP_TIMEOUT_MILLS 60 * 1000

// put function declarations here
void IRAM_ATTR isr();

void buildIntent(uint8_t intentId);
void switchIntent(uint8_t intentId, IntentArgument intentArgument);

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

WidgetImage widgetImage(display, imageDrawer, fileManager);

PowerStatus powerStatus(PIN_PWR_DET, PIN_CHG_DET, PIN_BAT_STAT);
WidgetPower widgetPower(display);

// Semaphore to assure scheduled task
SemaphoreHandle_t semaphoreHandle;
// Task handle for intent refresh
TaskHandle_t intentFreqHandle = NULL;
TaskHandle_t statusDsplHandle = NULL;

AbstractIntent* intentCurrent = new IntentHome(display, rtc, fileManager, imageDrawer);

// mapping of Good Display ESP32 Development Kit ESP32-L, e.g. to DESPI-C02
// BUSY -> GPIO13, RES -> GPIO12, D/C -> GPIO14, CS-> GPIO27, SCK -> GPIO18, SDI -> GPIO23
void setup()
{
	Serial.begin(115200);
	Serial.println("-------- BOOT SUCCESS --------");
	Serial.printf("Boot count: %i\n", ++bootCount);

	// Init and configure wdt with timeot seconds and panic flag
	esp_task_wdt_init(60, false);

	// Configure wake-up source
	sleepControl.configureExt1WakeUp();

	// Indicate that I'm alive
	lastUserInteraction = millis();
	xTaskCreate(blink, "blinky", 4096, NULL, 5, NULL);

	// Semaphore instancese to assure 2 tasks do not update display
	semaphoreHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(semaphoreHandle);

	// Init Inputs and 
	inputHandler.configure(isr, 100, 2500);
	rtc.setTime(0, 0, 0, 15, 9, 2024);

	fileManager.begin();

	initDisplay();
	clearDisplay();

	intentCurrent->onStartUp(IntentArgument::NO_ARG);

	xTaskCreatePinnedToCore(taskIntentFreq, "intentFreq", 4096, NULL, 1, &intentFreqHandle, 0);
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

		Serial.print("Loop -> action perfromed: ");
		Serial.println(switchInput, BIN);

		// Extract hold bit and create action arg
		bool held = false;
		uint8_t direction = controlDirection(switchInput, held);
		ActionArgument arg{direction, held};

		// Fire action
		ActionResult result = intentCurrent->onAction(arg);

		// Exit current intent and start next one
		if (result.type == ActionRetultType::CHANGE_INTENT) {
			Serial.printf("Change intent action fired with id: %i\n", result.id);
			switchIntent(result.id, result.data);
		}
		
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


void blink(void *pvParameters) {
	pinMode(PIN_LED, OUTPUT);
	for (;;) {
		// Blink
		digitalWrite(PIN_LED, HIGH);
		vTaskDelay(500 / portTICK_RATE_MS);
		digitalWrite(PIN_LED, LOW);
		vTaskDelay(500 / portTICK_RATE_MS);

		// Validate last input and sleep
		unsigned long lastInputTime = millis() - lastUserInteraction;
		if (lastInputTime >= SLEEP_TIMEOUT_MILLS) {
			Serial.println("Idle timeout reached. Ready to sleep!");
			break;
		}
	}

	// Will not give back semaphore, because deep sleep will be enabled
	xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
	
	// Sweet dreams are made of tears
	switchIntent(INTENT_ID_SLEEP, IntentArgument::NO_ARG);
}

void buildIntent(uint8_t intentId)
{
	switch (intentId)
	{
	case INTENT_ID_HOME:
		intentCurrent = new IntentHome(display, rtc, fileManager, imageDrawer);
		break;
	case INTENT_ID_FILE_SELECTOR:
		intentCurrent = new IntentFileSelector(display, fileManager);
		break;
	case INTENT_ID_SLEEP:
		intentCurrent = new IntentSleep(display, sleepControl, imageDrawer, fileManager);
		break;
	case INTENT_ID_BOOK:
		intentCurrent = new IntentBook(display, semaphoreHandle, textIndex, fileManager);
		break;
	default:
		intentCurrent = new IntentHome(display, rtc, fileManager, imageDrawer);
		break;
	}
}

void switchIntent(uint8_t intentId, IntentArgument intentArgument)
{
	intentCurrent->onExit();
	delete intentCurrent;

	// Init new intent based on the id under the hood will assign new intent to intent current
	buildIntent(intentId);

	clearDisplay();
	intentCurrent->onStartUp(intentArgument);
}

void taskIntentFreq(void *pvParameters)
{
	for(;;) {
		// Serial.println("Clock Task");
		xSemaphoreTake(semaphoreHandle, portMAX_DELAY);
		intentCurrent->onFrequncy();
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
		widgetPower.upgrade(powerMetrics);

		xSemaphoreGive(semaphoreHandle);
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}
