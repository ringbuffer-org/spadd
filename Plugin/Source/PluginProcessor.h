#pragma once

#include <shared_processing_code/shared_processing_code.h>
#include <shared_plugin_helpers/shared_plugin_helpers.h>
#include "IFFT.hpp"
#include "TimeDomain.hpp"
#include "ADSR.hpp"
#include "Timer.hpp"

/**** Benchmarking Constants ****/
const bool BENCHMARKING = false;
const bool FREQDOMAIN = true; // false => time domain
const int PARTIALS = 10000;
const int CHANNELS = 16;
/********************************/

class PluginAudioProcessor : public PluginHelpers::ProcessorBase,
                             private juce::AsyncUpdater,
                             private juce::Thread
{
public:
    PluginAudioProcessor();
    ~PluginAudioProcessor();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void handleAsyncUpdate() override;

    int gateCount;

private:
    static juce::AudioProcessor::BusesProperties getBuses();
    
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    BasicSignals signal;
    IFFT* ifft;
    
    ADSR gainEnvelope;
    std::vector<double> gainEnvelopeBuffer;

    TimeDomain* timeDomain;
    
    juce::LinearSmoothedValue<float> elevationAngle { 0.0 };
    juce::LinearSmoothedValue<float> azimuthAngle { 0.0 };
    juce::LinearSmoothedValue<float> width { 0.0 };
    juce::LinearSmoothedValue<float> horizontalDispersion { 0.0 };
    juce::LinearSmoothedValue<float> height { 0.0 };
    juce::LinearSmoothedValue<float> verticalDispersion { 0.0 };
    
    std::atomic<float>* waveformParameter = nullptr;
    std::atomic<float>* noiseDensityParameter = nullptr;
    std::atomic<float>* brightnessParameter = nullptr;
    std::atomic<float>* distanceParameter = nullptr;
    std::atomic<float>* azimuthAngleParameter = nullptr;
    std::atomic<float>* azimuthDisplacementParameter = nullptr;
    std::atomic<float>* widthParameter = nullptr;
    std::atomic<float>* horizontalDispersionParameter = nullptr;
    std::atomic<float>* elevationAngleParameter = nullptr;
    std::atomic<float>* elevationDisplacementParameter = nullptr;
    std::atomic<float>* heightParameter = nullptr;
    std::atomic<float>* verticalDispersionParameter = nullptr;
    std::atomic<float>* ambisonicsOrderParameter = nullptr;
    std::atomic<float>* ambisonicsNormalisationParameter = nullptr;
    std::atomic<float>* gainAttackParameter = nullptr;
    std::atomic<float>* gainDecayParameter = nullptr;
    std::atomic<float>* gainSustainParameter = nullptr;
    std::atomic<float>* gainReleaseParameter = nullptr;
    
    void run() override;
};
