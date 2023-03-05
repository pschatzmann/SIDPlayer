/**
 * @file SidPlayer.h
 * @author Gunnar Larsen
 * @brief A simple abstraction to underlying sid/6502 logic
 * @version 0.1
 * @date 2023-02-23
 *
 *
 */
#pragma once
#include <Arduino.h>
#include <AudioTools.h>
#include <stdint.h>
#include "sid-api/libcsid.h"

namespace audio_tools {

class SIDStreamConfig : public AudioBaseInfo {
public:
  SIDStreamConfig() {
    sample_rate = 22050;
    channels = 2;
    bits_per_sample = 16;
  }
  int sid_model = 6581;
  unsigned char *psid_data;
  int psid_length;  
};

/**
 * @brief Details about PSID file
 * @author Gunnar Larsen
 * @copyright GPLv3
 */
typedef struct{
    char title[32];
    char author[32];
    char sid_info[32];

    uint8_t total_tunes;
    uint8_t default_tune;
} PSidInfo;

/**
 * @brief Provides SID audio data
 * @author Gunnar Larsen
 * @copyright GPLv3
 */
class SIDStream : public AudioStream {

public:

  SIDStream() = default;

  /// Provides the default configuration
  SIDStreamConfig defaultConfig() {
    SIDStreamConfig c;
    return c;
  }

  bool begin(SIDStreamConfig cfg) {
    this->cfg = cfg;
    libcsid_init(cfg.sample_rate, cfg.sid_model);
    libcsid_load(cfg.psid_data, cfg.psid_length);

    // populate song info
    memcpy(info.author, libcsid_getauthor(), sizeof(info.author));
    memcpy(info.title, libcsid_gettitle(), sizeof(info.author));
    memcpy(info.sid_info, libcsid_getinfo(), sizeof(info.sid_info));
    info.default_tune = libcsid_get_default_tune_number();
    info.total_tunes = libcsid_get_total_tunes_number();
    dumpDebugInfo();
    return true;
  }

  void play() {
    play(info.default_tune);
  }

  void play(uint8_t tune_to_play) {
    playing = true;
    libcsid_play(tune_to_play);    
  }

  void end() { playing = false; }

  bool isPlaying() {return playing;}
    
  void dumpDebugInfo() {
    char buffer[32];
    sprintf(buffer, (char*)info.title);
    printf("SID song title: %s\n", buffer);
    sprintf(buffer, (char*)info.author);
    printf("SID song author: %s\n", buffer);
    sprintf(buffer, (char*)info.sid_info);
    printf("SID song info: %s\n", buffer);
    printf("SID song number of tunes: %d\n", info.total_tunes);
    printf("SID song default tune: %d\n", info.default_tune);
  }

  /// fill the data with 2 channels
  size_t readBytes(uint8_t *buffer, size_t bytes) override {
    size_t result = 0;

    if (!playing)
      return result;

    int16_t *ptr = (int16_t *)buffer;
    int frames = bytes / sizeof(int16_t) / cfg.channels;
    for (int j = 0; j < frames; j++) {
      int16_t sample = readSample();
      for (int i = 0; i < cfg.channels; i++) {
        *ptr++ = sample;
        result += 2;
      }
    }
    return result;
  }

  PSidInfo info = {0};

protected:
  SIDStreamConfig cfg;
  bool playing = false;

  /// Provides a single sample
  int16_t readSample() {
    unsigned short result;

    libcsid_render(&result, 1);

    return ((int16_t)result);
  }
};

}