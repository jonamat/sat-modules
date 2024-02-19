#include <PubSubClient.h>
#include "../module.h"

#define SAMPLING_RATE 10

extern PubSubClient mqttClient;

/**
 * @brief AnalogInput
 * @details Read the binary state of a pin and send the new state on change.
 *
 * @param topic Root topic of the device.
 * @param pin Pin to use.
 * @param delay Delay between each state check. Default is 1000ms.
 * @param tolerance Tolerance to trigger a state change. Default is 10.
 * @param task_priority Priority of the task. Default is P_M (medium).
 *
 * @note Available commands:
 * "STATE" publish the current state of the device
 *
 * TODO:
 * - Add resolution
 * - Add cycles
 * - Add sampling
 *
 */
class AnalogInput : public Module
{
private:
  int pin;
  uint delay;
  int tolerance;
  int state;

public:
  AnalogInput(
    String topic,
    int pin,
    uint delay = 1000,
    int tolerance = 10,
    TSK_PRT task_priority = TSK_PRT::P_M
  ) {
    this->topic = topic;
    this->pin = pin;
    this->delay = delay;
    this->tolerance = tolerance;
    this->task_priority = task_priority;
  }

  static void task(void* param) {
    AnalogInput* pThis = (AnalogInput*)param;

    while (true) {
      int samples = 0;
      for (int i = 0; i < SAMPLING_RATE; i++) {
        samples += analogRead(pThis->pin);
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }

      int currentState = int(samples / SAMPLING_RATE);

      if (currentState > pThis->state + pThis->tolerance || currentState < pThis->state - pThis->tolerance) {
        pThis->state = currentState;

        char c_state[4];
        sprintf(c_state, "%d", pThis->state);
        mqttClient.publish(pThis->topic.c_str(), c_state);
      }

      vTaskDelay(pThis->delay / portTICK_PERIOD_MS);
    }
  }

  void setup() {
  }

  void onCommand(String* payload) {
    if ((*payload) == "STATE") {
      int currentState = analogRead(this->pin);

      char c_state[4];
      sprintf(c_state, "%d", currentState);
      mqttClient.publish(this->topic.c_str(), c_state);
    }
  }

  BaseType_t start() {
    return xTaskCreate(
      &AnalogInput::task,
      this->topic.c_str(),
      4096,
      this,
      (UBaseType_t)this->task_priority,
      NULL);
  }
};
