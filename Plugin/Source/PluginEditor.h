#pragma once

#include "PluginProcessor.h"

class PluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    explicit PluginAudioProcessorEditor(PluginAudioProcessor&, juce::AudioProcessorValueTreeState&);
    
    enum
    {
        paramControlHeight = 20,
        paramLabelWidth    = 80,
        paramSliderWidth   = 280
    };

    void updateGUI();

private:
    juce::AudioProcessor& processor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    bool lastSuspended;

    juce::Font parameterFont{14.0f};
    juce::Label bufferSizeLabel; 
    juce::Label sampleRateLabel; 
    juce::Label busLayoutLabel; 
    juce::Label ifftSizeLabel; 

    juce::ComboBox waveformComboBox;
    juce::Label waveformLabel{{}, "Waveform"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
    juce::Slider noiseDensitySlider; 
    juce::Label noiseDensityLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseDensitySliderAttachment; 
    juce::Slider brightnessSlider; 
    juce::Label brightnessLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> brightnessSliderAttachment; 

    juce::Slider distanceSlider;
    juce::Label  distanceLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distanceSliderAttachment;
    juce::Slider azimuthAngleSlider;
    juce::Label  azimuthAngleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> azimuthalAngleSliderAttachment;
    juce::ComboBox azimuthDisplacementComboBox;
    juce::Label azimuthDisplacementLabel{{}, "Displacement Function"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> azimuthDisplacementAttachment;
    juce::Slider widthSlider;
    juce::Label  widthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthSliderAttachment;
    juce::Slider horizontalDispersionSlider;
    juce::Label  horizontalDispersionLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> horizontalDispersionSliderAttachment;

    juce::Slider elevationAngleSlider;
    juce::Label  elevationAngleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> polarAngleSliderAttachment;
    juce::ComboBox elevationDisplacementComboBox;
    juce::Label elevationDisplacementLabel{{}, "Displacement Function"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> elevationDisplacementAttachment;
    juce::Slider heightSlider;
    juce::Label  heightLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> heightSliderAttachment;
    juce::Slider verticalDispersionSlider;
    juce::Label  verticalDispersionLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> verticalDispersionSliderAttachment;
    
    juce::ComboBox ambisonicsOrderComboBox;
    juce::Label ambisonicsOrderLabel{{}, "Ambisonics Order"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ambisonicsOrderAttachment;
    juce::ComboBox ambisonicsNormalisationComboBox;
    juce::Label ambisonicsNormalisationLabel{{}, "Normalisation"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ambisonicsNormalisationAttachment;
    juce::Slider gainAttackSlider;
    juce::Label  gainAttackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttackSliderAttachment; 
    juce::Slider gainDecaySlider;
    juce::Label  gainDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainDecaySliderAttachment; 
    juce::Slider gainSustainSlider;
    juce::Label  gainSustainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSustainSliderAttachment; 
    juce::Slider gainReleaseSlider;
    juce::Label  gainReleaseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainReleaseSliderAttachment; 
};