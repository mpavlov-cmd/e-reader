#ifndef ABSTRACTINTENT_H
#define ABSTRACTINTENT_H

#pragma once

#include <Arduino.h>
#include <GxEPD2_BW.h>

struct AbstractIntent {

    public:
        virtual String getName() = 0;
        virtual void onStartUp() = 0;
        virtual void onFrequncy(uint32_t freqMills) = 0;
        virtual void onAction(uint16_t actionId) = 0;
        virtual void onExit() = 0;

        AbstractIntent();

    virtual ~AbstractIntent() {} // Virtual destructor
};


#endif