#include "IFFT.hpp"

/* TODO
 *  => compute oversampling factor based on K and window type 
 *  => IFFT size based on buffer size
 * 
 * 
 */

IFFT::IFFT (int ifftSize,
            WindowType windowType,
            int oversamplingFactor,
            int K)
    : m_WindowType(windowType),
      // TODO: samplerate in ctor argument   
      m_T(1.0f / 44100.0f),
      m_channels(AC),  
      m_frameSize(ifftSize),
      m_halfFrameSize(ifftSize / 2),
      m_hopSize(ifftSize / 4),
      m_oversamplingFactor(oversamplingFactor),
      m_K(K/2),
      m_motif(std::move(windowType), m_frameSize, m_oversamplingFactor, m_K),
      m_motifMiddleIndex(m_motif.getMiddleIndex()),
      m_sampleCount(0),
      m_plan(m_frameSize)
{
    m_phases.resize(10000, 0.0);

    m_synthWindow.resize(m_frameSize);
    createSynthWindow();

    for (auto& channel: m_spectrumArray)
        channel.resize(m_halfFrameSize + 1, std::complex<double> (0.0, 0.0));
        
    for (auto& channel: bufferArray)
        channel.resize(m_hopSize);

    for (auto& channel: m_overlapBufferArray)
        channel.resize(m_hopSize, 0.0);
    // Performance: https://stackoverflow.com/questions/8848575/fastest-way-to-reset-every-value-of-stdvectorint-to-0

    //////////////// KFR ////////////////
    for (int i = 0; i < m_channels; ++i)
    {
        m_ifftSpectrumArray[i].resize(m_halfFrameSize + 1, kfr::complex(0.0, 0.0));
        m_ifftSamplesArray[i].resize(m_frameSize, 0.0);
    }
}

IFFT::~IFFT() {}

void IFFT::createSpectrum(const std::vector<Partial<float>>& partials) noexcept
{
    double binRealLocation;
    int binFrameLocation;
    double binRemainder;
    double currentAmplitude;
    double currentFrequency;
    double phaseOffset;
    double currentPhase;
    double real;
    double imag;
    double cosPhase;
    double sinPhase;
    double amplitudeFactor;
    //std::fill (m_spectrum.begin(), m_spectrum.end(), std::complex<double> (0.0, 0.0));
    for (auto& channel: m_spectrumArray)
    {
        std::fill(channel.begin(), channel.end(), std::complex<double>(0.0, 0.0));
    }
    
    //for (int i = 0; i < m_frequencies.size(); i++)
    
    for (int i = 0; i < static_cast<int>(partials.size()); i++)
    {
        currentAmplitude = 0.5 * partials[i].amplitude;
        currentFrequency = partials[i].frequency;
        binRealLocation = currentFrequency * m_frameSize * m_T;
        binFrameLocation = (int)floor (binRealLocation + 0.5);
        binRemainder = floor (binRealLocation + 0.5) - binRealLocation;

        m_phases[i] = std::fmod(m_phases[i] + 2 * M_PI * currentFrequency * 0.5 * m_hopSize * m_T, 2 * M_PI);
        currentPhase = m_phases[i];
        m_phases[i] = std::fmod(m_phases[i] + 2 * M_PI * currentFrequency * 0.5 * m_hopSize * m_T, 2 * M_PI);
        
        //m_phases[i] = std::fmod(m_phases[i] + 2 * M_PI * currentFrequency * m_hopSize * m_T, 2 * M_PI);

        cosPhase = cos(currentPhase);
        sinPhase = sin(currentPhase);

        if ((binRealLocation >= m_K + 1) && (binRealLocation < m_halfFrameSize - m_K))
        {
            for (int j = -m_K; j <= m_K; ++j)
            {
                // TODO: Use the symmetry of the motif?
                amplitudeFactor = currentAmplitude * m_motif.getRealValueAtIndex((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex); 
                real = amplitudeFactor * cosPhase;
                imag = amplitudeFactor * sinPhase;
                //real = currentAmplitude 
                //       * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                //       * cosPhase;
                //imag = currentAmplitude 
                //       * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                //       * sinPhase;

                for (int c = 0; c < m_channels; ++c)    
                    m_spectrumArray[c][binFrameLocation + j] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                     partials[i].bFormat[c] * imag);
            }
        }
        else if ((binRealLocation > 0) && (binRealLocation < m_K + 1))
        {
            for (int j = -m_K; j <= m_K; ++j)
            {
                if (binFrameLocation + j < 0)
                {
                    real = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex) 
                           * cosPhase;
                    imag = -1 * currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex) 
                           * sinPhase;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][-1 * (binFrameLocation + j)] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                                partials[i].bFormat[c] * imag);
                }
                else if (binFrameLocation + j == 0)
                {
                    real = 2.0 * currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex) 
                           * cosPhase;
                    imag = 0.0;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][binFrameLocation + j] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                         partials[i].bFormat[c] * imag);
                }
                else
                {
                    real = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * cosPhase;
                    imag = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * sinPhase;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][binFrameLocation + j] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                         partials[i].bFormat[c] * imag);
                }
            }
        }
        else if ((binRealLocation >= m_halfFrameSize - m_K) && (binRealLocation < m_halfFrameSize - 1))
        {
            for (int j = -m_K; j <= m_K; ++j)
            {
                if (binFrameLocation + j > m_halfFrameSize)
                {
                    real = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * cosPhase;
                    imag = -1 * currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex) 
                           * sinPhase;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][m_frameSize - (binFrameLocation + j)] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                                         partials[i].bFormat[c] * imag);
                }
                else if (binFrameLocation + j == m_halfFrameSize)
                {
                    real = 2.0 * currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * cosPhase;
                    imag = 0.0;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][binFrameLocation + j] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                         partials[i].bFormat[c] * imag);
                }
                else
                {
                    real = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * cosPhase;
                    imag = currentAmplitude 
                           * m_motif.getRealValueAtIndex ((int)((binRemainder + j) * m_oversamplingFactor) + m_motifMiddleIndex)
                           * sinPhase;

                    for (int c = 0; c < m_channels; ++c)    
                        m_spectrumArray[c][binFrameLocation + j] += std::complex<double>(partials[i].bFormat[c] * real, 
                                                                                         partials[i].bFormat[c] * imag);
                }
            }
        }

        //m_phases[i] = std::fmod(currentPhase + 2 * M_PI * currentFrequency * 0.5 * m_hopSize * m_T, 2 * M_PI);
    }
}
   
void IFFT::IFFTprocess() noexcept
{
    for (int i = 0; i < m_channels; ++i)
    {
        for (int j = 0; j < m_halfFrameSize + 1; ++j)
        {
            m_ifftSpectrumArray[i][j] = kfr::complex(m_spectrumArray[i][j].real(), m_spectrumArray[i][j].imag()); 
        }

        m_plan.execute(m_ifftSamplesArray[i], m_ifftSpectrumArray[i], m_temp);

        for (int k = 0; k < m_hopSize; ++k)
        {
            bufferArray[i][k] = m_ifftSamplesArray[i][m_halfFrameSize + m_hopSize + k] * m_synthWindow[m_hopSize + k] / m_frameSize + m_overlapBufferArray[i][k];
            //bufferArray[i][k] = m_ifftSamplesArray[i][k] * m_synthWindow[m_halfFrameSize + k] / m_frameSize + m_overlapBufferArray[i][k];
            m_overlapBufferArray[i][k] = m_ifftSamplesArray[i][k] * m_synthWindow[m_halfFrameSize + k] / m_frameSize;
        }
    }
}

void IFFT::setChannels(int channels) noexcept
{
   m_channels = channels;
}

void IFFT::createSynthWindow()
{
    int twoTimeshopSize = 2 * m_hopSize;

    Window<double> inverseMotif(m_WindowType, m_frameSize, false);
    inverseMotif.normalize();
    auto inverseMotifWindow = inverseMotif.getWindow();

    Window<double> overlap(WindowType::Triangular, twoTimeshopSize, true);
    auto overlapWindow = overlap.getWindow();

    std::transform(m_synthWindow.begin() + m_hopSize, m_synthWindow.begin() + m_hopSize + twoTimeshopSize,
                   overlapWindow.begin(), m_synthWindow.begin() + m_hopSize,
                   std::plus<double>());

    std::transform(m_synthWindow.begin(), m_synthWindow.end(),
                   inverseMotifWindow.begin(), m_synthWindow.begin(),
                   std::divides<double>());
}   

double IFFT::phaseAdjustment(double phase, double frequency) noexcept
{
    return std::fmod(phase + 2 * M_PI * frequency * 0.5 * m_hopSize * m_T, 2 * M_PI);
    //double x =phase + frequency * m_phaseCorrectionFactor;
    //return x - trunc(x / (2 * M_PI)) * 2 * M_PI; 
}

//void IFFT::resetPhase()
//{
//    m_currentPhase = 0.0;
//    m_phases.resize(10000, 0.0);
//}

void IFFT::setSampleRate(float sampleRate) noexcept
{
    m_T = 1.0 / sampleRate;
}

//int IFFT::getTimer() noexcept { return m_sampleCount; }

//void IFFT::setTimer(int time) noexcept { m_sampleCount = time; }

//void IFFT::resetTimer() noexcept { m_sampleCount = 0; }

//int IFFT::getHopSize() noexcept { return m_hopSize; }

