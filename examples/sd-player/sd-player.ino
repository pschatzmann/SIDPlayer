#include "SIDPlayer.h"
#include "AudioLibs/AudioSourceSD.h"
#include "AudioLibs/AudioKit.h"

SIDSource source("/", "sid");
SizeSourceFile size(source);
AudioKitStream kit;
SIDPlayer sid(source, kit, size);

void setup() {
  Serial.begin(115200);
  // open in write mode
  auto cfg = kit.defaultConfig(TX_MODE);
  kit.begin(cfg);
  sid.begin();
}

void loop() {
  sid.copy();
}
