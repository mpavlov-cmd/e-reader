#ifndef ABSTRACTINTENT_H
#define ABSTRACTINTENT_H

#pragma once

#include <Arduino.h>

struct AbstractIntent {

    public:
        virtual String getName() = 0;
        virtual void onStartUp() = 0;
        virtual void onFrequncy() = 0;
        virtual void onAction(uint16_t actionId) = 0;
        virtual void onExit() = 0;

        AbstractIntent();

    virtual ~AbstractIntent() {} // Virtual destructor
};


#endif