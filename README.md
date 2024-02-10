# sat-modules

*WIP*<br />
MQTT bridge interface for ESP32, with cloud free OTA updates.<br />
It provides low level functionalities to develop IOT multi purpose devices<br />
Base template for rvmatic satellites

## Setup

Add devices by creating a new desired module object and put in the *modules* vector.

```cpp
std::vector<Module*> modules = {
  new DigitalOutputDAC("/lights", 33),
  new PWM("/heater", 22),
  new AnalogReader("/bathroom/temperature", 27)
};
```
