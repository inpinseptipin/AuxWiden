/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AuxSlider.h"
//==============================================================================
/**
*/
class AuxWidenAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AuxWidenAudioProcessorEditor (AuxWidenAudioProcessor&);
    ~AuxWidenAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AuxWidenAudioProcessor& audioProcessor;
    juce::Image background;
    juce::Rectangle<float> imageBounds;
    AuxSlider outputKnob;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuxWidenAudioProcessorEditor)
};
