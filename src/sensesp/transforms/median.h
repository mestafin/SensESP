#ifndef SENSESP_TRANSFORMS_MEDIAN_H_
#define SENSESP_TRANSFORMS_MEDIAN_H_

#include <vector>

#include "transform.h"

namespace sensesp {

/**
 * @brief Outputs the median value of sample_size inputs.
 *
 * Creates a sorted list of sample_size values and outputs the one
 * in the middle (i.e. element number 'sample_size / 2').
 *
 * @param sample_size Number of values you want to take the median of.
 *
 * @param config_path Path to configure this transform in the Config UI.
 */
class Median : public FloatTransform {
 public:
  Median(unsigned int sample_size = 10, const String& config_path = "");
  virtual void set(const float& input) override;
  virtual void get_configuration(JsonObject& root) override;
  virtual bool set_configuration(const JsonObject& config) override;
  virtual String get_config_schema() override;

 private:
  std::vector<float> buf_{};
  unsigned int sample_size_;
};

}  // namespace sensesp
#endif
