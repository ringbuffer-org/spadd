// TODO: namespace

/**
 * \class BasicSignals
 *
 *
 * \brief The BasicSignals class generates the spectral and spatial information of classic waveforms.
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

#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <vector>
#include <iostream>
#include <shared_processing_code/shared_processing_code.h>

enum class SignalType
{
    sine = 1,
    triangle,
    sawtooth,
    square,
    noise
};

enum class DisplacementFunction
{
    sin = 1, 
    cos,
    saw,
    sqr
};

class BasicSignals
{
public:
    BasicSignals (SignalType type, 
                  float amplitude, 
                  float frequencyStart,
                  float frequencyEnd, 
                  float phase);

    void createSignal(SignalType type) noexcept;

    SignalType getType();

    std::vector<float> getAmplitudes();

    std::vector<float> getFrequencies();
    
    std::vector<Partial<float>>& getPartials() noexcept;

    void setSampleRate(float) noexcept;

    float getPhase();

    void setType (SignalType type);

    void setFrequency (float frequency) noexcept;

    void setAmplitude (float amplitude);
    
    void setNumberOfPartials(int numberOfPartials) noexcept;
    
    void reset() noexcept;
    
    void setSpatialParameters(float distance, float azimuthAngle, float elevationAngle) noexcept;
    
    void setAzimuthDisplacement(DisplacementFunction azimuthDisplacement, float width, float horizontalDispersion) noexcept;
    
    void setElevationDisplacement(DisplacementFunction elevationDisplacement, float height, float verticalDispersion) noexcept;

    void setBrightness(float dampingFactor) noexcept;
    
    void setNormalisation(Normalisation normalisation);

private:
    std::vector<Partial<float>> m_partials;

    Partial<float> m_currentPartial;
    
    float m_sampleRate;

    SignalType m_type;

    float m_amplitude;

    float m_frequencyStart;
    
    float m_frequencyEnd;

    float m_phase;
    
    int m_numberOfPartials;
    
    int getRandomFrequency();

    juce::Random m_randomFrequency;

    juce::Random m_randomAmplitude;
    
    float m_azimuthAngle;
    
    float m_elevationAngle;

    float m_distance;
    
    void setDistribution(DisplacementFunction function);
    
    juce::Random m_randomAzimuth;

    juce::Random m_randomElevation;
    
    void azimuthWrapAround(float& azimuth) noexcept;

    void elevationWrapAround(float& elevation) noexcept;
    
    Normalisation m_normalisation;

    const unsigned int m_tableSize = 4096;

    std::vector<float> m_sinTable;
    
    std::vector<float> m_cosTable;

    std::vector<float> m_sawTable;

    std::vector<float> m_sqrTable;

    WaveType m_sinType;

    WaveType m_cosType;

};