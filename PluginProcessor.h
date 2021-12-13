#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void setLeftPreGain(float newValue);
    void setRightPreGain(float newValue);
    void setLeftToRightGain(float newValue);
    void setRightToLeftGain(float newValue);
    void setLeftPan(float newValue);
    void setRightPan(float newValue);
private:
    //==============================================================================
    juce::AudioParameterFloat* leftPreGain;
    juce::AudioParameterFloat* rightPreGain;
    juce::AudioParameterFloat* leftToRightGain;
    juce::AudioParameterFloat* rightToLeftGain;
    juce::AudioParameterFloat* leftPan; //default -1.0 / full left
    juce::AudioParameterFloat* rightPan; //default 1.0 / full right
    //==============================================================================
    // float prevLeftPreGain;
    // float prevRightPreGain;
    // float prevLeftToRightGain;
    // float prevRightToLeftGain;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
