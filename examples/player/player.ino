/**
 * @file player.ino
 * @author Phil Schatzmann
 * @brief Plays the included demo SID files to the defined output (e.g. on a regurlar ESP32)
 * Dependencies:
 * - https://github.com/pschatzmann/SIDPlayer
 * - https://github.com/pschatzmann/arduino-audio-tools
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "SIDPlayer.h"

SIDAudioSource source(DemoSongs, DemoSongsCount);
SIDSizeSource sidSize(source);
I2SStream out; // or AnalogAudioStream or PWMStream etc
AudioActions actions;
SIDPlayer sid(source, out, sidSize);

void previous(bool, int, void*) { sid.previous(); }
void next(bool, int, void*) { sid.next(); }
void volumeUp(bool, int, void*) { sid.setVolume(sid.volume()+0.2); }
void volumeDown(bool, int, void*) { sid.setVolume(sid.volume()-0.2);  }

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // setup button pins
  actions.add(12, previous);
  actions.add(13, next);
  actions.add(14, volumeUp);
  actions.add(15, volumeDown);

  auto cfg = out.defaultConfig(TX_MODE);
  out.begin(cfg);

  // move to next song after playing for 5 minuts
  sid.setTimeout(60*5);
  sid.begin();

}

void loop() {
  sid.copy();
}
