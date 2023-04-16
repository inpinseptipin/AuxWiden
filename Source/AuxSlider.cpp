/*
  ==============================================================================

    AuxSlider.cpp
    Created: 14 Mar 2023 12:19:14am
    Author:  satya

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AuxSlider.h"

//==============================================================================
AuxSlider::AuxSlider(juce::AudioParameterFloat* sliderParameter)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(slider);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::transparentBlack);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    this->sliderParameter = sliderParameter;
    slider.setRange(this->sliderParameter->range.start,this->sliderParameter->range.end,0.01);
    slider.setValue(*sliderParameter);
    slider.onValueChange = [this]
    {
        *this->sliderParameter = slider.getValue();
        normalizedValue = *this->sliderParameter / (slider.getRange().getEnd() - slider.getRange().getStart());
        repaint();
    };

    
}

AuxSlider::~AuxSlider()
{
}

void AuxSlider::setFeel(const Feel& feel)
{
    this->feel = feel;

    if(this->feel == widenKnob)
        rawKnob = juce::ImageCache::getFromMemory(BinaryData::knob1_png, BinaryData::knob1_pngSize);
    
}

void AuxSlider::setText(const juce::String& text)
{
    this->text = text;
}

void AuxSlider::setPrefix(const juce::String& prefix)
{
    this->prefix = prefix;
}

void AuxSlider::setSuffix(const juce::String& suffix)
{
    this->suffix = suffix;
}

void AuxSlider::setFontSize(const uint32_t& fontSize)
{
    this->fontSize = fontSize;
}

void AuxSlider::setInterval(const float& interval)
{
    this->interval = interval;
    slider.setRange(this->sliderParameter->range.start, this->sliderParameter->range.end, this->interval);

}

void AuxSlider::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    
    g.fillAll(juce::Colours::transparentBlack);



    
   
    
    LookandFeel::drawKnob(g, rawKnob, this->getLocalBounds(), slider,this->feel);
}

void AuxSlider::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto x = this->getWidth() / 10;
    auto y = this->getHeight() / 10;

    slider.setBounds(0, 0, 10 * x, 10 * y);
}



void LookandFeel::drawKnob(juce::Graphics& g, juce::Image& knob,const juce::Rectangle<int>& bounds, juce::Slider& slider, const AuxSlider::Feel& feel)
{
    float rotation = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
    uint32_t noOfFrames = knob.getHeight() / knob.getWidth();
    uint32_t frameID = (uint32_t)ceil(rotation * (static_cast<double>(noOfFrames - 1.0)));
    float radius = bounds.getWidth()/2;

    float centreX = bounds.getCentreX();
    float centreY = bounds.getHeight();
    float rx = centreX - knob.getWidth();
    float ry = centreY - knob.getWidth();
    if(feel == AuxSlider::Feel::widenKnob)
        g.drawImage(knob, bounds.getX(), bounds.getHeight()/2, bounds.getWidth(), bounds.getHeight(), 0, frameID * knob.getWidth(), knob.getWidth(), knob.getWidth(), false);
    


}
