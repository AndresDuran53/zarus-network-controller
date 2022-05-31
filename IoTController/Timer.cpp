#include "Timer.h"

int Timer::indexCounter = 0;
Timer* Timer::timersInstances[100];

Timer::Timer(unsigned long intervalInMillis, void (actionToExecute)()) {
  indexCounter++;
  this->intervalInMillis = intervalInMillis;
  this->lastTimeExecuted = 0;
  this->actionToExecute = actionToExecute;
}

void Timer::createTimer(unsigned long intervalInMillis, void (actionToExecute)()) {
  int counter = indexCounter;
  Logger::log("Creating new instance of Timer", Logger::DEBUG_LOG);
  timersInstances[counter] = new Timer(intervalInMillis, actionToExecute);
}

void Timer::executeLoops() {
  for (int i = 0; i < indexCounter; i++) {
    timersInstances[i]->loop();
  }
}

unsigned long Timer::getActualMillis() {
  return millis();
}

boolean Timer::haveToExecute() {
  if (getActualMillis() >= (lastTimeExecuted + intervalInMillis)) {
    return true;
  }
  else {
    return false;
  }
}

void Timer::executed() {
  setLastTimeExecuted(getActualMillis());
}

void Timer::loop() {
  if (haveToExecute()) {
    (*Timer::actionToExecute)();
    executed();
  }
}

void Timer::setActionToExecute(void (actionToExecute)()) {
  this->actionToExecute = actionToExecute;
}

void Timer::setIntervalInMillis(unsigned long intervalInMillis) {
  this->intervalInMillis = intervalInMillis;
}

unsigned long Timer::getInvervalInMillis() {
  return intervalInMillis;
}

void Timer::setLastTimeExecuted(unsigned long lastTimeExecuted) {
  this->lastTimeExecuted = lastTimeExecuted;
}

unsigned long Timer::getLastTimeExecuted() {
  return lastTimeExecuted;
}
