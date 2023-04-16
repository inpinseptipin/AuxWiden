/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AuxWidenAudioProcessor::AuxWidenAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),parameterMap(this)
#endif
{
    
    AFc = new juce::AudioParameterFloat("aFc", "aFc", 0, 1, 0.5);

    outputGain = new juce::AudioParameterFloat("outputGain", "outputGain", 0, 1, 0.5);
    
    parameterMap.addParameter(outputGain);
    parameterMap.addParameter(AFc);
    parameterMap.save();
    
}

AuxWidenAudioProcessor::~AuxWidenAudioProcessor()
{
}

//==============================================================================
const juce::String AuxWidenAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AuxWidenAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AuxWidenAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AuxWidenAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AuxWidenAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AuxWidenAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AuxWidenAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AuxWidenAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AuxWidenAudioProcessor::getProgramName (int index)
{
    return {};
}

void AuxWidenAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AuxWidenAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    highpassLeft.prepareToPlay(500, 1, this->getSampleRate(), AuxPort::IIR::Type::Highpass);
    highpassRight.prepareToPlay(500, 1, this->getSampleRate(), AuxPort::IIR::Type::Highpass);
    paraEQ.prepareToPlay(2500, 2.5, 1, sampleRate);
    allpass.prepareToPlay(20 + static_cast<uint32_t>(*AFc * (19980)), 10.0,1,sampleRate, AuxPort::IIR::Type::Allpass);
    highShelf.prepareToPlay(5000, 2.0, 1, this->getSampleRate(), AuxPort::IIR::Type::HighShelf);
}

void AuxWidenAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AuxWidenAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void AuxWidenAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    


    highpassLeft.prepareToPlay(2000, 7.44, this->getSampleRate(), AuxPort::IIR::Type::Highpass);
    highpassRight.prepareToPlay(2000, 7.44, this->getSampleRate(), AuxPort::IIR::Type::Highpass);

    paraEQ.prepareToPlay(150, 1, 12, this->getSampleRate());
    highShelf.prepareToPlay(5000, 1.0,6, this->getSampleRate(),AuxPort::IIR::Type::HighShelf);
    

    allpass.prepareToPlay(this->getSampleRate()/2, 1, this->getSampleRate(), 1, AuxPort::IIR::Type::Allpass);


    for (uint32_t i = 0; i < buffer.getNumSamples(); i++)
    {
        leftDry = buffer.getSample(0,i);
        rightDry = buffer.getSample(1, i);
        
        leftSum =  (leftDry + rightDry);
        
        chain5 = highpassLeft.processSample(leftDry);
        chain5 -= highpassLeft.processSample(rightDry);
        chain5a = paraEQ.processSample(chain5);
        chain5b = allpass.processSample(chain5);
        chain5c = highShelf.processSample(chain5);

        leftSum *= (1 - *outputGain/2);
        

        buffer.setSample(0, i,  leftSum + *outputGain * (chain5a + chain5b + chain5c));
        buffer.setSample(1, i,  leftSum + *outputGain * -((chain5a + chain5b + chain5c)));
    }
}

//==============================================================================
bool AuxWidenAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AuxWidenAudioProcessor::createEditor()
{
    return new AuxWidenAudioProcessorEditor (*this);
}

//==============================================================================
void AuxWidenAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AuxWidenAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AuxWidenAudioProcessor();
}
