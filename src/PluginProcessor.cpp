#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    addParameter(
        leftPreGain = new juce::AudioParameterFloat(
            "lpregain",
            "Left Gain",
            -2.0f,
            2.0f,
            1.0f
        )
    );
    addParameter(
        rightPreGain = new juce::AudioParameterFloat(
            "rpregain",
            "Right Gain",
            -2.0f,
            2.0f,
            1.0f
        )
    );
    addParameter(
        leftToRightGain = new juce::AudioParameterFloat(
            "l2rgain",
            "Left-to-Right Gain",
            -2.0f,
            2.0f,
            0.0f
        )
    );
    addParameter(
        rightToLeftGain = new juce::AudioParameterFloat(
            "r2lgain",
            "Right-to-Left Gain",
            -2.0f,
            2.0f,
            0.0f
        )
    );
    addParameter(
        leftPan = new juce::AudioParameterFloat(
            "leftpan",
            "Left Pan",
            -1.0f,
            1.0f,
            -1.0f
        )
    );
    addParameter(
        rightPan = new juce::AudioParameterFloat(
            "rightpan",
            "Right Pan",
            -1.0f,
            1.0f,
            1.0f
        )
    );
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    prevLeftPreGain = *leftPreGain;
    prevRightPreGain = *rightPreGain;
    prevLeftToRightGain = *leftToRightGain;
    prevRightToLeftGain = *rightToLeftGain;
    float prevLeftPan = *leftPan;
    float prevRightPan = *rightPan;
    if(prevLeftPan > 0.0f){
        prevLeftPostGain = prevLeftPan;
        prevLeftToRightPostGain = 0.0f;
    }
    else{
        prevLeftPostGain = 0.0f;
        prevLeftToRightPostGain = -prevLeftPan;
    }

    if(prevRightPan > 0.0f){
        prevRightPostGain = prevRightPan;
        prevRightToLeftPostGain = 0.0f;
    }
    else{
        prevRightPostGain = 0.0f;
        prevRightToLeftPostGain = -prevRightPan;
    }
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

float gainRamp(float& prevGain, float currentGain, int sampleNum){
    if(abs(prevGain - currentGain) < 0.01f){
        return currentGain;
    }

    float step = (currentGain - prevGain)/sampleNum;
    
    prevGain += step;

    return prevGain;
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto sampleNum = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //==================MODEL1=======================================================
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    //stereo channel mixing vars.
    float lPreGain = leftPreGain->get();
    float rPreGain = rightPreGain->get();
    float l2rGain = leftToRightGain->get();
    float r2lGain = rightToLeftGain->get();

    //post-stereo panning vars.
    float lPan = -leftPan->get();
    float rPan = rightPan->get();
    float lPostGain;
    float rPostGain;
    float l2rPostGain;
    float r2lPostGain;

    if(lPan > 0.0f){
        lPostGain = lPan;
        l2rPostGain = 0.0f;
    }
    else{
        lPostGain = 0.0f;
        l2rPostGain = -lPan;
    }

    if(rPan > 0.0f){
        rPostGain = rPan;
        r2lPostGain = 0.0f;
    }
    else{
        rPostGain = 0.0f;
        r2lPostGain = -rPan;
    }

    // float lPreGainStep = (lPreGain - prevLeftPreGain)/sampleNum;
    // float rPreGainStep = (rPreGain - prevLeftPreGain)/sampleNum;
    // float l2rGainStep = (l2rGain - prevLeftToRightGain)/sampleNum;
    // float r2lGainStep = (r2lGain - prevRightToLeftGain)/sampleNum;
    // float lPostGainStep = (lPostGain - prevLeftPostGain)/sampleNum;
    // float rPostGainStep = (rPostGain - prevRightPostGain)/sampleNum;
    // float l2rPostGainStep = (l2rPostGain - prevLeftToRightPostGain)/sampleNum;
    // float r2lPostGainStep = (r2lPostGain - prevRightToLeftPostGain)/sampleNum;

    for(int i = 0; i < buffer.getNumSamples(); i++){
        auto currentLeftSample = leftChannel[i];
        auto currentRightSample = rightChannel[i];

        // leftChannel[i] = lPreGain*currentLeftSample + r2lGain*currentRightSample;
        // rightChannel[i] = rPreGain*currentRightSample + l2rGain*currentLeftSample;
        leftChannel[i] = gainRamp(prevLeftPreGain, lPreGain, sampleNum)*currentLeftSample + gainRamp(prevRightToLeftGain, r2lGain, sampleNum)*currentRightSample;
        rightChannel[i] = gainRamp(prevRightPreGain, rPreGain, sampleNum)*currentRightSample + gainRamp(prevLeftToRightGain, l2rGain, sampleNum)*currentLeftSample;

        currentLeftSample = leftChannel[i];
        currentRightSample = rightChannel[i];

        // leftChannel[i] = lPostGain*currentLeftSample + r2lPostGain*currentRightSample;
        // rightChannel[i] = rPostGain*currentRightSample + l2rPostGain*currentLeftSample;

        leftChannel[i] = gainRamp(prevLeftPostGain, lPostGain, sampleNum)*currentLeftSample + gainRamp(prevRightToLeftPostGain, r2lPostGain, sampleNum)*currentRightSample;
        rightChannel[i] = gainRamp(prevRightPostGain, rPostGain, sampleNum)*currentRightSample + gainRamp(prevLeftToRightPostGain, l2rPostGain, sampleNum)*currentLeftSample;
    }
    //==================MODEL2=======================================================
    //process as buffer to apply gainRamp to to smooth gain change, or implement
    //own gain ramp to model 1(?)
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("pantheonParameterXML"));
    xml->setAttribute("leftpregain", (double) *leftPreGain);
    xml->setAttribute("rightpregain", (double) *rightPreGain);
    xml->setAttribute("lefttorightgain", (double) *leftToRightGain);
    xml->setAttribute("righttoleftgain", (double) *rightToLeftGain);
    xml->setAttribute("leftpan", (double) *leftPan);
    xml->setAttribute("rightpan", (double) *rightPan);
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if(xml.get() != nullptr){
        if(xml->hasTagName("pantheonparameterXML")){
            *leftPreGain = (float)xml->getDoubleAttribute("leftpregain", 1.0f);
            *rightPreGain = (float)xml->getDoubleAttribute("rightpregain", 1.0f);
            *leftToRightGain = (float)xml->getDoubleAttribute("lefttorightgain", 0.0f);
            *rightToLeftGain = (float)xml->getDoubleAttribute("righttoleftgain", 0.0f);
            *leftPan = (float)xml->getDoubleAttribute("leftpan", -1.0f);
            *rightPan = (float)xml->getDoubleAttribute("rightpan", 1.0f);
        }
    }
}
//==============================================================================
void AudioPluginAudioProcessor::setLeftPreGain(float newValue)
{
    *leftPreGain = newValue;
}

void AudioPluginAudioProcessor::setRightPreGain(float newValue)
{
    *rightPreGain = newValue;
}

void AudioPluginAudioProcessor::setLeftToRightGain(float newValue)
{
    *leftToRightGain = newValue;
}

void AudioPluginAudioProcessor::setRightToLeftGain(float newValue)
{
    *rightToLeftGain = newValue;
}

void AudioPluginAudioProcessor::setLeftPan(float newValue){
    *leftPan = newValue;
}

void AudioPluginAudioProcessor::setRightPan(float newValue){
    *rightPan = newValue;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
