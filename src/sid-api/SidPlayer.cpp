/**
 * @file SidPlayer.h
 * @author Gunnar Larsen
 * @brief Some glue logic between the underlying sid/6502 logic and the ESP32 audio libs
 * @version 0.1
 * @date 2023-02-23
 *
 *
 */

#include "SidPlayer.h"
#include "libcsid.h"

void SidPlayer::setSampleRate(uint16_t sr)
{
  m_sample_rate = sr;
}

void SidPlayer::setSidModel(int sid_model)
{
  m_sid_model = sid_model;
}

void SidPlayer::init()
{
  libcsid_init(m_sample_rate, m_sid_model);
}

void SidPlayer::loadTune(unsigned char *tunedata, int tunedatalen, int subtune)
{
  libcsid_load(tunedata, tunedatalen, subtune);
  printf("SID Title: %s\n", libcsid_gettitle());
  printf("SID Author: %s\n", libcsid_getauthor());
  printf("SID Info: %s\n", libcsid_getinfo());
}

/// Provides a single sample
int16_t SidPlayer::readSample()
{
  unsigned short result;

  libcsid_render(&result, 1);

  return ((int16_t)result);
}

/// fill the data with 2 channels
size_t SidPlayer::read(uint8_t *buffer, size_t bytes)
{
  size_t result = 0;
  int16_t *ptr = (int16_t *)buffer;
  for (int j = 0; j < bytes / 4; j++)
  {
    int16_t sample = readSample();
    *ptr++ = sample;
    *ptr++ = sample;
    result += 4;
  }
  return result;
}
