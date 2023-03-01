#pragma once
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

namespace audiotools {

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
    {(uint8_t *)Rambo_First_Blood_Part_II_sid, Rambo_First_Blood_Part_II_sid_len, 0},
    {(uint8_t *)Arkanoid_sid, Arkanoid_sid_len, 0},
    {(uint8_t *)Wizball_sid, Wizball_sid_len, 3},
};
const int DemoSongsCount = 9;

/**
 * @brief AudioSource with SID files which are stored in PROGMEM
 *
 */
class SIDAudioSource : AudioSource {
public:
  SIDAudioSource() = default;
  SIDAudioSource(SidTitle *songs, int count) {
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

  size_t actualFileSize() {
    return title_size;
  }

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

}