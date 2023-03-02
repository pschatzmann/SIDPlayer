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
#include <stdint.h>

class SidPlayer {

    public:
        // the scale defines the max value which is generated
        SidPlayer() {};

        void setSampleRate(uint16_t sr);
        void setChannels(uint8_t ch);
        void setSidModel(int sid_model);
        void begin();
        void end();
        void loadTune(unsigned char *tunedata, int tunedatalen, int subtune);

        /// filles the data with n channels
        size_t read(uint8_t *buffer, size_t bytes);

    protected:
        int m_sample_rate = 22050;
        int m_sid_model = 6581;
        int m_channels = 2;

        /// Provides a single sample
        int16_t readSample();

};
