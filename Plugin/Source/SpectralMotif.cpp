#include "SpectralMotif.hpp"

template <typename T>
SpectralMotif<T>::SpectralMotif (WindowType type,
                                 int ifftSize,
                                 int oversampling,
                                 int bins)
    : m_windowType(type),
      m_frameSize(ifftSize),
      m_oversamplingFactor(oversampling),
      m_K(bins),     
      m_windowSize(m_frameSize * m_oversamplingFactor),
      m_middleIndex(m_oversamplingFactor * (m_K + 1)),
      m_plan(m_windowSize),
      m_temp(m_plan.temp_size)
{
    m_spectralMotifComplex.resize(2 * m_middleIndex, std::complex<T>(0.0, 0.0));
    m_spectralMotifReal.resize(2 * m_middleIndex, 0.0);

    m_fftInput.resize(m_windowSize, kfr::complex<T>(0.0, 0.0));
    m_fftOutput.resize(m_windowSize, kfr::complex<T>(0.0, 0.0));
    m_complexOutput.reserve(m_windowSize);
    m_realOutput.reserve(m_windowSize);

    createSpectralMotif();
}

template <typename T>
SpectralMotif<T>::~SpectralMotif() {}

template <typename T>
void SpectralMotif<T>::createSpectralMotif()
{
    Window<T> m_window(m_windowType, m_frameSize, false);
    m_window.normalize();
    m_window.zeroPhase();
    m_window.zeroPad(m_windowSize, true);
    std::vector<T> windowValues = m_window.getWindow();

    ///////////////// KFR ///////////////////
    for (int i = 0; i < m_windowSize; ++i)
    {
        m_fftInput[i] = kfr::complex<T>(windowValues[i], 0.0);
    }
    
    m_plan.execute(m_fftOutput, m_fftInput, m_temp, false); 

    // Set only the mainlobe  
    for (int i = 0; i < m_middleIndex; ++i)
    {
        m_spectralMotifComplex[m_middleIndex + i] = std::complex<T>(m_fftOutput[i].real(), m_fftOutput[i].imag());
        m_spectralMotifComplex[i] = std::complex<T>(m_fftOutput[m_windowSize - m_middleIndex + i].real(), m_fftOutput[m_windowSize - m_middleIndex + i].imag());

        m_spectralMotifReal[m_middleIndex + i] = m_fftOutput[i].real();
        m_spectralMotifReal[i] = m_fftOutput[m_windowSize - m_middleIndex + i].real();
    }

    //std::cout << m_spectralMotifComplex[m_middleIndex].imag() << "\n";
}

template <typename T>
std::complex<T> SpectralMotif<T>::getComplexValueAtIndex(int index)
{
    return m_spectralMotifComplex[index];
}

template <typename T>
std::vector<std::complex<T>> SpectralMotif<T>::getSpectralMotifComplex()
{
   return m_spectralMotifComplex; 
}

template <typename T>
T SpectralMotif<T>::getRealValueAtIndex(int index)
{
    return m_spectralMotifReal[index];
}

template <typename T>
std::vector<T> SpectralMotif<T>::getSpectralMotifReal()
{
   return m_spectralMotifReal; 
}

template <typename T>
int SpectralMotif<T>::getMiddleIndex()
{
    return m_middleIndex;
}

template class SpectralMotif<float>;
template class SpectralMotif<double>;

