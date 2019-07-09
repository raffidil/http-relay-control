#include <Arduino.h>
#include <TaskScheduler.h>
#include <WiFiManager.h>
#include <stdlib.h>
#include "Timer.h"
#include <queue>

#define PIN_KEY D3
const int PIN_RELAY1 = D2;
const int PIN_RELAY2 = D5;
const int PIN_LED_ON = D6;
const int PIN_LED_OFF = D8;


long buttonDebounce = 200;
long long buttonTime = 0;

bool lastPinValue = HIGH;
bool currentValue;

int state = HIGH;      // the current state of the output pin

WiFiServer server(80);
WiFiManager wifiManager;

Timer t;

Scheduler runner;
void serverLoop();
void timerLoop();
void buttonLoop();
bool serverSetup();


Task serverTask(100, TASK_FOREVER, &serverLoop, NULL, false, &serverSetup);
Task buttonTask(50, TASK_FOREVER, &buttonLoop, NULL, false);
Task timerTask(50, TASK_FOREVER, &timerLoop, NULL, false);

std::queue<int> timerQueue;

void doAfterOn()
{
  Serial.println("TIMER ON");
  state = LOW;
  digitalWrite(PIN_RELAY1, state);
  digitalWrite(PIN_RELAY2, state);

  digitalWrite(PIN_LED_ON, HIGH);
  Serial.println("Relays on");
}
void doAfterOff()
{
  Serial.println("TIMER OFF");
  state = HIGH;
  digitalWrite(PIN_RELAY1, state);
  digitalWrite(PIN_RELAY2, state);

  digitalWrite(PIN_LED_OFF, HIGH);
  Serial.println("Relays off");
}

void buttonLoop() {
  currentValue = digitalRead(PIN_KEY);
  if (currentValue == HIGH && lastPinValue == LOW &&
      millis() - buttonTime > buttonDebounce) {
    state = !state;

    digitalWrite(PIN_RELAY1 , state);
    digitalWrite(PIN_RELAY2 , state);
    Serial.print("relays: ");
    Serial.println(state ? "OFF" : "ON");

    buttonTime = millis();
  }

  lastPinValue = currentValue;
}

void timerLoop(){
   t.update();
}


void setup () {
  Serial.begin(115200);
      // Off LED
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  pinMode(PIN_RELAY1, OUTPUT);
  pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_KEY, INPUT_PULLUP);
  pinMode(PIN_LED_ON, OUTPUT);
  pinMode(PIN_LED_OFF, OUTPUT);

  digitalWrite(PIN_RELAY1, state);
  digitalWrite(PIN_RELAY2, state);
  digitalWrite(PIN_LED_ON, HIGH);
  digitalWrite(PIN_LED_OFF, HIGH);



  // Inint task schedueler
  runner.init();
  runner.addTask(serverTask);
  runner.addTask(buttonTask);
  runner.addTask(timerTask);
  delay(100);

  serverTask.enable();
  buttonTask.enable();
  timerTask.enable();
}


void loop () {
  runner.execute();
}
