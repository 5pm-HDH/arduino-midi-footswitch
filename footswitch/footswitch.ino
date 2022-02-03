#include "MIDIUSB.h"

#define FOOT_BUTTONS 6
#define INTERRUPT_PINS 3

uint8_t foot_pins[FOOT_BUTTONS] = { 4,  5,  6,  8,  9, 10};
uint8_t channels[FOOT_BUTTONS] =  {14, 15, 16, 17, 18, 19};
uint8_t interrupt_pins[INTERRUPT_PINS] = {2, 3, 7};

unsigned long previousMillis[FOOT_BUTTONS] = {0, 0, 0, 0, 0, 0};
uint8_t debounce = 20;
uint8_t ping = 1000;
unsigned long previousPing = 0;

volatile int8_t pressed = -1;

int prevState[FOOT_BUTTONS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() {
  for(int i = 0; i < FOOT_BUTTONS; i++) {
    pinMode(foot_pins[i], INPUT_PULLUP);
  }
  for(int i = 0; i < INTERRUPT_PINS; i++) {
    pinMode(interrupt_pins[i], INPUT_PULLUP);
  }
  
  pinMode(17, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(interrupt_pins[0]), interFunc0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interrupt_pins[1]), interFunc1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interrupt_pins[2]), interFunc2, CHANGE);
}

void interFunc0() {
  pressed = 0;
}

void interFunc1() {
  pressed = 2;
}

void interFunc2() {
  pressed = 4;
}

void controlChange(byte channel, byte control, byte value) {

  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};

  MidiUSB.sendMIDI(event);
}

void loop() {
  unsigned long currentMillis = millis();
  if(pressed != -1) {
      int8_t i = pressed;
    if((currentMillis - previousMillis[i]) > debounce && digitalRead(foot_pins[i]) != prevState[i]) {
      previousMillis[i] = currentMillis;
      doChange(i);
    } else if((currentMillis - previousMillis[++i]) > debounce && digitalRead(foot_pins[i]) != prevState[i]) {
      previousMillis[i] = currentMillis;
      doChange(i);
    }
  }
  if((currentMillis - previousPing) > ping) {
    MidiUSB.flush();
  }
}

void doChange(int i) {
  if(digitalRead(foot_pins[i]) == LOW && prevState[i] == HIGH) {
    prevState[i] = LOW;
    controlChange(0, channels[i], 0);
    MidiUSB.flush();
  } else if(digitalRead(foot_pins[i]) == HIGH && prevState[i] == LOW) {
    prevState[i] = HIGH;
    controlChange(0, channels[i], 127);
    MidiUSB.flush();
  }
}
