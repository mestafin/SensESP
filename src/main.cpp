

#include "sensesp_app_builder.h"

using namespace sensesp;

reactesp::ReactESP app;

void setup() {
  // start processing
  sensesp_app->start();
}

void loop() { app.tick(); }