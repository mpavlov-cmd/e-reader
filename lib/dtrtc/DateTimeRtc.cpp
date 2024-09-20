#include "DateTimeRtc.h"

DateTimeRtc::DateTimeRtc(uint8_t hr, uint8_t mt, uint8_t sc, uint8_t d, uint8_t m, uint8_t y)
{
    hours = hr;
    minutes = mt;
    seconds = sc;
    month = m;
    day = d;
    year = y;

    changeMask = FULL_MASK;
}

DateTimeRtc::DateTimeRtc() : DateTimeRtc(23, 59, 55, 31, 12, 99)
{
}

uint8_t DateTimeRtc::verify(uint8_t value, uint8_t index)
{
    // Handle max days in moonth - special case
    if (index == IDX_DAY)
    {
        uint8_t maxDays = maxDaysInMonth(month);

        if (value < 1)
        {
            return maxDays;
        }

        return value > maxDays ? 1 : value;
    }

    // Handle min month
    if (index == IDX_MON)
    {
        // Handle corner cases 
        if (value < 1)
        {
            return VALUE_LIMIT[index];
        }
        if (value > VALUE_LIMIT[index])
        {
            return 1;
        }

        // Handle case when month is changes and day value is greater than max
        // Avoid redundand calculations 
        if (day > 27) {
            uint8_t maxDays = maxDaysInMonth(value);
            if (day > maxDays) {
                setValue(maxDays, IDX_DAY);
            }
        }
    }

    // Stay positive for other cases
    if (value == 255)
    {
        return VALUE_LIMIT[index];
    }

    // Default behavior
    return value > VALUE_LIMIT[index] ? 0 : value;
}

uint8_t DateTimeRtc::maxDaysInMonth(uint8_t month)
{
    uint8_t maxDays;

    // Handle leap year in february
    if (month == 2)
    {
        boolean leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        maxDays = leapYear ? 29 : 28;
    }
    else
    {
        maxDays = DAYS_IN_MONTH[month - 1];
    }

    return maxDays;
}

void DateTimeRtc::setValue(uint8_t value, uint8_t index)
{
    uint8_t existingValue = byIndex(index);
    if (existingValue == value)
    {
        return;
    }

    uint8_t vefified = verify(value, index);

    // Verify Debug
    // Serial.print(index);
    // Serial.print("-");
    // Serial.print(value);
    // Serial.print("-");
    // Serial.println(vefified);

    // Mark the value as changed
    byIndex(index) = vefified;
    bitSet(changeMask, index);
}

uint8_t DateTimeRtc::getMask()
{
    return changeMask;
}

void DateTimeRtc::forseMask()
{
    changeMask = FULL_MASK;
}

void DateTimeRtc::flushMask()
{
    changeMask = ZERO_MASK;
}

uint8_t &DateTimeRtc::byIndex(uint8_t idx)
{
    switch (idx)
    {
    case IDX_HOUR:
        return hours;
    case IDX_MIN:
        return minutes;
    case IDX_SEC:
        return seconds;
    case IDX_DAY:
        return day;
    case IDX_MON:
        return month;
    case IDX_YEAR:
        return year;
    default:
        return seconds;
    }
}
