#ifndef ABSTRACTINTENT_H
#define ABSTRACTINTENT_H

#pragma once

#include <Arduino.h>

enum ActionRetultType { VOID, CHANGE_INTENT };

struct ActionResult {
    ActionRetultType type;
    uint8_t id;

    static const ActionResult VOID;
};

struct AbstractIntent {

    public:
        virtual void onStartUp() = 0;
        virtual void onFrequncy() = 0;
        virtual void onExit() = 0;

        virtual ActionResult onAction(uint16_t actionId) = 0;
        virtual uint8_t getId() = 0;

        AbstractIntent();

    virtual ~AbstractIntent() {} // Virtual destructor
};


#endif