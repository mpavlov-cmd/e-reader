#ifndef ABSTRACTDISPLAYINTENT_H
#define ABSTRACTDISPLAYINTENT_H

#include <AbstractIntent.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>

struct AbstractDisplayIntent : public AbstractIntent
{
    protected:
        GxEPD2_GFX& display;

    public:
        AbstractDisplayIntent(GxEPD2_GFX& display);
};


#endif