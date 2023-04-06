// Some parts of the code below belong to the JUCE Wavetable synthesizer tutorial (WavetableSynthTutorial_03.h).
// The code has been modified by Hilko Tondock. Regarding original parts of the code, see below.
// One adjustment has been made: 
//                auto angleDelta = 2.0 * M_PI / (double) (tableSize - 1);
// Correction:
//                auto angleDelta = 2.0 * M_PI / (double) (tableSize);

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

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <vector>
#include <cmath>

enum class WaveType
{
    sin = 1,
    cos,
    saw,
    sqr
};

template <typename T>
class Wavetable
{
public:
    Wavetable(WaveType type, const int size)
        : m_type(type),
          m_tableSize(size)
    {
        createWavetable(m_tableData, m_type, m_tableSize);
    }
    
    static void createWavetable(std::vector<T>& tableData, WaveType type, const int tableSize)
    {
        switch (type)
        { 
            case WaveType::sin:
            {
                tableData.resize(tableSize + 1, 0.0);

                auto angleDelta = 2.0 * M_PI / (double) (tableSize);
                auto currentAngle = 0.0;

                for (unsigned int i = 0; i < tableSize; ++i)
                {
                    auto sample = sin(currentAngle);
                    tableData[i] = (float) sample;
                    currentAngle += angleDelta;
                }
                
                tableData[tableSize] = tableData[0];
            }
            break;
            
            case WaveType::cos:
            {
                tableData.resize(tableSize + 1, 0.0);

                auto angleDelta = 2.0 * M_PI / (double) (tableSize);
                auto currentAngle = 0.0;

                for (unsigned int i = 0; i < tableSize; ++i)
                {
                    auto sample = cos(currentAngle);
                    tableData[i] = (float) sample;
                    currentAngle += angleDelta;
                }
                
                tableData[tableSize] = tableData[0];
            }
            break;

            case WaveType::saw:
            {
                tableData.resize(tableSize + 1, 0.0);

                for (unsigned int i = 0; i < tableSize; ++i)
                {
                    if (i < tableSize * 0.5 + 1)
                    {
                        auto sample = i / (tableSize * 0.5);
                        tableData[i] = (float) sample;
                    }
                    else
                    {
                        auto sample = (tableSize - i) / (tableSize * 0.5 - 1);
                        tableData[i] = (float) -1.0 * sample; 
                    }
                }
                
                tableData[tableSize] = tableData[0];
            }

            case WaveType::sqr:
            {
                tableData.resize(tableSize + 1, 0.0);

                for (unsigned int i = 0; i < tableSize; ++i)
                {
                    if (i < tableSize * 0.5)
                        tableData[i] = 1.0;
                    else
                        tableData[i] = -1.0; 
                }
                
                tableData[tableSize] = tableData[0];
            }
  
            default:
            {
                tableData.resize(tableSize + 1, 1.0);
                tableData[tableSize] = tableData[0];
            }
            break;
        }
    }
         
    float getTableData(int index)
    {
        return m_tableData[index];
    }

private:
    WaveType m_type;

    const int m_tableSize;

    std::vector<T> m_tableData;
};

template class Wavetable<float>;
template class Wavetable<double>;