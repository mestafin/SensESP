#ifndef _multi_consumer_H_
#define _multi_consumer_H_

#include <ArduinoJson.h>
#include <stdint.h>

#include "sensesp.h"

namespace sensesp {

class ValueProducer;
template <typename T>

/**
 * @brief Provides an easy way of calling a function based on
 * the output of any ValueProducer.
 *
 * <p>To use LambdaConsumer, you create the function in
 * main.cpp as a lambda expression, and then pass it as the "function"
 * parameter to the LambaConsumer constructor.
 * See
 * https://github.com/SignalK/SensESP/blob/master/examples/lambda_consumer.cpp
 * for a detailed example.
 *
 * @tparam IN Consumer function input value type
 *
 * @param function A pointer to the function that's being called.
 * */

//template <typename T>
class MultiConsumer {
 public:
  //

  MultiConsumer(std::function<void(String[8], int[8])> function) {}

  void set_input_string(String input, uint8_t input_channel = 0) {
    input_string_[input_channel] = input;
   function(input_string_[8], input_int_[8]);
  }

  void set_input_int(int input, uint8_t input_channel = 0) {
    input_int_[input_channel] = input;
   function(input_string_[8], input_int_[8]);
  }

  void connect_from_string(ValueProducer<String>* producer,
                           uint8_t input_channel = 0) {
    producer->attach([producer, this, input_channel]() {
      this->set_input_string(producer->get(), input_channel);
    });
  }

  void connect_from_int(ValueProducer<int>* producer,
                        uint8_t input_channel = 0) {
    producer->attach([producer, this, input_channel]() {
      this->set_input_int(producer->get(), input_channel);
    });
  }

 protected:
  String input_string_[8];
  int input_int_[8];

  std::function<void(String, int)> function;
};

}  // namespace sensesp

#endif