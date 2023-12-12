#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <shared_processing_code/shared_processing_code.h>
#include "../../Plugin/Source/IFFT.hpp"

const bool SNR_K = true; // SNR_K = true: K and the windows are measured, SNR_K = false: The oversampling factor is scrutinized
const double sampleRate = 48000.0;
const int bufferSize = 256;
const int bufferCount = 8;

int main()
{
    if (SNR_K)
    {
        // Constant data
        const float frequency = 968.719; // Unfavourable bin location: For example Sample rate / IFFT size * 10.333
        const int oversamplingFactor = 128;
        
        // Prepare .csv file
        std::ostringstream filePath;
        filePath << "./" << "SNR_K" << ".csv";
        std::ofstream snrDataFile;
        snrDataFile.open(filePath.str());
        
        snrDataFile << "Signal-to-noise ratio" << "\n" << "\n";
        snrDataFile << "Sample rate:" << "," << sampleRate << "\n";
        snrDataFile << "Buffer size:" << "," << bufferSize << "\n";
        snrDataFile << "Frequency:" << "," << frequency << "\n";
        snrDataFile << "Oversampling factor:" << "," << oversamplingFactor << "\n" << "\n";

        std::cout << "-----------------------------------------------" << "\n";
        std::cout << "--------------Signal-to-noise Ratio------------" << "\n";
        std::cout << "-----------------------------------------------" << "\n";

        // Initialize data buffer
        std::vector<double> referenceData;
        std::vector<double> ifftData;
        referenceData.resize(bufferCount * bufferSize);
        ifftData.resize(bufferCount * bufferSize); 
        
        // Create test signal
        BasicSignals signal = BasicSignals(SignalType::sine, 1.0, frequency, frequency, 0.0); 
        signal.setSpatialParameters(1.0, 0.0, 0.0);
        signal.setAzimuthDisplacement(static_cast<DisplacementFunction>(0), 0.0, 0.0);
        signal.setElevationDisplacement(static_cast<DisplacementFunction>(0), 0.0, 0.0);
        auto signalData = signal.getPartials();
     
        // Choosing windows
        WindowType windows[] = {WindowType::Hann, WindowType::Kaiser, WindowType::BlackmanHarris4term}; 
        std::string windowNames[] = {"Hann", "Kaiser (beta=8)", "Blackman-Harris four-term"};
        int windowCount = 0;

        for (auto windowType: windows)
        {
            snrDataFile << windowNames[windowCount] << "\n";
            
            for (int K = 3; K <= 11; K += 2)
            {
                // Create IFFT data
                IFFT ifft = IFFT(4 * bufferSize, windowType, oversamplingFactor, K);
                ifft.setSampleRate(sampleRate);

                for (int i = 0; i < bufferCount; ++i)
                {     
                    ifft.createSpectrum(signalData);
                    ifft.IFFTprocess();
                    
                    for (int j = 0; j < bufferSize; ++j)
                    {
                        ifftData[i * bufferSize + j] = ifft.bufferArray[0][j];
                    }
                }
                
                // Calculate Energy of signal and noise and compare signals
                double signalEnergy = 0.0;
                double noiseEnergy = 0.0;
                double signalDifference = 0.0;

                // Start reference data at the initial phase of IFFT data
                double currentAngle = std::fmod(2 * M_PI * frequency * 0.5 * bufferSize * 1.0 / sampleRate, 2 * M_PI);

                double angleDelta = frequency / sampleRate * 2 * M_PI;

                for (int i = 0; i < (bufferCount - 1) * bufferSize; ++i)
                {     
                    referenceData[bufferSize + i] = std::cos(currentAngle);
                    currentAngle += angleDelta;
                    if (currentAngle >= 2 * M_PI)
                        currentAngle -= 2 * M_PI;
                    
                    // First buffers are discarded because there is no overlap data yet
                    
                    // Calculate Energy of signal
                    signalEnergy += referenceData[bufferSize + i] * referenceData[bufferSize + i];
                    
                    // Calculate Energy of noise
                    signalDifference = referenceData[bufferSize + i] - ifftData[bufferSize + i];    

                    noiseEnergy += signalDifference * signalDifference;
                }

                std::cout << "-----------------------------------------------" << "\n";
                std::cout << "Energy of signal: " << signalEnergy << "\n";
                std::cout << "Energy of noise: " << noiseEnergy << "\n";
                
                // Calculate SNR
                double SNR = 10 * log10(signalEnergy / noiseEnergy);

                std::cout   << "Signal-to-noise Ratio for K = " << K 
                            << " and window type = " << windowNames[windowCount]
                            << ": " << SNR << " dB" << "\n"; 
                std::cout << "-----------------------------------------------" << "\n";
                
                snrDataFile << K << "," << SNR << "\n";
            }
            ++windowCount;
        }
        
        snrDataFile.close();
    }
    else
    {
        // Constant data
        WindowType window = WindowType::Kaiser;
        std::string windowName = "Kaiser (beta=8)";
        const int K = 5;

        // Prepare .csv file
        std::ostringstream filePath;
        filePath << "./" << "SNR_O" << ".csv";
        std::ofstream snrDataFile;
        snrDataFile.open(filePath.str());
        
        snrDataFile << "Signal-to-noise ratio" << "\n" << "\n";
        snrDataFile << "Sample rate:" << "," << sampleRate << "\n";
        snrDataFile << "Buffer size:" << "," << bufferSize << "\n";
        snrDataFile << "K:" << "," << K << "\n";
        snrDataFile << "Window type:" << "," << windowName << "\n" << "\n";

        std::cout << "-----------------------------------------------" << "\n";
        std::cout << "--------------Signal-to-noise Ratio------------" << "\n";
        std::cout << "-----------------------------------------------" << "\n";

        // Initialize data buffer
        std::vector<double> referenceData;
        std::vector<double> ifftData;
        referenceData.resize(bufferCount * bufferSize);
        ifftData.resize(bufferCount * bufferSize); 
        
        // Oversampling factors
        std::vector<int> oversamplingFactors = {512};

        for (auto O: oversamplingFactors)
        {
            snrDataFile << "Oversampling factor:" << "," << O << "\n";
            
            for (double frequency = 20.0; frequency < 20000.0; frequency += 40)
            {
                // Create signal data
                BasicSignals signal = BasicSignals(SignalType::sine, 1.0, frequency, frequency, 0.0); 
                signal.setSpatialParameters(1.0, 0.0, 0.0);
                signal.setAzimuthDisplacement(static_cast<DisplacementFunction>(0), 0.0, 0.0);
                signal.setElevationDisplacement(static_cast<DisplacementFunction>(0), 0.0, 0.0);
                auto signalData = signal.getPartials();
                
                // Create IFFT data
                IFFT ifft = IFFT(4 * bufferSize, window, O, K);
                ifft.setSampleRate(sampleRate);

                for (int i = 0; i < bufferCount; ++i)
                {     
                    ifft.createSpectrum(signalData);
                    ifft.IFFTprocess();
                    
                    for (int j = 0; j < bufferSize; ++j)
                    {
                        ifftData[i * bufferSize + j] = ifft.bufferArray[0][j];
                    }
                }
                
                // Calculate Energy of signal and noise and compare signals
                double signalEnergy = 0.0;
                double noiseEnergy = 0.0;
                double signalDifference = 0.0;

                // Start reference data at the initial phase of IFFT data
                double currentAngle = std::fmod(2 * M_PI * frequency * 0.5 * bufferSize * 1.0 / sampleRate, 2 * M_PI);
                
                double angleDelta = frequency / sampleRate * 2 * M_PI;

                for (int i = 0; i < (bufferCount - 1) * bufferSize; ++i)
                {     
                    referenceData[bufferSize + i] = std::cos(currentAngle);
                    currentAngle += angleDelta;
                    if (currentAngle >= 2 * M_PI)
                        currentAngle -= 2 * M_PI;
                    
                    // First buffers are discarded because there is no overlap data yet
                    
                    // Calculate Energy of signal
                    signalEnergy += referenceData[bufferSize + i] * referenceData[bufferSize + i];
                    
                    // Calculate Energy of noise
                    signalDifference = referenceData[bufferSize + i] - ifftData[bufferSize + i];    

                    noiseEnergy += signalDifference * signalDifference;
                }

                std::cout << "-----------------------------------------------" << "\n";
                std::cout << "Energy of signal: " << signalEnergy << "\n";
                std::cout << "Energy of noise: " << noiseEnergy << "\n";
                
                // Calculate SNR
                double SNR = 10 * log10(signalEnergy / noiseEnergy);

                std::cout   << "Signal-to-noise Ratio for O = " << O 
                            << " and frequency = " << frequency
                            << ": " << SNR << " dB" << "\n"; 
                std::cout << "-----------------------------------------------" << "\n";
                
                snrDataFile << frequency << "," << SNR << "\n";
            }
        }
        
        snrDataFile.close();
    }

    return 0;
}
