#pragma once
#ifdef ARDUINO
#include "SD.h"
#endif
#include <stddef.h>
#include <stdint.h>

#include "audio/comic.h"
#include "audio/commando.h" // Good!
#include "audio/ik.h"       // Good!
#include "audio/mikie.h"    // Good!
#include "audio/ocean.h"    // Good!
#include "audio/yie.h" // Good!

namespace audio_tools {

/**
 * @brief Information about an individual SID song title
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
struct SidTitle {
  uint8_t *data;
  size_t size;
  int no;
};

const SidTitle DemoSongs[] = {
    {(uint8_t *)music_Commando_sid, music_Commando_sid_len, 0},
    {(uint8_t *)Yie_Ar_Kung_Fu_sid, Yie_Ar_Kung_Fu_sid_len, 18},
    {(uint8_t *)Mikie_sid, Mikie_sid_len, 0},
    {(uint8_t *)Ocean_Loader_2_sid, Ocean_Loader_2_sid_len, 0},
    {(uint8_t *)International_Karate_sid, International_Karate_sid_len, 0},
    {(uint8_t *)Comic_Bakery_sid, Comic_Bakery_sid_len, 0}
};
const int DemoSongsCount = 6;

/**
 * @brief AudioSource with SID files which are stored in PROGMEM
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SIDAudioSource : public AudioSource {
public:
  SIDAudioSource() = default;
  SIDAudioSource(const SidTitle *songs, int count) {
    for (int j = 0; j < count; j++) {
      addSong(songs[j]);
    }
  }

  void addSong(SidTitle song) { songs.push_back(song); }

  /// Reset actual stream and move to root
  virtual void begin() { 
    index = 0; 
    stream.begin();
  }

  /// Returns next audio stream
  virtual Stream *nextStream(int offset) {
    index += offset;
    if (index >= songs.size()) {
      index = 0;
    }
    if (index < 0) {
      index = songs.size() - 1;
    }
    return selectStream(index);
  }

  /// Returns audio stream at the indicated index (the index is zero based, so
  /// the first value is 0!)
  virtual Stream *selectStream(int index) {
    LOGI("selectStream: %d", index);
    SidTitle& title = songs[index];
    stream.setValue(title.data, title.size, FLASH_RAM);
    title_size = title.size;
    LOGI("actualFileSize: %d", (int)title_size);
    return &stream;
  }

  size_t actualFileSize() { return title_size; }

  /// File size of actual file
  int size() { return songs.size(); }

  /// Clears all songs
  void clear() { songs.clear(); }

  /// Returns audio stream by path
  virtual Stream *selectStream(const char *path) { return nullptr; }

protected:
  int index = 0;
  Vector<SidTitle> songs{0};
  MemoryStream stream{nullptr,0,true, FLASH_RAM};
  size_t title_size = 0;
};

/**
 * @brief Logic to determine the Size of the Stream: File ?
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SizeSource {
public:
  virtual size_t size() = 0;
};

#ifdef ARDUINO

/**
 * @brief Provides the size if the audio source is using Files
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SizeSourceFile : public SizeSource {
public:
  SizeSourceFile(AudioSource &source) { p_source = &source; }
  size_t size() override {
    // get the current stream (which is actually a file)
    File *p_file = (File *)p_source->nextStream(0);
    return p_file->size();
  };

protected:
  AudioSource *p_source;
};

#endif

/**
 * @brief Provides the size of the current file for a SIDAudioSource
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SIDSizeSource : public SizeSource {
public:
  SIDSizeSource(SIDAudioSource &source) { p_source = &source; }
  size_t size() { return p_source->actualFileSize(); }

protected:
  SIDAudioSource *p_source;
};


} // namespace audiotools