#pragma once
#include "AudioTools.h"
#include "SIDAudioSource.h"
#include "sid-api/SidPlayer.h"

namespace audio_tools {

/**
 * @brief SID player which is based on the AudioPlayer from the AudioTools
 * project
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SIDPlayer : public AudioBaseInfoDependent {
public:
  SIDPlayer(AudioSource &source, AudioPrint &output, SizeSource &sizeSource) {
    static CodecNOP nop;
    p_size_source = &sizeSource;
    setAudioInfo(output.audioInfo());
    p_player = new AudioPlayer(source, output, nop);
  }
  SIDPlayer(AudioSource &source, AudioStream &output, SizeSource &sizeSource) {
    static CodecNOP nop;
    p_size_source = &sizeSource;
    setAudioInfo(output.audioInfo());
    p_player = new AudioPlayer(source, output, nop);
  }

  virtual ~SIDPlayer() { delete p_player; }

  /// (Re)Starts the playing of the music (from the beginning)
  virtual bool begin(int index = 0, bool isActive = true) {
    return p_player->begin(index, isActive);
  }
  /// Ends the processing
  virtual void end() { p_player->end(); }

  /// (Re)defines the audio source
  void setAudioSource(AudioSource &source) { p_player->setAudioSource(source); }

  /// (Re)defines the output
  void setOutput(Print &output) { p_player->setOutput(output); }

  /// Updates the audio info in the related objects
  virtual void setAudioInfo(AudioBaseInfo info) override {
    p_player->setAudioInfo(info);
  };

  virtual AudioBaseInfo audioInfo() override { return p_player->audioInfo(); }

  /// starts / resumes the playing of a matching song
  virtual void play() { p_player->play(); }

  /// halts the playing
  virtual void stop() { p_player->stop(); }

  /// moves to next file or nth next file when indicating an offset. Negative
  /// values are supported to move back.
  virtual bool next(int offset = 1) { return p_player->next(offset); }

  /// moves to selected file
  virtual bool setIndex(int idx) { return p_player->setIndex(idx); }

  /// moves to selected file
  virtual bool setPath(const char *path) { return p_player->setPath(path); }

  /// moves to previous file
  virtual bool previous(int offset = 1) { return p_player->previous(offset); }

  /// Provides the actual stream (=e.g.file)
  virtual Stream *getStream() { return p_player->getStream(); }

  /// determines if the player is active
  virtual bool isActive() { return p_player->isActive(); }

  /// determines if the player is active
  operator bool() { return isActive(); }

  /// The same like start() / stop()
  virtual void setActive(bool isActive) { p_player->setActive(isActive); }

  /// sets the volume - values need to be between 0.0 and 1.0
  virtual void setVolume(float volume) { p_player->setVolume(volume); }

  /// Determines the actual volume
  virtual float volume() { return p_player->volume(); }

  /// If set to true the player writes 0 values instead of no data if the player
  /// is inactive
  void setSilenceOnInactive(bool active) {
    p_player->setSilenceOnInactive(active);
  }

  /// Checks if silence_on_inactive has been activated (default false)
  bool isSilenceOnInactive() { return p_player->isSilenceOnInactive(); }

  /// Call this method in the loop.
  virtual void copy() {

    if (!isActive()) {
      if (isSilenceOnInactive()) {
        getOutput()->writeSilence(DEFAULT_BUFFER_SIZE);
      }
      return;
    }

    if (state == Initial) {
      // load new SID
      Stream *p_stream = getStream();
      int size = p_size_source->size();
      sid_data.resize(size);
      p_stream->readBytes(sid_data.data(), size);

      sid.setSampleRate(audioInfo().sample_rate);
      sid.init();
      sid.loadTune(sid_data.data(), sid_data.size(), 0);
      state == Playing;
    } else {
      // play SID
      uint8_t buffer[DEFAULT_BUFFER_SIZE];
      size_t l = sid.read(buffer, DEFAULT_BUFFER_SIZE);
      getOutput()->write(buffer, l);
      if (l == 0) {
        state == Initial;
        if (!next(1)) {
          // restart
          setIndex(0);
        }
      }
    }
  }

protected:
  AudioPlayer *p_player = nullptr;
  Vector<uint8_t> sid_data{0};
  SidPlayer sid;
  SizeSource *p_size_source = nullptr;
  enum proces_state_enum { Initial, Playing };
  proces_state_enum state = Initial;

  AudioPrint *getOutput() {
    return (AudioPrint *)p_player->getStreamCopy().getTo();
  }
};

} // namespace audiotools
