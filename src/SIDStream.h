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
struct SIDStreamConfig : public AudioInfo {
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
  uint8_t total_tunes;
  uint8_t default_tune;

  void logInfo() {
    LOGI("SID Title: %s", title);
    LOGI("SID Author: %s", author);
    LOGI("SID Info: %s", sid_info);
    LOGI("SID Number of tunes: %d", total_tunes);
    LOGI("SID Default tune: %d", default_tune);
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
    TRACEI();
    if (config.bits_per_sample!=16){
      LOGE("bits_per_sample %d: must be 16", config.bits_per_sample);
      return false;
    }
    // save tune_data from constructor
    if (config.tune_data == nullptr && cfg.tune_data != nullptr) {
      config.tune_data = cfg.tune_data;
      config.tune_data_length = cfg.tune_data_length;
    }
    // Use subtune from constructor if not reqeusted by begin
    if (config.subtune==0 && cfg.subtune!=0){
      config.subtune = cfg.subtune;
    }
    this->cfg = config;
    this->frame_size = sizeof(int16_t) * cfg.channels;

    // allocate memory and setup processing
    libcsid_init(cfg.sample_rate, cfg.sid_model);

    // start to play default song
    if (cfg.tune_data != nullptr) {
      setSID(cfg.tune_data, cfg.tune_data_length, cfg.subtune);
    } else {
      LOGI("call setSID()");
    }

    return true;
  }

  /// @brief  Stops processing and releases the memory
  void end() override {
    TRACEI();
    active = false;
    libcsid_free();
  }

  /// @brief  Loads the tune to be played and starts generating audio
  /// @param tunedata
  /// @param tunedatalen
  /// @param subtune
  void setSID(const unsigned char *tunedata, int tunedatalen, int subtune = 0) {
    LOGI("setSID len %d, subtune: %d", tunedatalen, subtune);
    active = true;
    // update tune in cfg
    cfg.tune_data = tunedata;
    cfg.tune_data_length = tunedatalen;

    // load song
    libcsid_load((unsigned char *)tunedata, tunedatalen);

    // save metadata
    memcpy(meta.author, libcsid_getauthor(), sizeof(meta.author));
    memcpy(meta.title, libcsid_gettitle(), sizeof(meta.author));
    memcpy(meta.sid_info, libcsid_getinfo(), sizeof(meta.sid_info));
    meta.total_tunes = libcsid_get_total_tunes_number();
    meta.default_tune = libcsid_get_default_tune_number();
    setTune(subtune);
  }

  void setTune(int subtune = 0) {
    if (!active)
      return;
    LOGI("setTune: %d", subtune);
    cfg.subtune = subtune;
    meta.logInfo();
    libcsid_play(cfg.subtune);
  }

  /// fill the data with 2 channels
  size_t readBytes(uint8_t *buffer, size_t bytes) override {
    // return when not active
    if (!active)
      return 0;

    // check for acceptable size
    if (bytes<frame_size){
      LOGE("readBytes: %d - too small", bytes);
      return 0;
    }

    // request is valid
    LOGD("readBytes %d", bytes);
    size_t result = 0;
    int16_t *ptr = (int16_t *)buffer;
    int frames = bytes / frame_size;
    for (int j = 0; j < frames; j++) {
      int16_t sample = readSample();
      for (int i = 0; i < cfg.channels; i++) {
        *ptr++ = sample;
        result += 2;
      }
    }

    updateTimeOfLastSound((int16_t*)buffer, bytes/2);
    return result;
  }

  /// Provide the metadata for the sid
  SIDMetadata getMetadata() { return meta; }
  SIDStreamConfig getStreamConfigdata() { return cfg; }

  /// Detects if we still produce any sound: Some submodule play endlessly, others only for a short period of time
  /// This method can detect the ones that have ended and produce 0 as output. Warning: you need to call readBytes
  /// while using this method.
  bool isActive(int timeoutMs=2000) {
    return active && (time_of_last_sound > millis() - timeoutMs);
  }

protected:
  SIDStreamConfig cfg;
  SIDMetadata meta;
  bool active = false;
  uint8_t frame_size;
  uint64_t time_of_last_sound = 0;

  /// Provides a single sample
  int16_t readSample() {
    unsigned short result;

    libcsid_render(&result, 1);

    return ((int16_t)result);
  }

  void updateTimeOfLastSound(int16_t *data, int len){
    for (int j=0;j<len;j++){
      if (data[j]!=0){
        time_of_last_sound = millis();
        break;
      }
    }
  }

};

} // namespace audio_tools