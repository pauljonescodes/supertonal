/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SupertonalAudioProcessorEditor::SupertonalAudioProcessorEditor (SupertonalAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (325, 50);
    mMainComponent.setOnImpulseResponseComboBoxChanged([this](juce::String impulseResponse) -> void {
        audioProcessor.loadNamedResourceImpulseResponse(impulseResponse);
     });
}

SupertonalAudioProcessorEditor::~SupertonalAudioProcessorEditor()
{
}

//==============================================================================
void SupertonalAudioProcessorEditor::paint (juce::Graphics& g)
{
    addAndMakeVisible(mMainComponent);
}

void SupertonalAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
