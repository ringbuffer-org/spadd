#include "TimeDomain.hpp"

TimeDomain::TimeDomain(int bufferSize, float sampleRate)
    : m_bufferSize(bufferSize),
      m_sampleRate(sampleRate)
{
    Wavetable<float>::createWavetable(m_sineTable, WaveType::sin, m_tableSize);

    for (auto& buffer: bufferArray)
    {
        buffer.resize(m_bufferSize, 0.0);
    }
    
    for (auto& oscillator: oscillatorArray)
    {
        oscillator = new WavetableOscillator(m_sineTable);
        oscillator->setFrequency(440.0f, m_sampleRate);
        oscillator->buffer.resize(m_bufferSize, 0.0);
    }
}

void TimeDomain::process(const std::vector<Partial<float>>& partials) noexcept
{
    for (auto& buffer: bufferArray)
    {
        std::fill(buffer.begin(), buffer.end(), 0.0);
    }
    
    for (int partial = 0; partial < partials.size(); ++partial)
    {
        float currentAmplitude = partials[partial].amplitude;
        oscillatorArray[partial]->setFrequency(partials[partial].frequency, m_sampleRate);

        for (int sample = 0; sample < oscillatorArray[0]->buffer.size(); ++sample)
        {
            oscillatorArray[partial]->buffer[sample] = currentAmplitude * oscillatorArray[partial]->getNextSample();
        }

        for (int buffer = 0; buffer < m_channels; ++buffer)
        {
            float bFormat = partials[partial].bFormat[buffer];

            for (int sample = 0; sample < bufferArray[0].size(); ++sample)
            {
                bufferArray[buffer][sample] += bFormat * oscillatorArray[partial]->buffer[sample];
            }
        }
    }
}

void TimeDomain::setChannels(int channels)
{
    if (channels <= 16)
    {
        m_channels = channels;
    }
}