
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionTesterAudioProcessor::DistortionTesterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
,
state(*this, nullptr, juce::Identifier("params"),
    {
        std::make_unique<juce::AudioParameterInt>("type", "Type", 1, 3, 1),
    
        std::make_unique<juce::AudioParameterFloat> ("thresh", "Threshold", juce::NormalisableRange<float> (0.0f, 0.25F, 0.01f), 0.0f),
        
        std::make_unique<juce::AudioParameterFloat> ("mix", "Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f),
        
        std::make_unique<juce::AudioParameterFloat> ("input", "Input", juce::NormalisableRange<float> (0.0f, 3.0f, 0.01f), 1.0f),
        
        std::make_unique<juce::AudioParameterFloat> ("output", "Output", juce::NormalisableRange<float> (0.0f, 3.0f, 0.01f), 1.0f)
    }
)
#endif
{
}

DistortionTesterAudioProcessor::~DistortionTesterAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionTesterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionTesterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionTesterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionTesterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionTesterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionTesterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistortionTesterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionTesterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionTesterAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionTesterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionTesterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    typeParam = state.getRawParameterValue("type");
    inputParam = state.getRawParameterValue("input");
    outputParam = state.getRawParameterValue("output");
    threshParam = state.getRawParameterValue("thresh");
    mixParam = state.getRawParameterValue("mix");
}

void DistortionTesterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionTesterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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
#endif

void DistortionTesterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            //Input Gain Stage
            channelData[i] = channelData[i] * *inputParam;
            
            auto input = channelData[i] * 10;
            auto cleanOut = channelData[i];
            
            //Hard Clipping Algorithm
            if (*typeParam == 1)
            {
                if (input > *threshParam)
                {
                    input = *threshParam;
                }
                
                else if (input < -*threshParam)
                {
                    input = -*threshParam;
                }
                
                else
                {
                    input = input;
                }
            }
            
            //Soft Clipping Algorithm
            if (*typeParam == 2)
            {
                if (input > *threshParam)
                {
                    input = 1.0f - expf(-input);
                }
                
                else
                {
                    input = -1.0f + expf(input);
                }
            }
            
            //Half-Wave Rectifier Algorithm
            if (*typeParam == 3)
            {
                if (input > *threshParam)
                {
                    input = input;
                }
                
                else
                {
                    input = 0;
                }
            }

            //Formula for taking "Wet" affected Input signal and mixing back with "Dry" unaffected signal
            channelData[i] = ((1 - *mixParam) * cleanOut) + (*mixParam * input);
            
            //Output Make-up Gain stage
            channelData[i] = channelData[i] * *outputParam;
        }
    }
}

//==============================================================================
bool DistortionTesterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionTesterAudioProcessor::createEditor()
{
    return new DistortionTesterAudioProcessorEditor (*this, state);
}

//==============================================================================
void DistortionTesterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto stateTree = state.copyState();
    std::unique_ptr<juce::XmlElement> xml (stateTree.createXml());
    copyXmlToBinary (*xml, destData);
}

void DistortionTesterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(state.state.getType()))
    {
        state.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionTesterAudioProcessor();
}
