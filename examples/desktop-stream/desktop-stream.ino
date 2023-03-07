/**
 * @file stream.ino
 * @author Phil Schatzmann
 * @brief  Copies the Audio from Sid to an output device
 *
 * You can compile and run this sketch on the desktop e.g. with player-desktop | aplay on linux 
 * To build it:
 * go to this directory
 * mkdir build
 * cd build
 * cmake ..
 * make
 * and execute it with ./desktop-stream

 * @version 0.1
 * @date 2023-03-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "AudioTools.h"
#include "SIDStream.h"
#include "AudioLibs/StdioStream.h"
#include "audio/commando.h" 

uint16_t sample_rate=32000;
uint8_t channels = 2; 
StdioStream out; 
WAVEncoder enc;
EncodedAudioStream wav(&out, &enc); // final solution
SIDStream sid(music_Commando_sid, music_Commando_sid_len);
StreamCopy copier(wav, sid); 

// Arduino Setup
void setup(void) {  
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  // start output
  auto config = out.defaultConfig();
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.bits_per_sample = 16;
  out.begin(config);
  wav.begin(config);

  // Setup SID
  auto scfg = sid.defaultConfig();
  scfg.copyFrom(config);
  scfg.subtune = 2;
  sid.begin(scfg);
  //sid.setSID(music_Commando_sid, music_Commando_sid_len);
}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}

int main(){
  setup();
  while(true)loop();
}
