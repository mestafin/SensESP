#ifndef _multi_consumer_H_
#define _multi_consumer_H_

#include <ArduinoJson.h>
#include <stdint.h>

#include "sensesp.h"
#include "valueproducer.h"

namespace sensesp {

/**
 * @brief Provides an easy way of calling a lambda function based on
 * the output of up to 8 ValueProducers that are connected to the
 * input channels of the MultiConsumer
 *
 * The current value of each input is saved internally each time
 * the input changes and the lambda function is then executed
 * with the current values of the input channels.
 *
 * The ValueProducers can be a mix of String, int, float or bool producers.
 *
 * To use the MultiConsumer, you create a lamba function in
 * main.cpp and then pass it as the "function" parameter
 * to the MultiConsumer constructor. This is the only argument
 * of the MultiConsumer constructor
 *
 * @tparam func : user-defined lambda function that will be executed
 * each time one of the 8 inputs changes.
 *
 * The MultiConsumer is connected to the ValueProducers by calling
 * one of the following "connect_from" functions with a channel_no 0 - 7
 *
 *  ->connect_from_str(ValueProducerString, int channel_no)
 *  ->connect_from_int(ValueProducerInt, int channel_no)
 *  ->connect_from_float(ValueProducerFloat, int channel_no)
 *  ->connect_from_bool(ValueProducerBool, int channel_no)
 *
 * if the input value is available as a normal variable, one of the
 * following "set_value" functions can be called with a channel_no 0 - 7
 *
 *  ->set_value_str(String value, int channel_no)
 *  ->set_value_int(int value, int channel_no)
 *  ->set_value_float(float value, int channel_no)
 *  ->set_value_bool(bool value, int channel_no)
 *
 * Each channel can only be used for one value
 *
 * The lamba function must be defined with 4 arrays as arguments
 * in the following sequence:
 *
 * @tparam String input_str[8]
 * @tparam int input_int[8]
 * @tparam float input_str[8]
 * @tparam bool input_str[8]
 * 
 *
 * in the lamda function, the values of the 8 input channels can be accessed by
 * using the 4 arrays with the applicable channel number
 *
 * int i = input_int[2]   (variable i is set to integer value of channel 2)
 * float f = input_float[3] ((variable f is set to float value of channel 3)
 *
 * 
 * Example Use 1:
 * 
 * auto *mc = new MultiConsumer([capture](String input_str[8], int input_int[8], float input_float[8], bool input_bool[8])
 * {
 *    // do something with input channel values and capture variable
 * }
 * mc->connect_from_str(ValueProducerString,0);   (connect a String ValueProducer to channel 0)
 * mc->set_value_int(125,1);                      (set the value of channel 1 to the value 125)
 * 
 * Example Use 2:
 * 
 * auto ff = [capture](String input_str[8], int input_int[8], float input_float[8], bool input_bool[8])
 * {
 *    // do something with input channel values and capture variable
 * };
 * 
 * auto *mc = new MultiConsumer(ff);
 * mc->connect_from_str(ValueProducerString,0);   (connect a String ValueProducer to channel 0)
 * mc->set_value_int(125,1);                      (set the value of channel 1 to the value 125)
 * 
 * */

class MultiConsumer {
 public:
  MultiConsumer(std::function<void(String[8], int[8], float[8], bool[8])> func);

  void set_input_str(String input, uint8_t input_channel);
  void set_input_int(int input, uint8_t input_channel);
  void set_input_float(float input, uint8_t input_channel);
  void set_input_bool(bool input, uint8_t input_channel);

  void connect_from_str(ValueProducer<String>* producer, uint8_t input_channel);
  void connect_from_int(ValueProducer<int>* producer, uint8_t input_channel);
  void connect_from_float(ValueProducer<float>* producer, uint8_t input_channel);
  void connect_from_bool(ValueProducer<bool>* producer, uint8_t input_channel);

 protected:
  String input_str_[8] = {"", "", "", "", "", "", "", ""};
  int input_int_[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float input_float_[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  bool input_bool_[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  std::function<void(String[8], int[8], float[8], bool[8])> func;
};

}  // namespace sensesp

#endif
