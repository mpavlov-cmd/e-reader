#ifndef ABSTRACTINTENT_H
#define ABSTRACTINTENT_H

#pragma once

#include <Arduino.h>
#include <IntentArgument.h>

enum ActionRetultType { VOID, CHANGE_INTENT };

struct ActionResult {
    static const ActionResult VOID;

    ActionRetultType type;
    uint8_t id;
    IntentArgument data;
};

struct AbstractIntent {

    public:
        virtual void onStartUp(IntentArgument) = 0;
        virtual void onFrequncy() = 0;
        virtual void onExit() = 0;

        virtual ActionResult onAction(uint16_t actionId) = 0;
        virtual uint8_t getId() = 0;

        AbstractIntent();

    virtual ~AbstractIntent() {} // Virtual destructor
};


#endif