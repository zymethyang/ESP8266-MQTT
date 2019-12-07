#ifndef Memory_h
#define Memory_h

#include "Arduino.h"

class Memory
{
  public:
    Memory(float c01, float c02, float c03);
    void setState(String jsonString);
    float getChart01();
    float getChart02();
    float getChart03();

    int getDevice01();
    int getDevice02();
    int getDevice03();
    int getDevice04();
    int getDevice05();
    
  private:
    int _device_01 = 2;
    int _device_02 = 2;
    int _device_03 = 2;
    int _device_04 = 2;
    int _device_05 = 2;

    float _chart_01 = 0;
    float _chart_02 = 0;
    float _chart_03 = 0;
};

#endif
