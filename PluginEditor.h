#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         private juce::Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    void sliderValueChanged(juce::Slider* slider) override;

    juce::Slider lPreGainSlider;
    juce::Slider rPreGainSlider;
    juce::Slider l2rGainSlider;
    juce::Slider r2lGainSlider;

    juce::Image backGroundImg = juce::ImageCache::getFromMemory(BinaryData::bgscribble_png, BinaryData::bgscribble_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
