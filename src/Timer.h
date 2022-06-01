#ifndef Timer_h
#define Timer_h

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "Logger.h"


class Timer {
  private:
    static unsigned long getActualMillis();
    unsigned long lastTimeExecuted;
    unsigned long intervalInMillis;
    boolean haveToExecute();
    void executed();
    void (*actionToExecute)();
  public:
    static Timer* timersInstances[100];
    static int indexCounter;
    static void executeLoops();
    static void createTimer(unsigned long intervalInMillis, void (actionToExecute)());
    Timer(unsigned long intervalInMillis, void (actionToExecute)());
    void loop();
    void setActionToExecute(void (actionToExecute)());
    void setIntervalInMillis(unsigned long intervalInMillis);
    unsigned long getInvervalInMillis();
    void setLastTimeExecuted(unsigned long lastTimeExecuted);
    unsigned long getLastTimeExecuted();

};
#endif
