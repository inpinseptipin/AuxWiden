/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AuxIIR.h"
#include "AuxParam.h"

//==============================================================================
/**
*/
class AuxWidenAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    AuxWidenAudioProcessor();
    ~AuxWidenAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    AuxPort::ParameterMap parameterMap;
private:
    //==============================================================================
    
    /*******************************************************************************/
    juce::AudioParameterFloat* AFc;
    juce::AudioParameterFloat* outputGain;

    /*******************************************************************************/

    float leftDry;
    float leftSum;
    float rightDry;
    float chain5;
    float chain5a;
    float chain5b;
    float chain5c;
    /*******************************************************************************/
    AuxPort::IIR::Butterworth highpassLeft;
    AuxPort::IIR::Butterworth highpassRight;
    AuxPort::IIR::ParametricEQ paraEQ;
    AuxPort::IIR::General allpass;
    AuxPort::IIR::General highShelf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuxWidenAudioProcessor)
};
