#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginAudioProcessorEditor::PluginAudioProcessorEditor(
    PluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), 
      processor(p),
      valueTreeState(vts)
{
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    getLookAndFeel().setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    getLookAndFeel().setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey);
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::whitesmoke);
    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
    getLookAndFeel().setColour(juce::ComboBox::buttonColourId, juce::Colours::black);
    getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black);
    
    waveformLabel.setFont(parameterFont);
    addAndMakeVisible(&waveformLabel);
    waveformComboBox.addItem("Sine",       1);
    waveformComboBox.addItem("Triangle",   2);
    waveformComboBox.addItem("Saw",        3);
    waveformComboBox.addItem("Square",     4);
    waveformComboBox.addItem("Noise",      5);
    addAndMakeVisible(waveformComboBox);
    waveformAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, "waveform", waveformComboBox);

    noiseDensitySlider.setTextValueSuffix("");
    noiseDensitySlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    noiseDensityLabel.setText("Noise Density", juce::dontSendNotification);
    noiseDensityLabel.attachToComponent(&noiseDensitySlider, false);
    addAndMakeVisible(&noiseDensityLabel);
    addAndMakeVisible(&noiseDensitySlider);
    noiseDensitySliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "noiseDensity", noiseDensitySlider); 

    brightnessSlider.setTextValueSuffix("");
    brightnessSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    brightnessLabel.setText("Rolloff", juce::dontSendNotification);
    brightnessLabel.attachToComponent(&brightnessSlider, false);
    addAndMakeVisible(&brightnessLabel);
    addAndMakeVisible(&brightnessSlider);
    brightnessSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "brightness", brightnessSlider); 

    addAndMakeVisible(&bufferSizeLabel);
    addAndMakeVisible(&sampleRateLabel);
    addAndMakeVisible(&busLayoutLabel);

    distanceSlider.setTextValueSuffix("");
    distanceSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    distanceLabel.setText("Distance", juce::dontSendNotification);
    distanceLabel.attachToComponent(&distanceSlider, false);
    addAndMakeVisible(&distanceLabel);
    addAndMakeVisible(&distanceSlider);
    distanceSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "distance", distanceSlider);

    azimuthAngleSlider.setTextValueSuffix("");
    azimuthAngleSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    azimuthAngleLabel.setText("Angle", juce::dontSendNotification);
    azimuthAngleLabel.attachToComponent(&azimuthAngleSlider, false);
    addAndMakeVisible(&azimuthAngleLabel);
    addAndMakeVisible(&azimuthAngleSlider);
    azimuthalAngleSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "azimuthAngle", azimuthAngleSlider);

    azimuthDisplacementLabel.setFont(parameterFont);
    addAndMakeVisible(&azimuthDisplacementLabel);
    azimuthDisplacementComboBox.addItem("--", 1);
    azimuthDisplacementComboBox.addItem("Sin", 2);
    azimuthDisplacementComboBox.addItem("Cos", 3);
    azimuthDisplacementComboBox.addItem("Saw", 4);
    azimuthDisplacementComboBox.addItem("Sqr", 5);
    addAndMakeVisible(&azimuthDisplacementComboBox);
    azimuthDisplacementAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, "azimuthDisplacement", azimuthDisplacementComboBox);

    widthSlider.setTextValueSuffix("");
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.attachToComponent(&widthSlider, false);
    addAndMakeVisible(&widthLabel);
    addAndMakeVisible(&widthSlider);
    widthSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "width", widthSlider);

    horizontalDispersionSlider.setTextValueSuffix("");
    horizontalDispersionSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    horizontalDispersionLabel.setText(" Horizontal Dispersion", juce::dontSendNotification);
    horizontalDispersionLabel.attachToComponent(&horizontalDispersionSlider, false);
    addAndMakeVisible(&horizontalDispersionLabel);
    addAndMakeVisible(&horizontalDispersionSlider);
    horizontalDispersionSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "horizontalDispersion", horizontalDispersionSlider);

    elevationAngleSlider.setTextValueSuffix("");
    elevationAngleSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    elevationAngleLabel.setText("Angle", juce::dontSendNotification);
    elevationAngleLabel.attachToComponent(&elevationAngleSlider, false);
    addAndMakeVisible(&elevationAngleLabel);
    addAndMakeVisible(&elevationAngleSlider);
    polarAngleSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "elevationAngle", elevationAngleSlider);

    elevationDisplacementLabel.setFont(parameterFont);
    addAndMakeVisible(&elevationDisplacementLabel);
    elevationDisplacementComboBox.addItem("--", 1);
    elevationDisplacementComboBox.addItem("Sin", 2);
    elevationDisplacementComboBox.addItem("Cos", 3);
    elevationDisplacementComboBox.addItem("Saw", 4);
    elevationDisplacementComboBox.addItem("Sqr", 5);
    addAndMakeVisible(&elevationDisplacementComboBox);
    elevationDisplacementAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, "elevationDisplacement", elevationDisplacementComboBox);

    heightSlider.setTextValueSuffix("");
    heightSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    heightLabel.setText("Height", juce::dontSendNotification);
    heightLabel.attachToComponent(&heightSlider, false);
    addAndMakeVisible(&heightLabel);
    addAndMakeVisible(&heightSlider);
    heightSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "height", heightSlider);

    verticalDispersionSlider.setTextValueSuffix("");
    verticalDispersionSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    verticalDispersionLabel.setText("Vertical Dispersion", juce::dontSendNotification);
    verticalDispersionLabel.attachToComponent(&verticalDispersionSlider, false);
    addAndMakeVisible(&verticalDispersionLabel);
    addAndMakeVisible(&verticalDispersionSlider);
    verticalDispersionSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "verticalDispersion", verticalDispersionSlider);

    ambisonicsOrderLabel.setFont(parameterFont);
    addAndMakeVisible(&ambisonicsOrderLabel);
    ambisonicsOrderComboBox.addItem("0th", 1);
    ambisonicsOrderComboBox.addItem("1st", 4);
    ambisonicsOrderComboBox.addItem("2nd", 9);
    ambisonicsOrderComboBox.addItem("3rd", 16);
    addAndMakeVisible(&ambisonicsOrderComboBox);
    ambisonicsOrderAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, "ambisonicsOrder", ambisonicsOrderComboBox);
    
    ambisonicsNormalisationLabel.setFont(parameterFont);
    addAndMakeVisible(&ambisonicsNormalisationLabel);
    ambisonicsNormalisationComboBox.addItem("SN3D", 1);
    ambisonicsNormalisationComboBox.addItem("N3D", 2);
    addAndMakeVisible(&ambisonicsNormalisationComboBox);
    ambisonicsNormalisationAttachment = std::make_unique<ComboBoxAttachment>(valueTreeState, "ambisonicsNormalisation", ambisonicsNormalisationComboBox);

    gainAttackSlider.setTextValueSuffix(" s");
    gainAttackSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    gainAttackLabel.setText("Attack", juce::dontSendNotification);
    gainAttackLabel.attachToComponent(&gainAttackSlider, false);
    addAndMakeVisible(&gainAttackLabel);
    addAndMakeVisible(&gainAttackSlider);
    gainAttackSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "gainAttack", gainAttackSlider);

    gainDecaySlider.setTextValueSuffix(" s");
    gainDecaySlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    gainDecayLabel.setText("Decay", juce::dontSendNotification);
    gainDecayLabel.attachToComponent(&gainDecaySlider, false);
    addAndMakeVisible(&gainDecayLabel);
    addAndMakeVisible(&gainDecaySlider);
    gainDecaySliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "gainDecay", gainDecaySlider);

    gainSustainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    gainSustainLabel.setText("Sustain", juce::dontSendNotification);
    gainSustainLabel.attachToComponent(&gainSustainSlider, false);
    addAndMakeVisible(&gainSustainLabel);
    addAndMakeVisible(&gainSustainSlider);
    gainSustainSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "gainSustain", gainSustainSlider);

    gainReleaseSlider.setTextValueSuffix(" s");
    gainReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, paramLabelWidth, paramControlHeight);
    gainReleaseLabel.setText("Release", juce::dontSendNotification);
    gainReleaseLabel.attachToComponent(&gainReleaseSlider, false);
    addAndMakeVisible(&gainReleaseLabel);
    addAndMakeVisible(&gainReleaseSlider);
    gainReleaseSliderAttachment = std::make_unique<SliderAttachment>(valueTreeState, "gainRelease", gainReleaseSlider);
    
    setSize(900, 700);
    
    lastSuspended = !getAudioProcessor()->isSuspended();
    timerCallback();
    startTimer(500);
}

void PluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(23, 23, 23));
    g.setFont(20.0);
    g.setColour(juce::Colours::whitesmoke);
    g.drawText("Spectrum", 20, 20, 260, 30, juce::Justification::centred, false);
    g.drawText("Spatial Encoding", 320, 20, 260, 30, juce::Justification::centred, false);
    g.drawText("Output", 620, 20, 260, 30, juce::Justification::centred, false);
    g.setFont(18.0);
    g.drawText("Azimuth", 320, 140, 260, 20, juce::Justification::centred, false);
    g.drawText("Elevation", 320, 400, 260, 20, juce::Justification::centred, false);
    juce::Line<float> lineHorizontalTopFirst(juce::Point<float>(20.0, 65.0), juce::Point<float>(280.0, 65.0));
    juce::Line<float> lineHorizontalTopSecond(juce::Point<float>(320.0, 65.0), juce::Point<float>(580.0, 65.0));
    juce::Line<float> lineHorizontalTopThird(juce::Point<float>(620.0, 65.0), juce::Point<float>(880.0, 65.0));
    juce::Line<float> lineVertFirst(juce::Point<float>(getWidth() / 3.0, 20.0), 
                                    juce::Point<float>(getWidth() / 3, 660.0));
    juce::Line<float> lineVertSecond(juce::Point<float>(getWidth() * 2.0 / 3.0, 20.0), 
                                     juce::Point<float>(getWidth() * 2.0 / 3.0, 660.0));
    juce::Line<float> lineHorizontalBottom(juce::Point<float>(paramControlHeight, getHeight() - paramControlHeight), 
                                           juce::Point<float>(getWidth() - paramControlHeight, getHeight() - paramControlHeight));
    juce::Line<float> lineVertSmallFirst(juce::Point<float>(getWidth() * 0.25, getHeight() - 0.8 * paramControlHeight),
                                         juce::Point<float>(getWidth() * 0.25, getHeight() - 0.2 * paramControlHeight));
    juce::Line<float> lineVertSmallSecond(juce::Point<float>(getWidth() * 0.5, getHeight() - 0.8 * paramControlHeight), 
                                          juce::Point<float>(getWidth() * 0.5, getHeight() - 0.2 * paramControlHeight));
    juce::Line<float> lineVertSmallThird(juce::Point<float>(getWidth() * 0.75, getHeight() - 0.8 * paramControlHeight),
                                         juce::Point<float>(getWidth() * 0.75, getHeight() - 0.2 * paramControlHeight));
    g.drawLine(lineHorizontalBottom, 2.0);
    g.drawLine(lineVertSmallFirst, 1.0);
    g.drawLine(lineVertSmallSecond, 1.0);
    g.drawLine(lineVertSmallThird, 1.0);
    g.setColour(juce::Colour(50, 50, 50));
    g.drawLine(lineHorizontalTopFirst, 1.0);
    g.drawLine(lineHorizontalTopSecond, 1.0);
    g.drawLine(lineHorizontalTopThird, 1.0);
    g.drawLine(lineVertFirst, 1.0);
    g.drawLine(lineVertSecond, 1.0);
}

void PluginAudioProcessorEditor::resized()
{
    auto spectrumLeftBound = 15;
    waveformLabel.setBounds(spectrumLeftBound, 80, paramSliderWidth, paramControlHeight);
    waveformComboBox.setBounds(spectrumLeftBound, 100, 100, paramControlHeight);
    noiseDensityLabel.setBounds(spectrumLeftBound, 140, paramSliderWidth, paramControlHeight);
    noiseDensitySlider.setBounds(spectrumLeftBound, 160, paramSliderWidth, paramControlHeight);
    brightnessLabel.setBounds(spectrumLeftBound, 200, paramSliderWidth, paramControlHeight);
    brightnessSlider.setBounds(spectrumLeftBound, 220, paramSliderWidth, paramControlHeight);
    
    bufferSizeLabel.setBounds(0, getHeight() - paramControlHeight, getWidth() * 0.25, paramControlHeight);
    bufferSizeLabel.setJustificationType(juce::Justification::centred);
    sampleRateLabel.setBounds(getWidth() * 0.25, getHeight() - paramControlHeight, getWidth() * 0.25, paramControlHeight);
    sampleRateLabel.setJustificationType(juce::Justification::centred);
    busLayoutLabel.setBounds(getWidth() * 0.5, getHeight() - paramControlHeight, getWidth() * 0.25, paramControlHeight);
    busLayoutLabel.setJustificationType(juce::Justification::centred);

    auto spatialLeftBound = 320;
    distanceLabel.setBounds(spatialLeftBound, 80, paramSliderWidth, paramControlHeight);
    distanceSlider.setBounds(spatialLeftBound, 100, paramSliderWidth, paramControlHeight);
    azimuthAngleLabel.setBounds(spatialLeftBound, 160, paramSliderWidth, paramControlHeight);
    azimuthAngleSlider.setBounds(spatialLeftBound, 180, paramSliderWidth, paramControlHeight);
    azimuthDisplacementLabel.setBounds(spatialLeftBound, 220, paramSliderWidth, paramControlHeight);
    azimuthDisplacementComboBox.setBounds(spatialLeftBound, 240, 100, paramControlHeight);
    widthLabel.setBounds(spatialLeftBound, 280, paramSliderWidth, paramControlHeight);
    widthSlider.setBounds(spatialLeftBound, 300, paramSliderWidth, paramControlHeight);
    horizontalDispersionLabel.setBounds(spatialLeftBound, 340, paramSliderWidth, paramControlHeight);
    horizontalDispersionSlider.setBounds(spatialLeftBound, 360, paramSliderWidth, paramControlHeight);
    
    elevationAngleLabel.setBounds(spatialLeftBound, 420, paramSliderWidth, paramControlHeight);
    elevationAngleSlider.setBounds(spatialLeftBound, 440, paramSliderWidth, paramControlHeight);
    elevationDisplacementLabel.setBounds(spatialLeftBound, 480, paramSliderWidth, paramControlHeight);
    elevationDisplacementComboBox.setBounds(spatialLeftBound, 500, 100, paramControlHeight);
    heightLabel.setBounds(spatialLeftBound, 540, paramSliderWidth, paramControlHeight);
    heightSlider.setBounds(spatialLeftBound, 560, paramSliderWidth, paramControlHeight);
    verticalDispersionLabel.setBounds(spatialLeftBound, 600, paramSliderWidth, paramControlHeight);
    verticalDispersionSlider.setBounds(spatialLeftBound, 620, paramSliderWidth, paramControlHeight);
    
    auto outputLeftBound = 620;
    ambisonicsOrderLabel.setBounds(outputLeftBound, 80, paramSliderWidth / 2, paramControlHeight);
    ambisonicsOrderComboBox.setBounds(outputLeftBound, 100, 40, paramControlHeight);
    ambisonicsNormalisationLabel.setBounds(outputLeftBound + paramSliderWidth / 2, 80, paramSliderWidth / 2, paramControlHeight);
    ambisonicsNormalisationComboBox.setBounds(outputLeftBound + paramSliderWidth / 2, 100, 40, paramControlHeight);
    gainAttackLabel.setBounds(outputLeftBound, 140, paramSliderWidth, paramControlHeight);
    gainAttackSlider.setBounds(outputLeftBound, 160, paramSliderWidth, paramControlHeight);
    gainDecayLabel.setBounds(outputLeftBound, 200, paramSliderWidth, paramControlHeight);
    gainDecaySlider.setBounds(outputLeftBound, 220, paramSliderWidth, paramControlHeight);
    gainSustainLabel.setBounds(outputLeftBound, 260, paramSliderWidth, paramControlHeight);
    gainSustainSlider.setBounds(outputLeftBound, 280, paramSliderWidth, paramControlHeight);
    gainReleaseLabel.setBounds(outputLeftBound, 320, paramSliderWidth, paramControlHeight);
    gainReleaseSlider.setBounds(outputLeftBound, 340, paramSliderWidth, paramControlHeight);
}

void PluginAudioProcessorEditor::updateGUI()
{
    juce::String bufferSizeString = "Buffer Size: ";
    int bufferSize = processor.getBlockSize();

    if (ceil(log2(bufferSize)) == floor(log2(bufferSize)))
    {
        bufferSizeString += bufferSize;
    } 
    else
    {
        bufferSizeString += bufferSize;
        bufferSizeString += ", but should be power of 2!";
    }

    juce::String sampleRateString = "Sample Rate: ";
    sampleRateString += static_cast<int>(processor.getSampleRate());
    sampleRateString += " Hz";

    juce::String busLayoutString = "Output Channels: ";
    int busLayout = processor.getMainBusNumOutputChannels();
    busLayoutString += busLayout;

    if (busLayout >= 16)
    {
        ambisonicsOrderComboBox.setSelectedId(16, juce::NotificationType::dontSendNotification);
        ambisonicsOrderComboBox.setItemEnabled(9, true);
        ambisonicsOrderComboBox.setItemEnabled(4, true);
        ambisonicsOrderComboBox.setItemEnabled(1, true);
    }
    else if (busLayout >= 9)
    {
        ambisonicsOrderComboBox.setSelectedId(9, juce::NotificationType::dontSendNotification);
        ambisonicsOrderComboBox.setItemEnabled(16, false);
        ambisonicsOrderComboBox.setItemEnabled(9, true);
        ambisonicsOrderComboBox.setItemEnabled(4, true);
        ambisonicsOrderComboBox.setItemEnabled(1, true);
    }
    else if (busLayout >= 4)
    {
        ambisonicsOrderComboBox.setSelectedId(4, juce::NotificationType::dontSendNotification);
        ambisonicsOrderComboBox.setItemEnabled(16, false);
        ambisonicsOrderComboBox.setItemEnabled(9, false);
        ambisonicsOrderComboBox.setItemEnabled(4, true);
        ambisonicsOrderComboBox.setItemEnabled(1, true);
    }
    else 
    {
        ambisonicsOrderComboBox.setSelectedId(1, juce::NotificationType::dontSendNotification);
        ambisonicsOrderComboBox.setItemEnabled(16, false);
        ambisonicsOrderComboBox.setItemEnabled(9, false);
        ambisonicsOrderComboBox.setItemEnabled(4, false);
        ambisonicsOrderComboBox.setItemEnabled(1, true);
    }
    
    bufferSizeLabel.setText(bufferSizeString, juce::NotificationType::dontSendNotification);
    sampleRateLabel.setText(sampleRateString, juce::NotificationType::dontSendNotification);
    busLayoutLabel.setText(busLayoutString, juce::NotificationType::dontSendNotification);
}

void PluginAudioProcessorEditor::timerCallback()
{
    if (processor.isSuspended() != lastSuspended)
    {
        lastSuspended = processor.isSuspended();
        updateGUI();
    }
}
