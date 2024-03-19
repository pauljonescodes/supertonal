/*
    This code is part of the Supertonal guitar effects multi-processor.
    Copyright (C) 2023-2024  Paul Jones

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include "PluginAudioProcessorEditor.h"
#include "PluginAudioParameters.h"
#include "PluginUtils.h"

static const std::vector<std::vector<std::string>> sMixerIds = {
{
	apvts::instrumentEqualiserHighPassOnId,
	apvts::instrumentEqualiserHighPassFrequencyId,
	apvts::instrumentEqualiserLowPassFrequencyId,
	apvts::instrumentEqualiserLowPassOnId,
},
{
	apvts::instrumentEqualiserHighPassQualityId,
	apvts::instrumentEqualiserLowPassQualityId,
},
{
	apvts::instrumentEqualiserLowPeakOnId,
	apvts::instrumentEqualiserLowMidPeakOnId,
	apvts::instrumentEqualiserHighMidPeakOnId,
	apvts::instrumentEqualiserHighPeakOnId,
},
{
	apvts::instrumentEqualiserLowPeakFrequencyId,
	apvts::instrumentEqualiserLowMidPeakFrequencyId,
	apvts::instrumentEqualiserHighMidPeakFrequencyId,
	apvts::instrumentEqualiserHighPeakFrequencyId,
},
{
	apvts::instrumentEqualiserLowPeakQualityId,
	apvts::instrumentEqualiserLowMidPeakQualityId,
	apvts::instrumentEqualiserHighMidPeakQualityId,
	apvts::instrumentEqualiserHighPeakQualityId,
},
{
	apvts::instrumentEqualiserLowPeakGainId,
	apvts::instrumentEqualiserLowMidPeakGainId,
	apvts::instrumentEqualiserHighMidPeakGainId,
	apvts::instrumentEqualiserHighPeakGainId,
},
{
	apvts::instrumentCompressorIsOn,
	apvts::instrumentCompressorIsPreEq,
},
{
	apvts::instrumentCompressorInputGain,
	apvts::instrumentCompressorThreshold,
	apvts::instrumentCompressorKnee,
	apvts::instrumentCompressorRatio,
},
{
	apvts::instrumentCompressorIsAutoMakeupOn,
	apvts::instrumentCompressorIsAutoAttackOn,
	apvts::instrumentCompressorIsAutoReleaseOn,
	apvts::instrumentCompressorIsLookaheadOn,
},
{
	apvts::instrumentCompressorMakeup,
	apvts::instrumentCompressorAttack,
	apvts::instrumentCompressorRelease,
	apvts::instrumentCompressorMix,
},
{
	apvts::limiterOnId,
	apvts::limiterThresholdId,
	apvts::limiterReleaseId
}
};

static const std::vector<std::vector<std::string>> sHiddenIds = {
{
	apvts::noiseGateAttackId,
	apvts::noiseGateRatioId,
	apvts::noiseGateReleaseId,
}
};

PluginAudioProcessorEditor::PluginAudioProcessorEditor(PluginAudioProcessor& processorRef)
	:
	AudioProcessorEditor(&processorRef),
	mProcessorRef(processorRef),
	mAudioProcessorValueTreeState(processorRef.getAudioProcessorValueTreeState()),
	mTopComponent(std::make_unique<TopComponent>(processorRef)),
	mTabbedComponentPtr(std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtTop)),
	mPresetComponentPtr(std::make_unique<PresetComponent>(processorRef.getPresetManager(), processorRef.getUndoManager())),
	mPedalsComponentPtr(std::make_unique<PreAmpComponent>(processorRef)),
	mAmpComponentPtr(std::make_unique<AmpComponent>(mAudioProcessorValueTreeState)),
	mFileChooser(std::make_unique<juce::FileChooser>("Select an Impulse Response File", juce::File{}, "*.wav;*.aiff;*.flac")),
	mCabinetComponentPtr(std::make_unique<CabinetComponent>(mAudioProcessorValueTreeState, [this]() {
	this->launchAsyncFileChooserForImpulseResponse();
		})),
	mMixerApvtsIdComponentPtr(std::make_unique<ApvtsIdComponent>(mAudioProcessorValueTreeState, sMixerIds)),
			mHiddenApvtsIdComponentPtr(std::make_unique<ApvtsIdComponent>(mAudioProcessorValueTreeState, sHiddenIds))
{
	setLookAndFeel(&mLookAndFeel);

	addAndMakeVisible(mPresetComponentPtr.get());

	addAndMakeVisible(mTopComponent.get());

	addAndMakeVisible(mTabbedComponentPtr.get());

	mTabbedComponentPtr->addTab("Pedals", juce::Colours::transparentBlack, mPedalsComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Amplifier", juce::Colours::transparentBlack, mAmpComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Cabinet", juce::Colours::transparentBlack, mCabinetComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Mixer", juce::Colours::transparentBlack, mMixerApvtsIdComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Hidden", juce::Colours::transparentBlack, mHiddenApvtsIdComponentPtr.get(), true);

	setSize(800, 800);
	setResizable(true, true);
}

void PluginAudioProcessorEditor::launchAsyncFileChooserForImpulseResponse()
{
	mFileChooser->launchAsync(
		juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
		[this](const juce::FileChooser& chooser)
		{
			mAudioProcessorValueTreeState.state.setProperty(
				juce::Identifier(apvts::impulseResponseFileFullPathNameId),
				chooser.getResult().getFullPathName(), nullptr);
		});
}

PluginAudioProcessorEditor::~PluginAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

void PluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginAudioProcessorEditor::resized()
{
	auto localBounds = getLocalBounds();

	mPresetComponentPtr->setBounds(localBounds.removeFromTop(50));
	mTopComponent->setBounds(localBounds.removeFromTop(150));
	mTabbedComponentPtr->setBounds(localBounds);
}
