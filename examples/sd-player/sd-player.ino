/**
 * @file player.ino
 * @author Phil Schatzmann
 * @brief Plays the SID files from a SD Drive on an AI Thinker AudioKit
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
#include "AudioLibs/AudioSourceSD.h"
// #include "AudioLibs/AudioKit.h"

// replace with your CS pin
#define CS PIN_AUDIO_KIT_SD_CARD_CS

const char *startFilePath="/SID";
const char* ext="sid";
AudioSourceSD source(startFilePath, ext, CS);
SizeSourceFile fileSize(source);
I2SStream out; // or replace with your Output Device e.g. AudioKitStream
SIDPlayer sid(source, out, fileSize);
AudioActions actions;

void previous(bool, int, void*) { TRACEI(); sid.previous(); }
void next(bool, int, void*) { TRACEI(); sid.next(); }
void volumeUp(bool, int, void*) { sid.setVolume(sid.volume()+0.2); }
void volumeDown(bool, int, void*) { sid.setVolume(sid.volume()-0.2);  }

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  // open in write mode
  auto cfg = out.defaultConfig(TX_MODE);
  cfg.channels = 1; // 1 or 2 working 
  cfg.sample_rate = 8000; // use whatever is supported by the audiokit
  out.begin(cfg);

  // timout 5 minutes
  sid.setTimeout(60*5);
  sid.begin();

  actions.add(PIN_KEY3, previous);
  actions.add(PIN_KEY4, next);
  actions.add(PIN_KEY5, volumeDown);
  actions.add(PIN_KEY6, volumeUp);

}

void loop() {
  sid.copy();
  actions.processActions();
}
