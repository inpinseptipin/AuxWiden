/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AuxWidenAudioProcessorEditor::AuxWidenAudioProcessorEditor(AuxWidenAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), outputKnob(audioProcessor.parameterMap.getFloatParameter("outputGain"))
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 300);
    outputKnob.setFeel(AuxSlider::Feel::widenKnob);
    outputKnob.setPrefix("");
    outputKnob.setText("Widen");
    outputKnob.setSuffix("%");
    outputKnob.setInterval(0.01);
    outputKnob.setFontSize(24);
    addAndMakeVisible(outputKnob);


 

}

AuxWidenAudioProcessorEditor::~AuxWidenAudioProcessorEditor()
{
}

//==============================================================================
void AuxWidenAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    background = juce::ImageCache::getFromMemory(BinaryData::srsbackground_jpg, BinaryData::srsbackground_jpgSize);
    imageBounds.setBounds(this->getLocalBounds().getX(), this->getLocalBounds().getY(), this->getLocalBounds().getWidth(), this->getLocalBounds().getHeight());
    g.drawImage(background, imageBounds,juce::RectanglePlacement::stretchToFit,false);

    g.setFont(juce::Font("Papyrus", 48, 1));
    g.setColour(juce::Colours::red);
    g.drawText("AuxWiden", this->getLocalBounds(), juce::Justification::horizontallyCentred, false);
}

void AuxWidenAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto x = this->getLocalBounds().getWidth() / 10;
    auto y = this->getLocalBounds().getHeight() / 10;
  
    outputKnob.setBounds(0, 4*y, 10 * x, 6* y);
}
