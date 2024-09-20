#include "AbstractDisplayIntent.h"

AbstractDisplayIntent::AbstractDisplayIntent(
    GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display
) : display(display)
{
}