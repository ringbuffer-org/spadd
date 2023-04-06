/**
 * \class IFFT
 *
 *
 * \brief The IFFT synthesis Class ...
 *
 *
 * \author Hilko Tondock
 *
 * \version  0.1
 *
 * \date   2023/03/31
 *
 * Contact: h.tondock@campus.tu-berlin.de
 */

#pragma once

#include <algorithm>
#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <complex>
#include <functional>

#include <shared_processing_code/shared_processing_code.h>

#include "SpectralMotif.hpp" 
#include "BasicSignals.hpp"

const int AC = 16; // Ambisonics Channel Number

class IFFT
{
public:
    IFFT(int ifftSize,
         WindowType windowType,
         int oversamplingFactor,
         int K);

    ~IFFT();

    void setChannels(int) noexcept;

    void setSampleRate(float) noexcept;
    
    void createSpectrum(const std::vector<Partial<float>>& partials) noexcept;

    void IFFTprocess() noexcept;

    void resetPhase() noexcept;

    std::array<std::vector<double>, AC> bufferArray;
    
    int getTimer() noexcept; 

    void setTimer(int time) noexcept; 

    void resetTimer() noexcept;

    int getHopSize() noexcept;

private:
    WindowType m_WindowType;
    
    double m_T;
    
    int m_channels;
    
    int m_frameSize;
    
    int m_halfFrameSize;
    
    int m_hopSize;
    
    int m_oversamplingFactor;
    
    int m_K;
    
    SpectralMotif<double> m_motif;

    int m_motifMiddleIndex;

    std::vector<double> m_phases;
    
    std::array<std::vector<std::complex<double>>, AC> m_spectrumArray;
    std::vector<double> m_synthWindow;
    std::array<std::vector<double>, AC> m_overlapBufferArray;

    int m_sampleCount;
   
    ///////////////// KFR ///////////////////////////////
    kfr::dft_plan_real<double> m_plan;
    kfr::univector<kfr::u8> m_temp;
    std::array<kfr::univector<kfr::complex<double>>, AC> m_ifftSpectrumArray;
    std::array<kfr::univector<double>, AC> m_ifftSamplesArray;
    /////////////////////////////////////////////////////
    
    void createSynthWindow();

    double phaseAdjustment (double phase, double frequency) noexcept;
             
};
 
inline int IFFT::getTimer() noexcept { return m_sampleCount; }

inline void IFFT::setTimer(int time) noexcept { m_sampleCount = time; }

inline void IFFT::resetTimer() noexcept { m_sampleCount = 0; }

inline int IFFT::getHopSize() noexcept { return m_hopSize; }