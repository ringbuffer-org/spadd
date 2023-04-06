/**
 * \class TimeDomain
 *
 *
 * \brief The TimeDomain class implements a wavetable approach.
 *
 *
 * \author Hilko Tondock
 *
 * \version  0.1
 *
 * \date   2023/03/31
 *
 * Contact: h.tondock@campus.tu-berlin.de
 *
 */

#pragma once

#include <array>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <shared_processing_code/shared_processing_code.h>
#include "WavetableSineOscillator.hpp"
#include "BasicSignals.hpp"

class TimeDomain
{
public:
    TimeDomain(int bufferSize, float sampleRate);

    std::array<std::vector<float>, 16> bufferArray;

    std::array<WavetableOscillator*, 10000> oscillatorArray;

    void process(const std::vector<Partial<float>>& partials) noexcept; 
    
    void setChannels(int channels);
    
private:
    const unsigned int m_tableSize = 4096;

    WaveType m_waveType;

    std::vector<float> m_sineTable;

    int m_bufferSize;
    
    float m_sampleRate;
    
    int m_channels;
};
    