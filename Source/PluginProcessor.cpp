/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SupertonalAudioProcessor::SupertonalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mFormatManager.registerBasicFormats();
}

SupertonalAudioProcessor::~SupertonalAudioProcessor()
{
    
}

//==============================================================================
const juce::String SupertonalAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SupertonalAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SupertonalAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SupertonalAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SupertonalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SupertonalAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SupertonalAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SupertonalAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SupertonalAudioProcessor::getProgramName (int index)
{
    return {};
}

void SupertonalAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SupertonalAudioProcessor::loadNamedResourceImpulseResponse(juce::String resourceName) {
    mConvolution.reset();

    int dataSizeInBytes;
    const char* sourceData = BinaryData::getNamedResource(resourceName.getCharPointer(), dataSizeInBytes);

    if (sourceData != nullptr) {
        auto memoryInputStream = std::make_unique<juce::MemoryInputStream>(sourceData, dataSizeInBytes, false);
        juce::AudioFormatReader* reader = mFormatManager.createReaderFor(std::move(memoryInputStream));

        if (reader)
        {
            juce::AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);
            reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

            mConvolution.loadImpulseResponse(
                std::move(buffer),
                reader->sampleRate,
                juce::dsp::Convolution::Stereo::no,
                juce::dsp::Convolution::Trim::no,
                juce::dsp::Convolution::Normalise::no);
        }
    }
}

void SupertonalAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    mConvolution.prepare(spec);
}

void SupertonalAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::dsp::AudioBlock<float> block(buffer);
    mConvolution.process(juce::dsp::ProcessContextReplacing<float>(block));
}

void SupertonalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SupertonalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

//==============================================================================
bool SupertonalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SupertonalAudioProcessor::createEditor()
{
    return new SupertonalAudioProcessorEditor (*this);
}

//==============================================================================
void SupertonalAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SupertonalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SupertonalAudioProcessor();
}
