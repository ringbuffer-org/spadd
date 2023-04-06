/**
 * \class SpectralMotif
 *
 *
 * \brief The spectral motif class ...
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

#include <vector>
#include <iterator>
#include <complex>
#include <memory>
#include <dft.hpp>
#include <shared_processing_code/shared_processing_code.h>

/******************* NOTES ***********************/
// Using IFFT size as window size before 
// oversampling yields smallest distortion!
/*************************************************/

template <typename T>
class SpectralMotif
{
public:
   SpectralMotif(WindowType type, 
                 int ifftSize,
                 int oversampling, 
                 int bins);

   ~SpectralMotif();
    
   void createSpectralMotif();
                            
   /*static void getSpectralMotif (std::vector<double> SpectralMotif,
                                 WindowType type, 
                                 size_t frameSize, 
                                 size_t oversamplingFactor, 
                                 size_t K);*/

   std::complex<T> getComplexValueAtIndex(int index);
   std::vector<std::complex<T>> getSpectralMotifComplex();

   T getRealValueAtIndex(int index);
   std::vector<T> getSpectralMotifReal();
   
   int getMiddleIndex();

private:
   std::vector<std::complex<T>> m_spectralMotifComplex;   

   std::vector<T> m_spectralMotifReal;   

   WindowType m_windowType;

   int m_frameSize;

   int m_oversamplingFactor;
   
   int m_K;

   int m_windowSize; 
   
   int m_middleIndex;

   kfr::dft_plan<T> m_plan;
   kfr::univector<kfr::u8> m_temp;
   kfr::univector<kfr::complex<T>> m_fftInput;
   kfr::univector<kfr::complex<T>> m_fftOutput;
   
   std::vector<std::complex<T>> m_complexOutput;
   std::vector<T> m_realOutput;
};