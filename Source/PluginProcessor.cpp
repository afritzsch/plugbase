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

void MyPluginProcessor::processBlock(juce::AudioBuffer<float>& /*buffer*/, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // TODO: process audio here
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
