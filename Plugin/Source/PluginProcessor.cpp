#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginAudioProcessor::PluginAudioProcessor() 
        : PluginHelpers::ProcessorBase(getBuses()),
          Thread ("Background Thread"),
          parameters(*this, nullptr, juce::Identifier("PARAMETERS"), createParameters()),
          signal(SignalType::sine, 0.5, 440.0, 440.0, 0.0),
          gainEnvelope(),
          gateCount(0)
{
    waveformParameter = parameters.getRawParameterValue("waveform");
    noiseDensityParameter = parameters.getRawParameterValue("noiseDensity");    
    brightnessParameter = parameters.getRawParameterValue("brightness");    
    distanceParameter = parameters.getRawParameterValue("distance");
    azimuthAngleParameter = parameters.getRawParameterValue("azimuthAngle");
    azimuthDisplacementParameter = parameters.getRawParameterValue("azimuthDisplacement");
    widthParameter = parameters.getRawParameterValue("width");
    horizontalDispersionParameter = parameters.getRawParameterValue("horizontalDispersion");
    elevationAngleParameter = parameters.getRawParameterValue("elevationAngle");
    elevationDisplacementParameter = parameters.getRawParameterValue("elevationDisplacement");
    heightParameter = parameters.getRawParameterValue("height");
    verticalDispersionParameter = parameters.getRawParameterValue("verticalDispersion");
    ambisonicsOrderParameter = parameters.getRawParameterValue("ambisonicsOrder");
    ambisonicsNormalisationParameter = parameters.getRawParameterValue("ambisonicsNormalisation");
    gainAttackParameter = parameters.getRawParameterValue("gainAttack");
    gainDecayParameter = parameters.getRawParameterValue("gainDecay");
    gainSustainParameter = parameters.getRawParameterValue("gainSustain");
    gainReleaseParameter = parameters.getRawParameterValue("gainRelease");
    
    if (BENCHMARKING)
    {
        startThread();
    }
}

PluginAudioProcessor::~PluginAudioProcessor()
{
    if (BENCHMARKING)
    {
        stopThread(2000);
    }
}

void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    azimuthAngle.reset(sampleRate, 0.01);
    elevationAngle.reset(sampleRate, 0.01);
    width.reset(sampleRate, 0.01);
    height.reset(sampleRate, 0.01);
    horizontalDispersion.reset(sampleRate, 0.01);
    verticalDispersion.reset(sampleRate, 0.01);

    ifft = new IFFT(4 * samplesPerBlock, WindowType::BlackmanHarris4term, 128, 7);
     
    ifft->setSampleRate(sampleRate);
    ifft->setTimer(ifft->getHopSize());

    gainEnvelope.setAttackRate(0.1 * sampleRate);
    gainEnvelope.setDecayRate(0.5 * sampleRate);
    gainEnvelope.setSustainLevel(0.8);
    gainEnvelope.setReleaseRate(1.5 * sampleRate);
    gainEnvelopeBuffer.resize(2 * samplesPerBlock, 0.0);
    
    int outputChannels = getMainBusNumOutputChannels();

    if (outputChannels >= 16)
        ifft->setChannels(16);
    else if (outputChannels >= 9)
        ifft->setChannels(9);
    else if (outputChannels >= 4)
        ifft->setChannels(4);
    else
        ifft->setChannels(1);
         
    triggerAsyncUpdate();
        
    timeDomain = new TimeDomain(samplesPerBlock, static_cast<float>(sampleRate));

    if (BENCHMARKING)
    {
        Timer::initializeTimeData;
    }
}

void PluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    float sampleRate = getSampleRate();

    int channelsGui = static_cast<int>(*ambisonicsOrderParameter);
    int channelsHost = getMainBusNumOutputChannels();
    int channelsIFFT = channelsHost < channelsGui ? channelsHost : channelsGui;
    
    if (BENCHMARKING)
    {
        ifft->setChannels(CHANNELS);
        timeDomain->setChannels(CHANNELS);
    }
    else
    {
        ifft->setChannels(channelsIFFT);
        timeDomain->setChannels(channelsIFFT);
    }
        
    for (auto i = 0; i < channelsHost; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        
        if (msg.isNoteOn())
        {  
            ++gateCount;
            gainEnvelope.gate(true);
            signal.setFrequency(static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(msg.getNoteNumber())));
        }
        else if (msg.isNoteOff()) 
        {
            --gateCount;
            if (gateCount <= 0)
                gainEnvelope.gate(false);
        }
    }
    
    midiMessages.clear();
    
    
    gainEnvelope.setAttackRate(*gainAttackParameter * sampleRate);
    gainEnvelope.setDecayRate(*gainDecayParameter * sampleRate);
    gainEnvelope.setSustainLevel(*gainSustainParameter);
    gainEnvelope.setReleaseRate(*gainReleaseParameter * sampleRate);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        gainEnvelopeBuffer[i] = gainEnvelope.process();

    azimuthAngle.setTargetValue(azimuthAngleParameter->load());
    elevationAngle.setTargetValue(elevationAngleParameter->load());
    width.setTargetValue(widthParameter->load());
    horizontalDispersion.setTargetValue(horizontalDispersionParameter->load());
    height.setTargetValue(heightParameter->load());
    verticalDispersion.setTargetValue(verticalDispersionParameter->load());

    signal.reset();

    if (BENCHMARKING)
    {
        signal.setNumberOfPartials(PARTIALS);
        signal.createSignal(static_cast<SignalType>(5));
    }
    else
    {
        signal.setNumberOfPartials(static_cast<int>(*noiseDensityParameter));
        signal.createSignal(static_cast<SignalType>(static_cast<int>(*waveformParameter)));
    }

    
    signal.setBrightness(*brightnessParameter);
    signal.setNormalisation(static_cast<Normalisation>(static_cast<int>(*ambisonicsNormalisationParameter)));

    float azmthAng = azimuthAngle.getNextValue();
    azmthAng = azmthAng < 0.0 ? azmthAng * (-1.0) : 360.0 - azmthAng;

    signal.setSpatialParameters(*distanceParameter, azmthAng * M_PI / 180.0, elevationAngle.getNextValue() * M_PI / 180.0);
    signal.setAzimuthDisplacement(static_cast<DisplacementFunction>(static_cast<int>(*azimuthDisplacementParameter) - 1), width.getNextValue() * M_PI / 360.0, horizontalDispersion.getNextValue());
    signal.setElevationDisplacement(static_cast<DisplacementFunction>(static_cast<int>(*elevationDisplacementParameter) - 1), height.getNextValue() * M_PI / 360.0, verticalDispersion.getNextValue());

    // Benchmarking Frequency Domain
    if (FREQDOMAIN)
    {
        {
            //Timer timer;
        //if (ifft->getTimer() >= ifft->getHopSize())
        //{
            ifft->createSpectrum(signal.getPartials());
            ifft->IFFTprocess();
        }
            //ifft->resetTimer();
        //}
    } 
    else
    {
        //Timer timer;
        timeDomain->process(signal.getPartials());
    }

    //int offset = ifft->getTimer();
   
    if (channelsHost == 2)
    {
        for (int channel = 0; channel < channelsHost; ++channel)
        {
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                if (FREQDOMAIN)
                    buffer.setSample(channel, sample, static_cast<float>(ifft->bufferArray[0][sample] * gainEnvelopeBuffer[sample]));
                    //buffer.setSample(channel, sample, static_cast<float>(ifft->bufferArray[0][offset + sample] * gainEnvelopeBuffer[sample]));
                else
                    buffer.setSample(channel, sample, timeDomain->bufferArray[0][sample] * gainEnvelopeBuffer[sample]);
            }
        }
    }
    else
    {
        for (int channel = 0; channel < channelsIFFT; ++channel)
        {
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                if (FREQDOMAIN)
                    buffer.setSample(channel, sample, static_cast<float>(ifft->bufferArray[channel][sample] * gainEnvelopeBuffer[sample]));
                    //buffer.setSample(channel, sample, static_cast<float>(ifft->bufferArray[channel][offset + sample] * gainEnvelopeBuffer[sample]));
                else
                    buffer.setSample(channel, sample, timeDomain->bufferArray[channel][sample] * gainEnvelopeBuffer[sample]);
            }
        }
    } 
    
    //ifft->setTimer(ifft->getTimer() + buffer.getNumSamples());
}

juce::AudioProcessor::BusesProperties PluginAudioProcessor::getBuses()
{
    juce::AudioProcessor::BusesProperties properties;

    auto thirdOrder = juce::AudioChannelSet::ambisonic(3);

    properties = properties.withOutput("Output", thirdOrder, true);

    return properties;
}

bool PluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannels() > 16)
        return false;
    
    return true;
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    params.add(std::make_unique<juce::AudioParameterInt>("waveform", "Waveform", 1, 5, 5)); 
    params.add(std::make_unique<juce::AudioParameterInt>("noiseDensity", "Noise Density", 1, 10000, 1000));
    params.add(std::make_unique<juce::AudioParameterFloat>("brightness", "Brightness", juce::NormalisableRange<float>(0.5, 250.0, 0.01, 0.2), 10.0));
    params.add(std::make_unique<juce::AudioParameterFloat>("distance", "Distance", juce::NormalisableRange<float>(1.0, 100.0, 0.01, 0.3), 1.0));
    params.add(std::make_unique<juce::AudioParameterFloat>("azimuthAngle", "Azimuth Angle", -180.0, 180.0, 0.0));
    params.add(std::make_unique<juce::AudioParameterInt>("azimuthDisplacement", "Azimuth Displacement Function", 1, 5, 2));
    params.add(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0, 180.0, 0.0));
    params.add(std::make_unique<juce::AudioParameterFloat>("horizontalDispersion", " Horizontal Dispersion", juce::NormalisableRange<float>(0.0, 300.0, 0.1), 1.0));
    params.add(std::make_unique<juce::AudioParameterFloat>("elevationAngle", "Elevation Angle", 0.0, 90.0, 0.0));
    params.add(std::make_unique<juce::AudioParameterInt>("elevationDisplacement", "Elevation Displacement Function", 1, 5, 2));
    params.add(std::make_unique<juce::AudioParameterFloat>("height", "Height", 0.0, 90.0, 0.0));
    params.add(std::make_unique<juce::AudioParameterFloat>("verticalDispersion", "Vertical Dispersion", juce::NormalisableRange<float>(0.0, 300.0, 0.1), 1.0));
    params.add(std::make_unique<juce::AudioParameterInt>("ambisonicsOrder", "Ambisonics Order", 1, 16, 16));
    params.add(std::make_unique<juce::AudioParameterInt>("ambisonicsNormalisation", "Normalisation", 1, 2, 1));
    params.add(std::make_unique<juce::AudioParameterFloat>("gainAttack", "Gain Attack", juce::NormalisableRange<float>(0.0, 5.0, 0.01, 0.3), 0.1));
    params.add(std::make_unique<juce::AudioParameterFloat>("gainDecay", "Gain Decay", juce::NormalisableRange<float>(0.0, 8.0, 0.01, 0.3), 0.5));
    params.add(std::make_unique<juce::AudioParameterFloat>("gainSustain", "Gain Sustain", juce::NormalisableRange<float>(0.0, 1.0, 0.01, 1.3), 0.8));
    params.add(std::make_unique<juce::AudioParameterFloat>("gainRelease", "Gain Release", juce::NormalisableRange<float>(0.0, 8.0, 0.01, 0.3), 1.5));

    return params;
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginAudioProcessorEditor(*this, parameters);
}

void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    //Serializes your parameters, and any other potential data into an XML:

    juce::ValueTree params("Params");

    for (auto& param: getParameters())
    {
        juce::ValueTree paramTree(PluginHelpers::getParamID(param));
        paramTree.setProperty("Value", param->getValue(), nullptr);
        params.appendChild(paramTree, nullptr);
    }

    juce::ValueTree pluginPreset("MyPlugin");
    pluginPreset.appendChild(params, nullptr);
    //This a good place to add any non-parameters to your preset

    copyXmlToBinary(*pluginPreset.createXml(), destData);
}

void PluginAudioProcessor::setStateInformation(const void* data,
                                                          int sizeInBytes)
{
    //Loads your parameters, and any other potential data from an XML:

    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto preset = juce::ValueTree::fromXml(*xml);
        auto params = preset.getChildWithName("Params");

        for (auto& param: getParameters())
        {
            auto paramTree = params.getChildWithName(PluginHelpers::getParamID(param));

            if (paramTree.isValid())
                param->setValueNotifyingHost(paramTree["Value"]);
        }

        //Load your non-parameter data now
    }
}

void PluginAudioProcessor::handleAsyncUpdate() 
{
    if (auto* editor = getActiveEditor())
        if (auto* PluginEditor = dynamic_cast<PluginAudioProcessorEditor*> (editor))
            PluginEditor->updateGUI();
}

void PluginAudioProcessor::run()
{
    wait(30000);
    
    Timer::writeTimeData(CHANNELS, PARTIALS, FREQDOMAIN, getBlockSize(), getSampleRate());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}