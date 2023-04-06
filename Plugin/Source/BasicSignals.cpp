#include "BasicSignals.hpp"

BasicSignals::BasicSignals (SignalType type,
                            float amplitude,
                            float frequencyStart,
                            float frequencyEnd,
                            float phase)
        : m_type(type),
          m_amplitude(amplitude),
          m_frequencyStart(frequencyStart),
          m_frequencyEnd(frequencyEnd),
          m_phase(phase),
          m_numberOfPartials(1),
          m_azimuthAngle(0.0),
          m_elevationAngle(0.0),
          m_distance(1.0),
          m_normalisation(Normalisation::SN3D)
{
    m_partials.reserve(10000);

    createSignal(type);

    Wavetable<float>::createWavetable(m_sinTable, WaveType::sin, m_tableSize); 
    Wavetable<float>::createWavetable(m_cosTable, WaveType::cos, m_tableSize); 
    Wavetable<float>::createWavetable(m_sawTable, WaveType::saw, m_tableSize); 
    Wavetable<float>::createWavetable(m_sqrTable, WaveType::sqr, m_tableSize); 
}

void BasicSignals::createSignal(SignalType type) noexcept
{
    switch (type)
    {
        case SignalType::sine:
        {
            m_currentPartial.amplitude = m_amplitude;
            m_currentPartial.frequency = m_frequencyStart;
            m_partials.push_back(m_currentPartial);
        }
        break;

        case SignalType::triangle:
        {
            float currentFrequency = m_frequencyStart;
            float currentAmplitude = m_amplitude;
            int a = 1;

            while (currentFrequency < 20000.0f)
            {
                m_currentPartial.amplitude = currentAmplitude;
                m_currentPartial.frequency = currentFrequency;
                m_partials.push_back(m_currentPartial); 
                
                a += 2;
                currentFrequency = currentFrequency + 2.0f * m_frequencyStart;
                currentAmplitude = 1.0f / powf (a, 2.0f) * m_amplitude;
            }
        }
        break;

        case SignalType::sawtooth:
        {
            float currentFrequency = m_frequencyStart;
            float currentAmplitude = m_amplitude;
            int a = 1;

            while (currentFrequency < 20000.0f) 
            {
                m_currentPartial.amplitude = currentAmplitude;
                m_currentPartial.frequency = currentFrequency;
                m_partials.push_back(m_currentPartial); 

                a += 1;
                currentFrequency = currentFrequency + m_frequencyStart;
                currentAmplitude = 1.0f / static_cast<float>(a) * m_amplitude;
            }
        }
        break;

        case SignalType::square:
        {
            float currentFrequency = m_frequencyStart;
            float currentAmplitude = m_amplitude;
            int a = 1;

            while (currentFrequency < 20000.0f) 
            {
                m_currentPartial.amplitude = currentAmplitude;
                m_currentPartial.frequency = currentFrequency;
                m_partials.push_back(m_currentPartial); 

                a += 2;
                currentFrequency = currentFrequency + 2.0f * m_frequencyStart;
                currentAmplitude = 1.0f / static_cast<float>(a) * m_amplitude;
            }
        }
        break;

        case SignalType::noise:
        {
            for (int i = 0; i < m_numberOfPartials; ++i)
            {
                auto a = m_randomAmplitude.nextFloat();
                auto f = m_randomFrequency.nextFloat();
                m_currentPartial.amplitude = juce::jmap(a, 0.001f, 0.02f);
                m_currentPartial.frequency = juce::jmap(f, 10.0f, 20000.0f);    
                m_partials.push_back(m_currentPartial); 
            }
        }
        break;
            
        default:
        {
            m_currentPartial.amplitude = m_amplitude;
            m_currentPartial.frequency = m_frequencyStart;
            m_partials.push_back(m_currentPartial); 
        }
        break;
    }
}

SignalType BasicSignals::getType()
{
    return m_type;
}

std::vector<Partial<float>>& BasicSignals::getPartials() noexcept
{
    return m_partials;
}

void BasicSignals::setSampleRate(float sampleRate) noexcept
{
    m_sampleRate = sampleRate;
}

float BasicSignals::getPhase()
{
    return m_phase;
}

void BasicSignals::setType (SignalType type)
{
    m_type = type;
}

void BasicSignals::setFrequency (float frequency) noexcept
{
    m_frequencyStart = frequency;
}

void BasicSignals::setAmplitude (float amplitude)
{
    m_amplitude = amplitude;
}

void BasicSignals::reset() noexcept
{
    m_partials.clear();
}

void BasicSignals::setNumberOfPartials(int numberOfPartials) noexcept
{
    m_numberOfPartials = numberOfPartials;
}

void BasicSignals::setSpatialParameters(float distance, float azimuthAngle, float elevationAngle) noexcept
{
    m_distance = distance;
    m_azimuthAngle = azimuthAngle;
    m_elevationAngle = elevationAngle;
    
    for (int i = 0; i < m_partials.size(); ++i)
        m_partials[i].distance = m_distance;
}

void BasicSignals::setAzimuthDisplacement(DisplacementFunction azimuthDisplacement, float width, float horizontalDispersion) noexcept
{
    float stepSize = 2.0 * M_PI * horizontalDispersion / static_cast<float>(m_partials.size());

    switch (azimuthDisplacement)
    {
        case DisplacementFunction::sin:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].azimuth = m_azimuthAngle + width * sin(i * stepSize);    

                azimuthWrapAround(m_partials[i].azimuth);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
            
        case DisplacementFunction::cos:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].azimuth = m_azimuthAngle + width * cos(i * stepSize);    

                azimuthWrapAround(m_partials[i].azimuth);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;

        case DisplacementFunction::saw:
        {
            float stepSizeTable = m_tableSize * horizontalDispersion / static_cast<float>(m_partials.size());
                                                          
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].azimuth = m_azimuthAngle + width * m_sawTable[static_cast<int>(i * stepSizeTable) % m_tableSize];    

                azimuthWrapAround(m_partials[i].azimuth);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
 
        case DisplacementFunction::sqr:
        {
            float stepSizeTable = m_tableSize * horizontalDispersion / static_cast<float>(m_partials.size());
            
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].azimuth = m_azimuthAngle + width * m_sqrTable[static_cast<int>(i * stepSizeTable) % m_tableSize];    

                azimuthWrapAround(m_partials[i].azimuth);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;

        default:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].azimuth = m_azimuthAngle;
                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
    }
}

void BasicSignals::setElevationDisplacement(DisplacementFunction elevationDisplacement, float height, float verticalDispersion) noexcept
{
    float stepSize = 2.0 * M_PI * verticalDispersion / static_cast<float>(m_partials.size());

    switch (elevationDisplacement)
    {
        case DisplacementFunction::sin:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].elevation = m_elevationAngle + height * sin(i * stepSize);

                elevationWrapAround(m_partials[i].elevation);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
            
        case DisplacementFunction::cos:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].elevation = m_elevationAngle + height * cos(i * stepSize);    

                elevationWrapAround(m_partials[i].elevation);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;

        case DisplacementFunction::saw:
        {
            float stepSizeTable = m_tableSize * verticalDispersion / static_cast<float>(m_partials.size());
                                                          
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].elevation = m_elevationAngle + height * m_sawTable[static_cast<int>(i * stepSizeTable) % m_tableSize];

                azimuthWrapAround(m_partials[i].elevation);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
 
        case DisplacementFunction::sqr:
        {
            float stepSizeTable = m_tableSize * verticalDispersion / static_cast<float>(m_partials.size());
            
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].elevation = m_elevationAngle + height * m_sqrTable[static_cast<int>(i * stepSizeTable) % m_tableSize];    

                azimuthWrapAround(m_partials[i].elevation);

                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;

        default:
        {
            for (int i = 0; i < m_partials.size(); ++i)
            {
                m_partials[i].elevation = m_elevationAngle;
                m_partials[i].setBFormat(m_normalisation);
            }
        }
        break;
    }
}

void BasicSignals::setBrightness(float dampingFactor) noexcept
{
    for (int i = 0; i < static_cast<int>(m_partials.size()); ++i)
        m_partials[i].amplitude = m_partials[i].amplitude * exp(-1.0 * static_cast<float>(i) / dampingFactor);
}

void BasicSignals::setNormalisation(Normalisation normalisation)
{
    m_normalisation = normalisation;
}

void BasicSignals::azimuthWrapAround(float& azimuth) noexcept
{
    if (azimuth > 2.0 * M_PI) 
        azimuth = azimuth - 2.0 * M_PI;
    else if (azimuth < 0.0)
        azimuth = 2.0 * M_PI + azimuth;
}

void BasicSignals::elevationWrapAround(float& elevation) noexcept
{
    if (elevation > 0.5 * M_PI) 
        elevation = 0.5 * M_PI;
    else if (elevation < 0.0)
        elevation = 0.0;
}
 
    