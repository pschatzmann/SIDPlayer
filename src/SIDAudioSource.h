#pragma once
#include "SD.h"
#include <stddef.h>
#include <stdint.h>

#include "audio/arkanoid.h"
#include "audio/comic.h"
#include "audio/commando.h" // Good!
#include "audio/ik.h"       // Good!
#include "audio/mikie.h"    // Good!
#include "audio/ocean.h"    // Good!
#include "audio/rambo.h"
#include "audio/wizball.h"
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
    {(uint8_t *)Yie_Ar_Kung_Fu_sid, Yie_Ar_Kung_Fu_sid_len, 0},
    {(uint8_t *)Mikie_sid, Mikie_sid_len, 0},
    {(uint8_t *)Ocean_Loader_2_sid, Ocean_Loader_2_sid_len, 0},
    {(uint8_t *)International_Karate_sid, International_Karate_sid_len, 0},
    {(uint8_t *)Comic_Bakery_sid, Comic_Bakery_sid_len, 0},
    {(uint8_t *)Rambo_First_Blood_Part_II_sid,
     Rambo_First_Blood_Part_II_sid_len, 0},
    {(uint8_t *)Arkanoid_sid, Arkanoid_sid_len, 0},
    {(uint8_t *)Wizball_sid, Wizball_sid_len, 3},
};
const int DemoSongsCount = 9;

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
  virtual void begin() { index = 0; }

  /// Returns next audio stream
  virtual Stream *nextStream(int offset) {
    index += offset;
    if (index > songs.size() || index < 0) {
      index = 0;
    }
    return selectStream(index);
  }

  /// Returns audio stream at the indicated index (the index is zero based, so
  /// the first value is 0!)
  virtual Stream *selectStream(int index) {
    SidTitle title = songs[index];
    stream.setValue(title.data, title.size);
    title_size = title.size;
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
  MemoryStream stream;
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

/**
 * @brief Provides the size of the current file for a SIDAudioSource
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class SIDSizeSource : public SizeSource {
public:
  SIDSizeSource(SIDAudioSource &source) { p_source = &source; }
  size_t size() { return p_source->size(); }

protected:
  SIDAudioSource *p_source;
};

} // namespace audiotools