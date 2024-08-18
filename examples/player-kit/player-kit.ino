/**
 * @file player.ino
 * @author Phil Schatzmann
 * @brief Plays the included demo SID files on an AI Thinker AudioKit
 * Dependencies:
 * - https://github.com/pschatzmann/SIDPlayer
 * - https://github.com/pschatzmann/arduino-audio-tools
 * - https://github.com/pschatzmann/arduino-audiokit
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "SIDPlayer.h"
#include "AudioLibs/AudioBoardStream.h"

SIDAudioSource source(DemoSongs, DemoSongsCount);
SIDSizeSource sidSize(source);
AudioBoardStream kit(AudioKitEs8388V1);
SIDPlayer sid(source, kit, sidSize);

void previous(bool, int, void*) { TRACEI(); sid.previous(); }
void next(bool, int, void*) { TRACEI(); sid.next(); }
void volumeUp(bool, int, void*) { sid.setVolume(sid.volume()+0.2); }
void volumeDown(bool, int, void*) { sid.setVolume(sid.volume()-0.2);  }

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  auto cfg = kit.defaultConfig(TX_MODE);
  cfg.channels = 1; // 1 or 2 working 
  cfg.sample_rate = 8000; // use whatever is supported by the audiokit
  kit.begin(cfg);

  kit.addAction(PIN_KEY3, previous);
  kit.addAction(PIN_KEY4, next);
  kit.addAction(PIN_KEY5, volumeDown);
  kit.addAction(PIN_KEY6, volumeUp);

  // move to next song after playing for 5 minuts
  sid.setTimeout(60*5);
  sid.begin(random(source.size()));

}

void loop() {
  sid.copy();
  kit.processActions();
}
