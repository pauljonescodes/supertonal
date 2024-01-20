#pragma once

#include <JuceHeader.h>

using namespace juce;

/** Base class to override (mostly) unused juce::AudioProcessor functions */
class JuceProcWrapper : public AudioProcessor
{
public:
    explicit JuceProcWrapper () {}

    

    void prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/) override {}

    //JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Woverloaded-virtual")
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override {}
    //JUCE_END_IGNORE_WARNINGS_GCC_LIKE

    double getTailLengthSeconds() const override { return 0.0; }
    void releaseResources() override {}

    bool acceptsMidi() const final { return false; }
    bool producesMidi() const final { return false; }

    AudioProcessorEditor* createEditor() final { return nullptr; }
    bool hasEditor() const final { return false; }

    int getNumPrograms() final { return 0; }
    void setCurrentProgram (int /*index*/) final {}
    int getCurrentProgram() final { return 0; }

    const String getProgramName (int /*index*/) final { return {}; }
    void changeProgramName (int /*index*/, const String& /*newName*/) final {}

    void getStateInformation (MemoryBlock& /*destData*/) final {}
    void setStateInformation (const void* /*data*/, int /*sizeInBytes*/) final {}

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceProcWrapper)
};