#include <Arduino.h>

#include "sensesp/sensors/analog_input.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/transforms/analogvoltage.h"
#include "sensesp/transforms/curveinterpolator.h"
#include "sensesp/transforms/linear.h"
#include "sensesp/transforms/voltagedivider.h"
#include "sensesp_app.h"
#include "sensesp_app_builder.h"

using namespace sensesp;

/*
  Illustrates a custom transform that takes a resistance value in ohms and
  returns the estimated temperature in Kelvin. Note that you
  will never instantiate CurveInterpolator in your own SensESP project - you
  will always need to create a descendant class of it, like
  TemperatureInterpreter in this example. The constructor needs to do only two
  things: call clearSamples(), then call addSample() at least twice. (This class
  can't function without at least two samples.) You can call addSample() as many
  times as you like, and the more samples you have, the more accurately this
  transform will emulate the analog sensor. Note, however, that since all
  transforms inherit from class Configurable, you can make its data configurable
  in the Config UI, and that means not only that you can edit (at runtime)
  whatever samples you add in the constructor, but also that you can add more
  samples at runtime.

  One of your samples should be at the lowest value for the input,and one should
  be at the highest value, so the input to CurveInterpolator will always be
  between two values.

  This example is a bit complex because of the need for the VoltageDivider2
  transform, but that also makes it an excellent "advanced" example.
*/

class TemperatureInterpreter : public CurveInterpolator {
 public:
  TemperatureInterpreter(String config_path = "")
      : CurveInterpolator(NULL, config_path) {
    // Populate a lookup table tp translate the ohm values returned by
    // our temperature sender to degrees Kelvin
    clear_samples();
    // addSample(CurveInterpolator::Sample(knownOhmValue, knownKelvin));
    add_sample(CurveInterpolator::Sample(0, 418.9));
    add_sample(CurveInterpolator::Sample(5, 414.71));
    add_sample(CurveInterpolator::Sample(36, 388.71));
    add_sample(CurveInterpolator::Sample(56, 371.93));
    add_sample(CurveInterpolator::Sample(59, 366.48));
    add_sample(CurveInterpolator::Sample(81, 355.37));
    add_sample(CurveInterpolator::Sample(112, 344.26));
    add_sample(CurveInterpolator::Sample(240, 322.04));
    add_sample(CurveInterpolator::Sample(550, 255.37));
    add_sample(CurveInterpolator::Sample(10000, 237.6));
  }
};

// SensESP builds upon the ReactESP framework. Every ReactESP application
// defines an "app" object.
reactesp::ReactESP app;

void setup() {
  SetupLogging();

  // Create the global SensESPApp() object by first creating a
  // SensESPAppBuilder object, then setting some hard-coded
  // program attributes, and then calling get_app().

  // Create a builder object
  SensESPAppBuilder builder;

  sensesp_app = builder.set_hostname("your device name")
                    ->set_sk_server("192.168.1.5", 80)
                    ->get_app();

  // The "Signal K path" identifies the output of this sensor to the Signal K
  // network.
  // To find valid Signal K Paths that fits your need you look at this link:
  // https://signalk.org/specification/1.4.0/doc/vesselsBranch.html
  const char* sk_path = "electrical.alternators.12V.temperature";

  // If you are creating a new Signal K path that does not
  // already exist in the specification, it is best to
  // define "metadata" that describes your new value. This
  // metadata will be reported to the server upon the first
  // time your sensor reports its value(s) to the server
  SKMetadata* metadata = new SKMetadata();
  metadata->description_ = "Alternator Temperature";
  metadata->display_name_ = "Alternator Temperature";
  metadata->short_name_ = "Alt Temp";
  metadata->units_ = "K";

  /*
  Connecting a physical temperature sender to the MCU involves using a "voltage
  divider" circuit. A resistor (R1) is connected to the voltage output of the
  board (3.3v or 5v, specified in Vin) to an intersection of two wires. One wire
  connects to the AnalogInput pin of the MCU, and the other wire connects to the
  sending unit doing the measurement. The sender is itself a variable resistor
  that is then connected to ground, forming the "R2" of a voltage divider
  circuit. The transform "VoltageDividerR2" takes a known voltage in (3.3 in
  this example), a known value for resistor R1 (51 ohms in this example), and
  based on the voltage read from AnalogInput, computes the resistance of the
  sender (R2), in ohms.

  One important point: the value of the output from the AnalogIn pin is not in
  volts, as implied above. It's a value in the range 0 - 1023, and it represents
  the voltage in the range 0.0V - 3.3V. But we need to send an actual voltage to
  VoltageDivider2(), which means we need to first convert the value of
  AnalogInput to volts, which is done with AnalogVoltage().

  Although complex, this example perfectly illustrates the normal "chain" for
  getting a value from a sensor all the way to the Signal K server:
  - A real-world sensor sends its output to an input pin on the microcontroller
  - The value read from that pin gives us our actual input to the program. In
  this example, that's an AnalogInput().
  - That input is then sent through one or more transforms so that the end
  result is the value we want to send to Signal K. In this example, the
  transforms are VoltageDivider2(), then
  TemperatureInterpreter(), then Linear().
  - Finally, the value we want is sent to the final "consumer", in this case
  SKOutputNumber().

  Notice the Linear() transport in the chain below. It's there in case you need
  to calibrate the final output of your sensor, after it's gone through all the
  other transforms. For example, if you know your temperature sender always
  reports a temp that's about 5 degrees Fahrenheit too cold, you can calibrate
  its output here. A difference of 5 degrees F is a difference of 2.78 degrees
  Kelvin, and all temps are sent to Signal K in Kelvin. So the Linear() below
  would be Linear(1.0, 2.78, "gen/temp/calibrate") - 1.0 is the multiplier (so
  it does nothing), and 2.78 is the offset, so it's added to the output of
  TemperatureInterpreter() just before the value is sent to Signal K.
  ("gen/temp/calibrate" tells the Config UI how to display it.)
  */

  // Voltage sent into the voltage divider circuit that includes the analog
  // sender
  const float volt_div_v_in = 3.3;
  // The resistance, in ohms, of the fixed resistor (R1) in the voltage divider
  // circuit
  const float R1 = 51.0;

  // An AnalogInput gets the value from the microcontroller's AnalogIn pin,
  // which is a value from 0 to 1023 and then scales it to the max pin voltage
  // (max_analog_in_voltage), giving the pin voltage.
  const float max_analog_in_voltage = 3.3;
  // ESP32 has many pins that can be used for AnalogIn, and they're
  // expressed here as the XX in GPIOXX.
  auto* analog_input = new AnalogInput(
      36, 1000, "/12V_alternator/temp/analog_in/", max_analog_in_voltage);

  /* Translating the number returned by AnalogInput into a temperature,
     and sending it to Signal K, requires several transforms. Wire them
     up in sequence:
     - convert voltage into ohms with VoltageDividerR2()
     - find the Kelvin value for the given ohms value with
     TemperatureInterpreter()
     - use Linear() in case you want to calibrate the output at runtime
     - send calibrated Kelvin value to Signal K with SKOutputNumber()
  */
  analog_input
      ->connect_to(new VoltageDividerR2(R1, volt_div_v_in,
                                        "/12V_alternator/temp/sender"))
      ->connect_to(new TemperatureInterpreter("/12V_alternator/temp/curve"))
      ->connect_to(new Linear(1.0, 0.0, "/12V_alternator/temp/calibrate"))
      ->connect_to(
          new SKOutputFloat(sk_path, "/12V_alternator/temp/sk", metadata));
}

// The loop function is called in an endless loop during program execution.
// It simply calls `app.tick()` which will then execute all reactions as needed.
void loop() { app.tick(); }
