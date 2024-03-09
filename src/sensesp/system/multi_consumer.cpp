
#include "multi_consumer.h"

#include "sensesp.h"
#include "sensesp_app.h"

namespace sensesp {

MultiConsumer::MultiConsumer(
    std::function<void(String[8], int[8], float[8], bool[8])> func)
    : func{func} {}

void MultiConsumer::set_input_str(String input, uint8_t input_channel = 0) {
  this->input_str_[input_channel] = input;
  func(this->input_str_, this->input_int_, this->input_float_,
       this->input_bool_);
}

void MultiConsumer::set_input_int(int input, uint8_t input_channel = 0) {
  this->input_int_[input_channel] = input;
  func(this->input_str_, this->input_int_, this->input_float_,
       this->input_bool_);
}

void MultiConsumer::set_input_float(float input, uint8_t input_channel = 0) {
  this->input_int_[input_channel] = input;
  func(this->input_str_, this->input_int_, this->input_float_,
       this->input_bool_);
}

void MultiConsumer::set_input_bool(bool input, uint8_t input_channel = 0) {
  this->input_int_[input_channel] = input;
  func(this->input_str_, this->input_int_, this->input_float_,
       this->input_bool_);
}

void MultiConsumer::connect_from_str(ValueProducer<String>* producer,
                                     uint8_t input_channel = 0) {
  producer->attach([producer, this, input_channel]() {
    this->set_input_str(producer->get(), input_channel);
  });
}

void MultiConsumer::connect_from_int(ValueProducer<int>* producer,
                                     uint8_t input_channel = 0) {
  producer->attach([producer, this, input_channel]() {
    this->set_input_int(producer->get(), input_channel);
  });
}

}  // namespace sensesp