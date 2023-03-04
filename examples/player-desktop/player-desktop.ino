/**
 * @file player-desktop.ino
 * @author Phil Schatzmann
 * @brief Sends the PCM stream of the the included demo SID files to the the standard output. 
 * You can compile and run this sketch on the desktop e.g. with player-desktop | aplay on linux 
 * To build it:
 * go to this directory
 * mkdir build
 * cd build
 * cmake ..
 * make
 * and execute it with ./player-desktop
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "SIDPlayer.h"
#include "AudioLibs/StdioStream.h"

SIDAudioSource source(DemoSongs, DemoSongsCount);
SIDSizeSource sidSize(source);
StdioStream out; // or AnalogAudioStream or PWMStream etc
WAVEncoder enc;
EncodedAudioStream wav(&out, &enc);
SIDPlayer sid(source, wav, sidSize);

void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Error);

  auto cfg = out.defaultConfig();
  cfg.sample_rate = 44100;
  cfg.channels = 2;
  cfg.bits_per_sample = 16;
  out.begin(cfg);
  wav.begin(cfg);

  // move to next song after playing for 5 minuts
  sid.setTimeout(60*5);
  sid.begin();

}

void loop() {
  sid.copy();

}
