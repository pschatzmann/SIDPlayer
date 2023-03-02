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

const char *startFilePath="/";
const char* ext="sid";
AudioSourceSD source(startFilePath, ext);
SizeSourceFile fileSize(source);
I2SStream out; // or replace with your Output Device e.g. AudioKitStream
SIDPlayer sid(source, out, fileSize);

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  // open in write mode
  auto cfg = out.defaultConfig(TX_MODE);
  out.begin(cfg);
  sid.begin();
}

void loop() {
  sid.copy();
}
