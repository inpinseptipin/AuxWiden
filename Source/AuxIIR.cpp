/*
  ==============================================================================

    IIR.cpp
    Created: 12 Mar 2023 5:03:32pm
    Author:  Satyarth Arora

  ==============================================================================
*/

#include "AuxIIR.h"

AuxPort::IIR::Butterworth::Butterworth()
{
    z1 = 0;
    z2 = 0;
    output = 0;
    coefficients.resize(5);
    for (uint32_t i = 0; i < coefficients.size(); i++)
        coefficients[i] = 0;
}

void AuxPort::IIR::Butterworth::prepareToPlay(float fc, float q, float sampleRate, Type type)
{
    if (type == Type::Lowpass)
    {
        float theta_c = M_PI * fc / sampleRate;
        float c = 1.0 / std::tanf(theta_c);
        coefficients[index::a0] = 1.0 / (1.0 + sqrt2 * c + powf(c, 2));
        coefficients[index::a1] = 2.0 * coefficients[index::a0];
        coefficients[index::a2] = coefficients[index::a0];
        coefficients[index::b1] = 2.0 * coefficients[index::a0] * (1.0 - powf(c, 2));
        coefficients[index::b2] = coefficients[a0] * (1.0 - sqrt2 * c + powf(c, 2));
    }

    if (type == Type::Highpass)
    {
        float theta_c = M_PI * fc / sampleRate;
        float c = 1.0 / std::tan(theta_c);
        coefficients[index::a0] = 1.0 / (1.0 + sqrt2 * c + powf(c, 2));
        coefficients[index::a1] = -2.0 * coefficients[index::a0];
        coefficients[index::a2] = coefficients[index::a0];
        coefficients[index::b1] = 2.0 * coefficients[index::a0] * (powf(c, 2) - 1.0);
        coefficients[index::b2] = coefficients[index::a0] * (1.0 - sqrt2 * c + powf(c, 2));
    }

    if (type == Type::Bandpass)
    {
        float theta_c = 2.0 * M_PI * fc / sampleRate;
        float bandwidth = fc / q;
        float delta_c = M_PI * bandwidth / sampleRate;
        if (delta_c >= 0.95 * M_PI / 2.0)
            delta_c = 0.95 * M_PI / 2.0;
        float c = 1.0 / tanf(delta_c);
        float d = 2.0 * cosf(theta_c);

        coefficients[index::a0] = 1.0 / (1.0 + c);
        coefficients[index::a1] = 0.0;
        coefficients[index::a2] = -coefficients[index::a0];
        coefficients[index::b1] = -coefficients[index::a0] * (c * d);
        coefficients[index::b2] = coefficients[index::a0] * (c - 1.0);
    }
}

float AuxPort::IIR::Butterworth::processSample(float sample)
{
    output = sample * coefficients[index::a0] + z1;
    z1 = sample * coefficients[index::a1] + z2 - coefficients[index::b1] * output;
    z2 = sample * coefficients[index::a2] - coefficients[index::b2] * output;
    return output;
}

AuxPort::IIR::Engine::Engine(Filter filter)
{
    this->filter = filter;
    if (filter == ButterLPF6dB || filter == ButterHPF6dB || filter == ButterBPF6dB)
        butter1.resize(2);
    if (filter == ButterLPF12dB || filter == ButterHPF12dB || filter == ButterBPF12dB)
    {
        butter1.resize(2);
        butter2.resize(2);
    }
    if (filter == ParametricEQ)
        paramEQ.resize(2);
    if (filter == GeneralAllpass || filter == GeneralHighShelf)
        general1.resize(2);

}

void AuxPort::IIR::Engine::prepareToPlay(float fc, float q, float sampleRate,float boost, uint32_t channels)
{
    if (filter == ButterLPF6dB)
    {
        if (channels > butter1.size())
            butter1.resize(channels);
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Lowpass);
        }
    }


    if (filter == ButterHPF6dB)
    {
        if (channels > butter1.size())
            butter1.resize(channels);
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Highpass);
        }
    }

    if (filter == ButterBPF6dB)
    {
        if (channels > butter1.size())
            butter1.resize(channels);
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Bandpass);
        }
    }


    if (filter == ButterHPF12dB)
    {
        if (channels > butter1.size())
        {
            butter1.resize(channels);
            butter2.resize(channels);
        }
            
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Highpass);
            butter2[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Highpass);
        }
    }

    if (filter == ButterLPF12dB)
    {
        if (channels > butter1.size())
        {
            butter1.resize(channels);
            butter2.resize(channels);
        }
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Lowpass);
            butter2[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Lowpass);
        }
    }

    if (filter == ButterBPF12dB)
    {
        if (channels > butter1.size())
        {
            butter1.resize(channels);
            butter2.resize(channels);
        }
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Bandpass);
            butter2[i].prepareToPlay(fc, q, sampleRate, AuxPort::IIR::Type::Bandpass);
        }
    }

    if (filter == ParametricEQ)
    {
        if (channels > paramEQ.size())
            paramEQ.resize(channels);
        for (uint32_t i = 0; i < paramEQ.size(); i++)
            paramEQ[i].prepareToPlay(fc, q, boost, sampleRate);
    }

    if (filter == GeneralAllpass)
    {
        if (channels > general1.size())
            general1.resize(channels);
        for (uint32_t i = 0; i < general1.size(); i++)
            general1[i].prepareToPlay(fc, q, sampleRate, boost,AuxPort::IIR::Type::Allpass);
    }

    if (filter == GeneralHighShelf)
    {
        if (channels > general1.size())
            general1.resize(channels);
        for (uint32_t i = 0; i < general1.size(); i++)
            general1[i].prepareToPlay(fc, q, sampleRate, boost,AuxPort::IIR::Type::HighShelf);
    }

}

void AuxPort::IIR::Engine::process(juce::AudioBuffer<float>& buffer)
{
    for (uint32_t i = 0; i < buffer.getNumSamples(); i++)
    {
        for (uint32_t j = 0; j < buffer.getNumChannels(); j++)
        {
            if (filter == ButterLPF6dB || filter == ButterHPF6dB || filter == ButterBPF6dB)
                buffer.setSample(j,i,butter1[j].processSample(buffer.getSample(j, i)));
            if (filter == ButterLPF12dB || filter == ButterHPF12dB || filter == ButterBPF12dB)
                buffer.setSample(j, i, butter2[j].processSample(butter1[j].processSample(buffer.getSample(j, i))));
            if (filter == ParametricEQ)
                buffer.setSample(j, i, paramEQ[j].processSample(buffer.getSample(j, i)));
        }
    }
}

AuxPort::IIR::ParametricEQ::ParametricEQ()
{
    z1 = 0;
    z2 = 0;
    output = 0;
    coefficients.resize(5);
    for (uint32_t i = 0; i < coefficients.size(); i++)
        coefficients[i] = 0;
}

void AuxPort::IIR::ParametricEQ::prepareToPlay(float fc, float q, float boost,float sampleRate)
{
    float k = tanf(M_PI * fc / sampleRate);
    float vo = powf(10.0, boost / 20.0);
    bool toBoost = boost >= 0 ? true : false;

    float d0 = 1.0 + (1.0 / q) * k + powf(k, 2);
    float e0 = 1.0 + (1.0 / (vo * q)) * k + powf(k, 2);
    float alpha = 1.0 + (vo / q) * k + powf(k, 2);
    float beta = 2.0 * (powf(k, 2) - 1.0);
    float gamma = 1.0 - (vo / q) * k + powf(k, 2);
    float delta = 1.0 - (1.0 / q) * k + powf(k, 2);
    float eta = 1.0 - (1.0 / (vo * q)) * k + powf(k, 2);

    coefficients[index::a0] = toBoost ? alpha / d0 : d0 / e0;
    coefficients[index::a1] = toBoost ? beta / d0 : beta / e0;
    coefficients[index::a2] = toBoost ? gamma / d0 : delta / e0;
    coefficients[index::b1] = toBoost ? beta / d0 : beta / e0;
    coefficients[index::b2] = toBoost ? delta / d0 : eta / e0;
}

float AuxPort::IIR::ParametricEQ::processSample(float sample)
{
    output = sample * coefficients[index::a0] + z1;
    z1 = sample * coefficients[index::a1] + z2 - coefficients[index::b1] * output;
    z2 = sample * coefficients[index::a2] - coefficients[index::b2] * output;
    return output;
}

AuxPort::IIR::General::General()
{
    z1 = 0;
    z2 = 0;
    output = 0;
    coefficients.resize(7);
    for (uint32_t i = 0; i < coefficients.size(); i++)
        coefficients[i] = 0;
}

void AuxPort::IIR::General::prepareToPlay(float fc, float q,float boost,float sampleRate, Type type)
{
    if (type == Type::Allpass)
    {
        float theta_c = 2.0 * M_PI * fc / sampleRate;
        float bandwidth = fc / q;
        float argTan = M_PI * bandwidth / sampleRate;
        if (argTan >= 0.95 * M_PI / 2.0)
            argTan = 0.95 * M_PI / 2.0;

        float alphaNumerator = tanf(argTan) - 1.0;
        float alphaDenominator = tanf(argTan) + 1.0;
        float alpha = alphaNumerator / alphaDenominator;
        float beta = -cosf(theta_c);

        coefficients[index::a0] = -alpha;
        coefficients[index::a1] = beta * (1.0 - alpha);
        coefficients[index::a2] = 1.0;
        coefficients[index::b1] = beta * (1.0 - alpha);
        coefficients[index::b2] = -alpha;

    }

    if (type == Type::HighShelf)
    {
        float theta_c = 2.0 * M_PI * fc / sampleRate;
        float mu = powf(10.0, boost / 20.0);
        float beta = (1.0 + mu) / 4.0;
        float delta = beta * tanf(theta_c / 2.0);
        float gamma = (1.0 - delta) / (1.0 + delta);

        
        coefficients[index::a0] = (1.0 + gamma) / 2.0;
        coefficients[index::a1] = -coefficients[index::a0];
        coefficients[index::a2] = 0.0;
        coefficients[index::b1] = -gamma;
        coefficients[index::b2] = 0.0;

        coefficients[index::c0] = mu - 1.0;
        coefficients[index::d0] = 1.0;
    }

}

float AuxPort::IIR::General::processSample(float sample)
{
    output = sample * coefficients[index::a0] + z1;
    z1 = sample * coefficients[index::a1] + z2 - coefficients[index::b1] * output;
    z2 = sample * coefficients[index::a2] - coefficients[index::b2] * output;
    return coefficients[index::d0] * sample + coefficients[index::c0] * output;
}
