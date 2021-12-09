#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(420, 620);

    lPreGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    lPreGainSlider.setRange(-2.0, 2.0);
    lPreGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    lPreGainSlider.setPopupDisplayEnabled(true, false, this);
    lPreGainSlider.setTextValueSuffix(" x");
    lPreGainSlider.setValue(1.0);

    rPreGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    rPreGainSlider.setRange(-2.0, 2.0);
    rPreGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    rPreGainSlider.setPopupDisplayEnabled(true, false, this);
    rPreGainSlider.setTextValueSuffix(" x");
    rPreGainSlider.setValue(1.0);

    l2rGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    l2rGainSlider.setRange(-2.0, 2.0);
    l2rGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    l2rGainSlider.setPopupDisplayEnabled(true, false, this);
    l2rGainSlider.setTextValueSuffix(" x");
    l2rGainSlider.setValue(0.0);

    r2lGainSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    r2lGainSlider.setRange(-2.0, 2.0);
    r2lGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    r2lGainSlider.setPopupDisplayEnabled(true, false, this);
    r2lGainSlider.setTextValueSuffix(" x");
    r2lGainSlider.setValue(0.0);
    
    lPreGainSlider.addListener(this);
    rPreGainSlider.addListener(this);
    l2rGainSlider.addListener(this);
    r2lGainSlider.addListener(this);

    addAndMakeVisible(r2lGainSlider);
    addAndMakeVisible(lPreGainSlider);
    addAndMakeVisible(rPreGainSlider);
    addAndMakeVisible(l2rGainSlider);

    setResizable(false, false);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.drawImageAt(backGroundImg, 0, 0);

    g.setColour (juce::Colours::black);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    r2lGainSlider.setBounds(95, 130, 20, 361);
    lPreGainSlider.setBounds(165, 130, 20, 361);
    rPreGainSlider.setBounds(235, 130, 20, 361);
    l2rGainSlider.setBounds(305, 130, 20, 361);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider){
    if(slider == &lPreGainSlider){
        processorRef.setLeftPreGain(lPreGainSlider.getValue());
    }
    else if(slider == &rPreGainSlider){
        processorRef.setRightPreGain(rPreGainSlider.getValue());
    }
    else if(slider == &l2rGainSlider){
        processorRef.setLeftToRightGain(l2rGainSlider.getValue());
    }
    else if(slider == & r2lGainSlider){
        processorRef.setRightToLeftGain(r2lGainSlider.getValue());
    }
}
