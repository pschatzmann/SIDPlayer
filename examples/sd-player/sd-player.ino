#include "SIDPlayer.h"
#include "AudioLibs/AudioSourceSD.h"
#include "AudioLibs/AudioKit.h"

const char *startFilePath="/";
const char* ext="sid";
AudioSourceSD source(startFilePath, ext);
SizeSourceFile fileSize(source);
AudioKitStream kit;
SIDPlayer sid(source, kit, fileSize);

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  // open in write mode
  auto cfg = kit.defaultConfig(TX_MODE);
  kit.begin(cfg);
  sid.begin();
}

void loop() {
  sid.copy();
}
