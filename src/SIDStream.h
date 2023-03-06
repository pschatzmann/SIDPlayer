/**
 * @file SidPlayer.h
 * @author Gunnar Larsen
 * @brief A simple abstraction to underlying sid/6502 logic
 * @version 0.1
 * @date 2023-02-23
 */
#pragma once
#include <stdint.h>
#include "sid-api/libcsid.h"
#include "AudioTools.h"

namespace audio_tools {

/**
 * @brief Configuration data for a SIDStream
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct SIDStreamConfig : public AudioBaseInfo {
  SIDStreamConfig() {
    sample_rate = 22050;
    channels = 2;
    bits_per_sample = 16;
  }
  int sid_model = 6581;
  const unsigned char *tune_data = nullptr;
  int tune_data_length = 0;
  int subtune = 0;
};

/**
 * @brief Title and Author from the SID
 * @author Gunnar Larsen
 * @copyright GPLv3
 *
 */
struct SIDMetadata {
  char title[32] = {0};
  char author[32] = {0};
  char sid_info[32] = {0};

  void logInfo() {
    LOGI("SID Title: %s", title);
    LOGI("SID Author: %s", author);
    LOGI("SID Info: %s", libcsid_getinfo());
  }
};

/**
 * @brief Provides SID audio data
 * @author Gunnar Larsen
 * @copyright GPLv3
 */
class SIDStream : public AudioStream {

public:
  /**
   * @brief Construct a new SIDStream object
   * To play a song call begin() and then setSID()
   */
  SIDStream() = default;
  /**
   * @brief Construct a new SIDStream object
   * To play a song just call begin()
   * @param tunedata
   * @param tunedatalen
   * @param subtune
   */
  SIDStream(const unsigned char *tunedata, int tunedatalen, int subtune = 0) {
    cfg.tune_data = tunedata;
    cfg.tune_data_length = tunedatalen;
    cfg.subtune = subtune;
  }

  /// Provides the default configuration
  SIDStreamConfig defaultConfig() {
    SIDStreamConfig c;
    return c;
  }

  bool begin(SIDStreamConfig config) {
    // save tune_data from constructor
    if (config.tune_data == nullptr && cfg.tune_data != nullptr) {
      config.tune_data = cfg.tune_data;
      config.tune_data_length = cfg.tune_data_length;
      config.subtune = cfg.subtune;
    }
    this->cfg = config;

    // allocate memory and setup processing
    libcsid_init(cfg.sample_rate, cfg.sid_model);

    // start to play song
    if (cfg.tune_data != nullptr) {
      setSID(cfg.tune_data, cfg.tune_data_length, cfg.subtune);
    } else {
      LOGI("call setSID()");
    }

    return true;
  }

  /// @brief  Stops processing and releases the memory
  void end() {
    active = false;
    libcsid_free();
  }

  /// @brief  Loads the tune to be played and starts generating audio
  /// @param tunedata
  /// @param tunedatalen
  /// @param subtune
  void setSID(const unsigned char *tunedata, int tunedatalen, int subtune = 0) {
    active = true;
    // update tune in cfg
    cfg.tune_data = tunedata;
    cfg.tune_data_length = tunedatalen;
    cfg.subtune = subtune;

    // load song
    libcsid_load((unsigned char *)tunedata, tunedatalen, subtune);

    // save metadata
    memcpy(meta.author, libcsid_getauthor(), sizeof(meta.author));
    memcpy(meta.title, libcsid_gettitle(), sizeof(meta.author));
    memcpy(meta.sid_info, libcsid_getinfo(), sizeof(meta.sid_info));
    meta.logInfo();
  }

  /// fill the data with 2 channels
  size_t readBytes(uint8_t *buffer, size_t bytes) override {
    if (!active)
      return 0;
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

  /// @brief Provide the metadata for the sid
  SIDMetadata getMetadata() { return meta; }

protected:
  SIDStreamConfig cfg;
  SIDMetadata meta;
  bool active = false;

  /// Provides a single sample
  int16_t readSample() {
    unsigned short result;

    libcsid_render(&result, 1);

    return ((int16_t)result);
  }
};

} // namespace audio_tools