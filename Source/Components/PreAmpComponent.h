#pragma once

#include <JuceHeader.h>
#include "../PluginPresetManager.h"
#include "../PluginAudioParameters.h"
#include "../PluginUtils.h"
#include "PedalComponent.h"
#include "EquiliserComponent.h"

class PreAmpComponent : public juce::Component
{
public:
	PreAmpComponent(
		juce::AudioProcessorValueTreeState& audioProcessorValueTreeState) :
		mAudioProcessorValueTreeState(audioProcessorValueTreeState)
	{
		mViewportPtr = std::make_unique<juce::Viewport>();
		mContainerPtr = std::make_unique<juce::Component>();

		addAndMakeVisible(mViewportPtr.get());
		mViewportPtr->setViewedComponent(mContainerPtr.get(), false);

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Compressor",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::preCompressorThresholdId, "Threshold", "dB"},
				{ apvts::preCompressorAttackId, "Attack", "ms" },
				{ apvts::preCompressorRatioId, "Ratio", " : 1" },
				{ apvts::preCompressorReleaseId, "Release", "ms" },
				{ apvts::preCompressorGainId, "Makeup", "dB" },
				{ apvts::preCompressorBlendId, "Blend", "" }
		},
			apvts::preCompressorOnId));

		mContainerPtr->addAndMakeVisible(new EquiliserComponent(
			audioProcessorValueTreeState,
			"Equaliser",
			std::vector<EquiliserComponent::ParameterSetting>{
				{ apvts::preEqualiser100GainId, "100hz", "dB"},
				{ apvts::preEqualiser200GainId, "2000hz", "dB" },
				{ apvts::preEqualiser400GainId, "400hz", "dB" },
				{ apvts::preEqualiser800GainId, "800z", "dB" },
				{ apvts::preEqualiser1600GainId, "1600hz", "dB" },
				{ apvts::preEqualiser3200GainId, "3200hz", "dB" },
				{ apvts::preEqualiser6400GainId, "6400hz", "dB" },
				{ apvts::preEqualiserLevelId, "Level", "dB" },
		},
			apvts::preEqualiserOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Screamer",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::tubeScreamerDriveId, "Drive", ""},
				{ apvts::tubeScreamerToneId, "Tone", "" },
				{ apvts::tubeScreamerLevelId, "Level", "" }
		},
			apvts::tubeScreamerOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Driver",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::mouseDriveDistortionId, "Distortion", ""},
				{ apvts::mouseDriveFilterId, "Filter", ""},
				{ apvts::mouseDriveVolumeId, "Volume", "" }
		},
			apvts::mouseDriveOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Delay",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::delayTimeFractionalDenominatorId, "Time", " / beat"},
				{ apvts::delayFeedbackId, "Feedback", "" },
				{ apvts::delayDryWetId, "Mix", "" },
		},
			apvts::delayOnId ));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Chorus",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::chorusFractionOfBeatId, "Time", " / beat"},
				{ apvts::chorusDepthId, "Depth", "" },
				{ apvts::chorusFeedbackId, "Feedback", "" },
				{ apvts::chorusCenterDelayFractionOfBeatId, "Delay", " / beat" },
		},
			apvts::chorusOnId));

		mContainerPtr->addAndMakeVisible(new PedalComponent(
			audioProcessorValueTreeState,
			"Phaser",
			std::vector<PedalComponent::ParameterSetting>{
				{ apvts::phaserRateFractionOfBeatId, "Time", " / beat"},
				{ apvts::phaserDepthId, "Depth", "" },
				{ apvts::phaserCenterFrequencyId, "Center", "" },
				{ apvts::phaserFeedbackId, "Feedback", ""},
		},
			apvts::phaserOnId));

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
			if (index == 1)
			{
				comp->setBounds(xPosition, 10, 480, localBounds.getHeight() - 20);
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
	juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

	std::unique_ptr <juce::Viewport> mViewportPtr;
	std::unique_ptr <juce::Component> mContainerPtr;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PreAmpComponent)
};