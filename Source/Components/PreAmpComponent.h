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

#pragma once

#include <JuceHeader.h>
#include "../PluginPresetManager.h"
#include "../PluginAudioParameters.h"
#include "../PluginUtils.h"
#include "PedalComponent.h"
#include "EquiliserComponent.h"
#include "DelayComponent.h"
#include "TunerComponent.h"

class PreAmpComponent : public juce::Component
{
public:
	PreAmpComponent(
		PluginAudioProcessor& audioProcessor) :
		mAudioProcessor(audioProcessor)
	{
		mViewportPtr = std::make_unique<juce::Viewport>();
		mContainerPtr = std::make_unique<juce::Component>();

		addAndMakeVisible(mViewportPtr.get());
		mViewportPtr->setViewedComponent(mContainerPtr.get(), false);

		mContainerPtr->addAndMakeVisible(new TunerPedalComponent(
			mAudioProcessor,
			apvts::tunerOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Compressor",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::preCompressorThresholdId, "Threshold", "dB"},
				{ apvts::preCompressorAttackId, "Attack", "ms" },
				{ apvts::preCompressorRatioId, "Ratio", " : 1" },
				{ apvts::preCompressorReleaseId, "Release", "ms" },
				{ apvts::preCompressorGainId, "Makeup", "dB" },
				{ apvts::preCompressorDryWetMixId, "Mix", "" }
		},
			apvts::preCompressorOnId,
					apvts::preCompressorAutoMakeUpOnId));

		mContainerPtr->addAndMakeVisible(new EqualiserComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Equaliser",
			std::vector<EqualiserComponent::ParameterSetting>{
				{ apvts::preEqualiser100GainId, "100hz", "dB"},
				{ apvts::preEqualiser200GainId, "200hz", "dB" },
				{ apvts::preEqualiser400GainId, "400hz", "dB" },
				{ apvts::preEqualiser800GainId, "800z", "dB" },
				{ apvts::preEqualiser1600GainId, "1600hz", "dB" },
				{ apvts::preEqualiser3200GainId, "3200hz", "dB" },
				{ apvts::preEqualiser6400GainId, "6400hz", "dB" },
				{ apvts::preEqualiserLevelId, "Level", "dB" },
		},
			apvts::preEqualiserOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Screamer",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::tubeScreamerDriveId, "Drive", ""},
				{ apvts::tubeScreamerToneId, "Tone", "" },
				{ apvts::tubeScreamerLevelId, "Level", "" }
		},
			apvts::tubeScreamerOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Driver",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::mouseDriveDistortionId, "Distortion", ""},
				{ apvts::mouseDriveFilterId, "Filter", "" },
				{ apvts::mouseDriveVolumeId, "Volume", "" }
		},
			apvts::mouseDriveOnId));

		mContainerPtr->addAndMakeVisible(new DelayComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Delay",
			apvts::delayLeftPerBeatId,
			apvts::delayRightPerBeatId,
			apvts::delayLeftMillisecondId,
			apvts::delayRightMillisecondId,
			apvts::delayHighPassFrequencyId,
			apvts::delayLowPassFrequencyId,
			apvts::delayFeedbackId,
			apvts::delayDryWetId,
			apvts::delayIsSyncedId,
			apvts::delayLinkedId,
			apvts::delayOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Chorus",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::chorusDelayId, "Delay", ""},
				{ apvts::chorusDepthId, "Depth", "" },
				{ apvts::chorusFrequencyId, "Frequency", "" },
				{ apvts::chorusWidthId, "Width", "" },
		},
			apvts::chorusOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Phaser",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::phaserDepthId, "Depth", "" },
				{ apvts::phaserWidthId, "Width", "" },
				{ apvts::phaserFeedbackId, "Feedback", "" },
				{ apvts::phaserFrequencyId, "Frequency", "" },
		},
			apvts::phaserIsOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Flanger",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::flangerDelayId, "Delay", "" },
				{ apvts::flangerDepthId, "Depth", "" },
				{ apvts::flangerFeedbackId, "Feedback", "" },
				{ apvts::flangerFrequencyId, "Frequency", "" },
				{ apvts::flangerWidthId, "Width", "" },
		},
			apvts::flangerOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Bit Crusher",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::bitCrusherSampleRateId, "Sample rate", " hz"},
				{ apvts::bitCrusherBitDepthId, "Bit depth", "bits" },
		},
			apvts::bitCrusherOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			mAudioProcessor.getAudioProcessorValueTreeState(),
			"Reverb",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::roomSizeId, "Size", ""},
				{ apvts::roomDampingId, "Damping", "" },
				{ apvts::roomWidthId, "Width", "" },
				{ apvts::roomMixId, "Mix", "" },
		},
			apvts::roomOnId));

		resized();
	};

	~PreAmpComponent()
	{
		mViewportPtr.reset();
		mContainerPtr.reset();
	};

	void paint(juce::Graphics& g) override
	{
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	};

	void resized() override {
		const auto localBounds = getLocalBounds();
		mViewportPtr->setBounds(localBounds);

		int xPosition = 0; 
		int index = 0;
		for (auto* comp : mContainerPtr->getChildren()) {
			if (index == 2)
			{
				comp->setBounds(xPosition, 10, 560, localBounds.getHeight() - 20);
			}
			else
			{
				comp->setBounds(xPosition, 10, 320, localBounds.getHeight() - 20);
			}
			xPosition += comp->getWidth(); // 10 is the margin
			index++;
		}

		mContainerPtr->setBounds(0, 0, xPosition, mViewportPtr->getMaximumVisibleHeight() - 8);
	}

private:
	PluginAudioProcessor& mAudioProcessor;

	std::unique_ptr <juce::Viewport> mViewportPtr;
	std::unique_ptr <juce::Component> mContainerPtr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreAmpComponent)
};