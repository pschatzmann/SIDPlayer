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
};

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
    return true;
  }

  void end() { libcsid_free(); }

  /// @brief  Loads the tune to be played
  /// @param tunedata 
  /// @param tunedatalen 
  /// @param subtune 
  void loadTune(const unsigned char *tunedata, int tunedatalen,
                          int subtune=0) {
    libcsid_load((unsigned char *)tunedata, tunedatalen, subtune);
    printf("SID Title: %s\n", libcsid_gettitle());
    printf("SID Author: %s\n", libcsid_getauthor());
    printf("SID Info: %s\n", libcsid_getinfo());
  }

  /// fill the data with 2 channels
  size_t readBytes(uint8_t *buffer, size_t bytes) override {
    size_t result = 0;
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

protected:
  SIDStreamConfig cfg;

  /// Provides a single sample
  int16_t readSample() {
    unsigned short result;

    libcsid_render(&result, 1);

    return ((int16_t)result);
  }
};

}