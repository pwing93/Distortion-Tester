
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionTesterAudioProcessorEditor::DistortionTesterAudioProcessorEditor (DistortionTesterAudioProcessor& p, juce::AudioProcessorValueTreeState& state)
    : AudioProcessorEditor (&p), audioProcessor (p), params(state)
{
    setSize (600, 400);
    
    setLookAndFeel(&otherLookAndFeel);
    
    addSlider("type", "Type", typeSlider, typeLabel, typeAttachment);
    addSlider("input", "Input", inputSlider, inputLabel, inputAttachment);
    addSlider("output", "Output", outputSlider, outputLabel, outputAttachment);
    addSlider("thresh", "Thresh", threshSlider, threshLabel, threshAttachment);
    addSlider("mix", "Mix", mixSlider, mixLabel, mixAttachment);
    
    typeSlider.setSliderStyle(juce::Slider::Rotary);
    inputSlider.setSliderStyle(juce::Slider::Rotary);
    outputSlider.setSliderStyle(juce::Slider::Rotary);
    threshSlider.setSliderStyle(juce::Slider::Rotary);
    mixSlider.setSliderStyle(juce::Slider::Rotary);
}

DistortionTesterAudioProcessorEditor::~DistortionTesterAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void DistortionTesterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void DistortionTesterAudioProcessorEditor::resized()
{
    typeSlider.setBounds(25, 25, 150, 150);
    inputSlider.setBounds(225, 25, 150, 150);
    threshSlider.setBounds(425, 25, 150, 150);
    mixSlider.setBounds(125, 225, 150, 150);
    outputSlider.setBounds(325, 225, 150, 150);
}
