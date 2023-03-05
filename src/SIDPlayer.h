#pragma once
#include "AudioTools.h"
#include "SIDAudioSource.h"
#include "SIDStream.h"
#include "SIDConfig.h"

namespace audio_tools {

/**
 * @brief SID player which is based on the AudioPlayer from the AudioTools
 * project and the SidPlayer provided by Gunnar Larsen. 
 * The SID emulation is based on cSID light by Hermit (Mihaly Horvath), (Year 2017).
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SIDPlayer : public AudioBaseInfoDependent {
public:
  SIDPlayer(AudioSource &source, AudioPrint &output, SizeSource &sizeSource) {
    static CodecNOP nop;
    p_size_source = &sizeSource;
    player.setAudioSource(source);
    player.setOutput(output);
    p_info = &output;
    p_out = &output;
  }
  SIDPlayer(AudioSource &source, AudioStream &output, SizeSource &sizeSource) {
    static CodecNOP nop;
    p_size_source = &sizeSource;
    player.setAudioSource(source);
    player.setOutput(output);
    p_info = &output;
    p_out = &output;
  }

  /// (Re)Starts the playing of the music (from the beginning)
  bool begin(int index = 0, bool isActive = true) {
    TRACEI();
    // get update audio info from destination
    setAudioInfo(p_info->audioInfo());
    info.logInfo();
    player.setAudioInfo(info);
    // setup sid stream
    auto cfg = sid.defaultConfig();
    cfg.copyFrom(info);
    sid.begin(cfg);

    // make sure we have valid audio data
    assert(cfg.channels>0);
    assert(cfg.sample_rate>0);
    assert(cfg.bits_per_sample==16);

    // setup player
    player.setAutoNext(false);
    bool result = player.begin(index, isActive);

    // // we can just copy the audio from the sid stream
    // StreamCopy *p_copy = &player.getStreamCopy();
    // p_copy->begin(*getOutput(), sid);
    // player.setActive(true);
    return result;
  }
  /// Ends the processing
  void end() {
    player.end();
    sid.end();
  }

  /// Updates the audio info in the related objects
  void setAudioInfo(AudioBaseInfo info) override {
    TRACED();
    this->info = info;
  };

  AudioBaseInfo audioInfo() override { return info; }

  /// starts / resumes the playing of a matching song
  void play() { player.play(); }

  /// halts the playing
  void stop() { player.stop(); }

  /// moves to previous file
  bool previous(int offset = 1) {
    state = Initial;
    return player.previous(offset);
  }

  /// moves to next file or nth next file when indicating an offset. Negative
  /// values are supported to move back.
  bool next(int offset = 1) {
    state = Initial;
    return player.next(offset);
  }

  /// moves to selected file
  bool setIndex(int idx) {
    state = Initial;
    return player.setIndex(idx);
  }

  /// moves to selected file
  bool setPath(const char *path) {
    state = Initial;
    return player.setPath(path);
  }

  /// Provides the actual stream (=e.g.file)
  Stream *getStream() { return player.getStream(); }

  /// determines if the player is active
  bool isActive() { return player.isActive(); }

  /// determines if the player is active
  operator bool() { return isActive(); }

  /// The same like start() / stop()
  void setActive(bool isActive) { player.setActive(isActive); }

  /// sets the volume - values need to be between 0.0 and 1.0
  void setVolume(float volume) { player.setVolume(volume); }

  /// Determines the actual volume
  float volume() { return player.volume(); }

  /// If set to true the player writes 0 values instead of no data if the player
  /// is inactive
  void setSilenceOnInactive(bool active) {
    player.setSilenceOnInactive(active);
  }

  /// Checks if silence_on_inactive has been activated (default false)
  bool isSilenceOnInactive() { return player.isSilenceOnInactive(); }

  /// automatically move to next song after the timeout (in seconds) has
  /// expired;
  void setTimeout(size_t timeout) { timeout_sec = timeout; }

  /// Call this method in the loop.
  void copy() {
    TRACED();
    if (!isActive()) {
      if (isSilenceOnInactive()) {
        player.writeSilence(DEFAULT_BUFFER_SIZE);
      }
      return;
    }

    if (state == Initial) {
      loadSID();
      setIsPlayingTimeout();  
      setSIDAsInput();

    } else {
      // play sid file
      player.copy();
      // We can set a timeout for each song
      if (isPlayingTimedOut()) {
        moveNextOnEnd();
      }
    }
  }

protected:
  AudioPlayer player;
  SIDStream sid;
  Print *p_out;
  AudioBaseInfoDependent *p_info;
  AudioBaseInfo info;
  Vector<uint8_t> sid_data{0};
  SizeSource *p_size_source = nullptr;
  enum proces_state_enum { Initial, Playing };
  proces_state_enum state = Initial;
  size_t timeout_sec = 0;
  size_t playing_timout_ms = 0;

  Print *getOutput() { return player.getVolumeOutput(); }

  void loadSID() {
    TRACEI();
    Stream *p_stream = getStream();
    
    int size = p_size_source->size();
    sid_data.resize(size);
    p_stream->readBytes(sid_data.data(), size);
    LOGI("loadTune size: %d", size);
    if (size<MAX_FILE_SIZE){
      sid.loadTune(sid_data.data(), size, 0);
      state = Playing;
    } else {
      LOGE("Song is too big!");
      next(1);
    }
  }

  /// calculates when the song expires
  void setIsPlayingTimeout() {
    if (timeout_sec > 0) {
      playing_timout_ms = millis() + (timeout_sec * 1000);
    }
  }

  /// checks if the song has expired
  bool isPlayingTimedOut() {
    return (playing_timout_ms == 0) ? false : millis() > playing_timout_ms;
  }

  void moveNextOnEnd() {
    TRACEI();
    state = Initial;
    // move to next play
    next(1);
    state = Initial;
  }

  /// Make sure that we copy the data from the sid stream to the output
  void setSIDAsInput(){
    StreamCopy *p_copy = &player.getStreamCopy();
    p_copy->begin(*getOutput(), sid);
    player.setActive(true);
  }
};

} // namespace audio_tools
