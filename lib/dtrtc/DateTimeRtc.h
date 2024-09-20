#ifndef DATETIMERCT_H
#define DATETIMERCT_H

#include <Arduino.h>

const uint8_t ZERO_MASK = 0b000000;
const uint8_t FULL_MASK = 0b111111;
const uint8_t VALUE_LIMIT[] = {23, 59, 59, 0, 12, 99};
const uint8_t DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

const uint8_t IDX_HOUR = 0;
const uint8_t IDX_MIN  = 1;
const uint8_t IDX_SEC  = 2;
const uint8_t IDX_DAY  = 3;
const uint8_t IDX_MON  = 4;
const uint8_t IDX_YEAR = 5;

struct DateTimeRtc {
    private:
        uint8_t hours, minutes, seconds, day, month, year;
        uint8_t changeMask = ZERO_MASK;
        
        uint8_t verify(uint8_t value, uint8_t index);
        uint8_t maxDaysInMonth(uint8_t month);

    public:
        DateTimeRtc(uint8_t hr, uint8_t mt, uint8_t sc, uint8_t y, uint8_t m, uint8_t d);
        DateTimeRtc();

        void setValue(uint8_t value, uint8_t index);
        
        uint8_t getMask();
        void forseMask();
        void flushMask();

        uint8_t& byIndex(uint8_t idx);
};

#endif  // DATETIMERCT_H
