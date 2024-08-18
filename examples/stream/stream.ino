/**
 * @file stream.ino
 * @author Phil Schatzmann
 * @brief  Copies the Audio from Sid to an output device
 * @version 0.1
 * @date 2023-03-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "AudioTools.h"
#include "SIDStream.h"
#include "AudioLibs/AudioBoardStream.h"
#include "audio/commando.h" 

uint16_t sample_rate=32000;
uint8_t channels = 2; 
AudioBoardStream out(AudioKitEs8388V1);
SIDStream sid(music_Commando_sid, music_Commando_sid_len);
StreamCopy copier(out, sid); 

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Info);

  // start I2S
  Serial.println("starting I2S...");
  auto config = out.defaultConfig(TX_MODE);
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.bits_per_sample = 16;
  out.begin(config);

  // Setup SID
  auto scfg = sid.defaultConfig();
  scfg.copyFrom(config);
  sid.begin(scfg);
  //sid.setSID(music_Commando_sid, music_Commando_sid_len);

}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}
