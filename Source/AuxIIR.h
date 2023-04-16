#ifndef AUXPORT_IIR_H
#define AUXPORT_IIR_H
/*
  ==============================================================================

    IIR.h
    Created: 12 Mar 2023 5:03:32pm
    Author:  Satyarth Arora

  ==============================================================================
*/
#include "JuceHeader.h"
#define _USE_MATH_DEFINES  // To enable Math constants in MSVC 
#include <math.h>
namespace AuxPort
{
    namespace IIR
    {
        enum class Type
        {
            Lowpass,Highpass,Bandpass,Allpass,HighShelf
        };
        class Butterworth
        {
        public:
            Butterworth();
            ~Butterworth() = default;
            Butterworth(const Butterworth& butterworth) = default;
            void prepareToPlay(float fc, float q, float sampleRate,Type type = Type::Lowpass);
            float processSample(float sample);
        private:
            std::vector<float> coefficients;
            enum index
            {
                a0,a1,a2,b1,b2
            };
            float z1;
            float z2;
            const float sqrt2 = sqrtf(2);
            float output;
        };

        class ParametricEQ
        {
        public:
            ParametricEQ();
            ~ParametricEQ() = default;
            ParametricEQ(const ParametricEQ& peq) = default;
            void prepareToPlay(float fc, float q, float boost, float sampleRate);
            float processSample(float sample);
        private:
            std::vector<float> coefficients;
            enum index
            {
                a0, a1, a2, b1, b2
            };
            float z1;
            float z2;
            const float sqrt2 = sqrtf(2);
            float output;
        };
        
        
        class General
        {
        public:
            General();
            ~General() = default;
            General(const General& general) = default;
            void prepareToPlay(float fc, float q,float boost,float sampleRate,Type type = Type::Lowpass);
            float processSample(float sample);
        private:
            std::vector<float> coefficients;
            enum index
            {
                a0, a1, a2, b1, b2,c0,d0
            };
            float z1;
            float z2;
            const float sqrt2 = sqrtf(2);
            float output;
        };


        class Engine
        {
        public:
            enum Filter
            {
                ButterLPF6dB,ButterHPF6dB,ButterLPF12dB,ButterHPF12dB,ButterBPF6dB,ButterBPF12dB,ParametricEQ,GeneralAllpass,GeneralHighShelf
            };
            Engine() = default;
            Engine(Filter filter);
            ~Engine() = default;
            Engine(const Engine& engine) = default;
            void prepareToPlay(float fc, float q, float sampleRate,float boost = 0,uint32_t channels = 2);
            void process(juce::AudioBuffer<float>& buffer);
        private:
            std::vector<Butterworth> butter1;
            std::vector<Butterworth> butter2;
            std::vector<AuxPort::IIR::ParametricEQ> paramEQ;
            std::vector<General> general1;
            Filter filter;
        };


        

    }
}

#endif