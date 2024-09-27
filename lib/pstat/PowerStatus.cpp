#include "PowerStatus.h"


PowerStatus::PowerStatus(
    ESP32Time& espTime,
    uint8_t powerSensePin,
    uint8_t chargeStatusPin,
    uint8_t batterySensePin,
    uint8_t chargeEnablePin
) : 
        espTime(espTime),
        battery(VOLTAGE_MIN_MILLIVOLTS, VOLTAGE_MAX_MILLIVOLTS, batterySensePin, ADC_RESOLUTION),
        powerSensePin(powerSensePin), 
        chargeStatusPin(chargeStatusPin),
        chargeEnablePin(chargeEnablePin)
{
    pinMode(powerSensePin, INPUT);
    pinMode(chargeStatusPin, INPUT);
    pinMode(chargeEnablePin, OUTPUT);
    digitalWrite(chargeEnablePin, LOW);

    // Set 12 bit ADC resolution
    analogReadResolution(ADC_RESOLUTION);
    battery.begin(VOLTAGE_REF_MILLIVOLTS, VOLTAGE_DIVIDER_RAITO, &asigmoidal);
}

boolean PowerStatus::getChageEnabled()
{
    return digitalRead(chargeEnablePin) == HIGH;
}

boolean PowerStatus::getConnected()
{
    return digitalRead(powerSensePin) == HIGH;
}

uint8_t PowerStatus::getBatteryLevelPercent()
{
    unsigned long currentTime = espTime.getLocalEpoch();  

     if (currentTime > timeLatch + CHARGING_MEASURE_DELAY_SEC && measureingDuringCharge) {
            
        uint8_t levelReading = battery.level();
        voltageDuringMeasuring = battery.voltage();

        Serial.println("---------MEASURING AFTER DELAY --------");
        Serial.println(voltageDuringMeasuring);
        Serial.println(levelReading);

        // Put value to latch
        batteryPercentLatch = levelReading;

        // Exit measuring mode and re-enable charging
        measureingDuringCharge = false;
        allowCharge(true);
    }

    // If power not connected, battery percent should only decrease
    boolean currentlyConnected = getConnected();
    if (!measureingDuringCharge && currentlyConnected) {

        if (currentTime > timeLatch + CHARGING_MEASURE_INTERVAL_SEC) {
            Serial.println("------------ DIABLE CHARGE FOR MEASURING ------------");
            
            allowCharge(false);
            measureingDuringCharge = true;

            timeLatch = currentTime;
        }

    }

    if (!currentlyConnected) {
         uint8_t levelReading = battery.level();
         // TODO: Reomve
         voltageDuringMeasuring = battery.voltage();

        if (batteryPercentLatch > levelReading) {
            batteryPercentLatch = levelReading;
        } 
    }

    connectedLatch = currentlyConnected;
    return batteryPercentLatch;
}

uint16_t PowerStatus::getBatteryVoltage()
{
    return battery.voltage();
}

uint16_t PowerStatus::getVoltageDuringMeasuring()
{
    return voltageDuringMeasuring;
}

// TODO: Implement logic to indicate battery not connected
ChargeingStatus PowerStatus::getChargingStatus()
{
    return digitalRead(chargeStatusPin) == LOW ? CHARGING : FULL;
}

void PowerStatus::allowCharge(boolean chargeAllowed)
{
    uint8_t highOrLow = chargeAllowed == true ? HIGH : LOW;
    digitalWrite(chargeEnablePin, highOrLow);
}
