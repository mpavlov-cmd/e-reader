#ifndef ABSTRACTDISPLAYINTENT_H
#define ABSTRACTDISPLAYINTENT_H

#include <AbstractIntent.h>
#include <GxEPD2_BW.h>

struct AbstractDisplayIntent : public AbstractIntent
{
    protected:
        GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> display;

    public:
        AbstractDisplayIntent(GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display);
};


#endif