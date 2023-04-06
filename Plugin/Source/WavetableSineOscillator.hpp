// Some parts of the code below belong to the JUCE Wavetable synthesizer tutorial (WavetableSynthTutorial_03.h).
// The code has been modified by Hilko Tondock. Regarding original parts of the code, see below.

/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include <array>
#include <vector>

class WavetableOscillator
{
public:
    WavetableOscillator(const std::vector<float>& wavetableToUse)//, double sampleRate)
        : wavetable(wavetableToUse),
          tableSize(wavetable.size() - 1)
    {}

    std::array<std::vector<float>, 16> bufferArray;

    std::vector<float> buffer;
    
    void setFrequency(float frequency, float sampleRate)
    {
        auto tableSizeOverSampleRate = (float) tableSize / sampleRate;

        tableDelta = frequency * tableSizeOverSampleRate;
    }

    inline float getNextSample() noexcept
    {
        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float) index0;

        auto value0 = wavetable[index0];
        auto value1 = wavetable[index1];

        auto currentSample = value0 + frac * (value1 - value0);

        if ((currentIndex += tableDelta) > (float) tableSize)
            currentIndex -= (float) tableSize;

        return currentSample;
    }

private:
    const std::vector<float>& wavetable;

    const int tableSize;

    float currentIndex = 0.0f; 

    float tableDelta = 0.0f;

    std::array<double, 16> m_BFormat;
};