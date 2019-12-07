#include "Arduino.h"
#include "Memory.h"
#include <ArduinoJson.h>

Memory::Memory(float c01, float c02, float c03)
{
  _chart_01 = c01;
  _chart_02 = c02;
  _chart_03 = c03;
}

void Memory::setState(String jsonString)
{
  StaticJsonDocument<128> doc;
  deserializeJson(doc, jsonString);
  if (doc.containsKey("a")) {
    _chart_01 = doc["a"];
  }
  if (doc.containsKey("b")) {
    _chart_02 = doc["b"];
  }
  if (doc.containsKey("c")) {
    _chart_03 = doc["c"];
  }

  if (doc.containsKey("d")) {
    _device_01 = doc["d"];
  }

  if (doc.containsKey("e")) {
    _device_02 = doc["e"];
  }
  if (doc.containsKey("f")) {
    _device_03 = doc["f"];
  }
  if (doc.containsKey("g")) {
    _device_04 = doc["g"];
  }
  if (doc.containsKey("h")) {
    _device_05 = doc["h"];
  }
}

float Memory::getChart01() {
  return _chart_01;
}

float Memory::getChart02() {
  return _chart_02;
}

float Memory::getChart03() {
  return _chart_03;
}

int Memory::getDevice01() {
  return _device_01;
}

int Memory::getDevice02() {
  return _device_02;
}

int Memory::getDevice03() {
  return _device_03;
}


int Memory::getDevice04() {
  return _device_04;
}

int Memory::getDevice05() {
  return _device_05;
}
