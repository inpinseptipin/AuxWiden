/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
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
    AuxPort::Extensions::ParameterMap parameterMap;
private:
    //==============================================================================
    /*******************************************************************************/
    float leftDry = 0.0f;
    float leftSum = 0.0f;
    float rightDry = 0.0f;
    float chain5 = 0.0f;
    float chain5a = 0.0f;
    float chain5b = 0.0f;
    float chain5c = 0.0f;
    /*******************************************************************************/
    AuxPort::Audio::IIR::Butterworth highpassLeft;
    AuxPort::Audio::IIR::Butterworth highpassRight;
    AuxPort::Audio::IIR::ParametricEQ paraEQ;
    AuxPort::Audio::IIR::General allpass;
    AuxPort::Audio::IIR::General highShelf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuxWidenAudioProcessor)
};
