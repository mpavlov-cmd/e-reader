#include "SleepControl.h"

SleepControl::SleepControl(SleepControlConf config) : config(config)
{
}

void SleepControl::sleepNow()
{
    if (extWakeUpConfResult != ESP_OK) {
        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.printf("Unable to sleep, configuration failure: %i\n", extWakeUpConfResult);
        #endif
        return;
    }

    #if defined(SERIAL_DEBUG_ENABLED)
    Serial.println("Entering deep sleep. Zzzz...");
    #endif
    esp_deep_sleep_start();
}

bool SleepControl::setWarkupTimer(uint16_t wakeupTimerSecons)
{
    esp_err_t result = esp_sleep_enable_timer_wakeup(wakeupTimerSecons * US_TO_S_FACTOR);
    
    #if defined(SERIAL_DEBUG_ENABLED)
    Serial.printf("Set wake up timer, reult: %i\n", result);
    #endif

    return result == ESP_OK;
}

esp_sleep_wakeup_cause_t SleepControl::getWarkeupCause()
{
    return esp_sleep_wakeup_cause_t();
}

void SleepControl::configureExt1WakeUp()
{
    extWakeUpConfResult = esp_sleep_enable_ext1_wakeup(config.gpioMask, config.mode);

    #if defined(SERIAL_DEBUG_ENABLED)
    Serial.printf("Ext1 wakeup configured: %i\n", extWakeUpConfResult);
    #endif
}
