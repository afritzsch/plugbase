#include "PluginProcessor.h"
#include "PluginEditor.h"

MyPluginProcessor::MyPluginProcessor()
    : AudioProcessor(BusesProperties()
          .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MyPluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // TODO: add parameters here, e.g.:
    // layout.add(std::make_unique<juce::AudioParameterFloat>(
    //     juce::ParameterID{ "gain", 1 }, "Gain",
    //     juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), 0.0f,
    //     juce::AudioParameterFloatAttributes().withLabel("dB")
    // ));

    return layout;
}

void MyPluginProcessor::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // TODO: initialise DSP here
}

bool MyPluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    return mainOut == layouts.getMainInputChannelSet();
}

void MyPluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, buffer.getNumSamples());

    // TODO: process audio here

    updateWaveform(buffer); // remove if not needed
    updateSpectrum(buffer); // remove if not needed
}

void MyPluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MyPluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* MyPluginProcessor::createEditor()
{
    return new MyPluginEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyPluginProcessor();
}

// ── Visualizers ── (remove if not needed) ─────────────────────────────────────

void MyPluginProcessor::updateWaveform(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    int start1, size1, start2, size2;
    waveformFifo.prepareToWrite(numSamples, start1, size1, start2, size2);
    for (int i = 0; i < size1; ++i) {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getReadPointer(ch)[i];
        waveformBuffer[static_cast<size_t>(start1 + i)] = mono / static_cast<float>(numChannels);
    }
    for (int i = 0; i < size2; ++i) {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getReadPointer(ch)[size1 + i];
        waveformBuffer[static_cast<size_t>(start2 + i)] = mono / static_cast<float>(numChannels);
    }
    waveformFifo.finishedWrite(size1 + size2);
}

void MyPluginProcessor::updateSpectrum(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    for (int i = 0; i < numSamples; ++i) {
        float mono = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            mono += buffer.getReadPointer(ch)[i];
        mono /= (float)numChannels;

        fftBuffer[static_cast<size_t>(fftBufferIndex)] = mono;
        if (++fftBufferIndex == fftSize) {
            fftBufferIndex = 0;
            std::fill(fftBuffer.begin() + fftSize, fftBuffer.end(), 0.0f);
            fft.performFrequencyOnlyForwardTransform(fftBuffer.data());
            juce::SpinLock::ScopedLockType lock(spectrumLock);
            for (int bin = 0; bin < fftSize / 2; ++bin) {
                float mag = fftBuffer[static_cast<size_t>(bin)] / (float)fftSize;
                spectrumBins[static_cast<size_t>(bin)] = juce::Decibels::gainToDecibels(mag, -100.0f);
            }
        }
    }
}
