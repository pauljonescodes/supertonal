#pragma once

#include <JuceHeader.h>
#include "../PluginAudioProcessor.h"

class TunerPedalComponent : public juce::Component, private juce::Timer
{
public:

	explicit TunerPedalComponent(
		PluginAudioProcessor& processorRef,
		const std::string& toggleOnParameterId) : mAudioProcessorRef(processorRef) 
	{
		mGroupComponentPtr.reset(new juce::GroupComponent("tuner", "Tuner"));
		addAndMakeVisible(*mGroupComponentPtr);

		mPitchLabel = std::make_unique<juce::Label>("Pitch", "Pitch");
		addAndMakeVisible(*mPitchLabel);

		mRemainderLabel = std::make_unique<juce::Label>("Remainder", "Remainder");
		addAndMakeVisible(*mRemainderLabel);

		juce::Timer::startTimerHz(4);

		// Toggle Button
		mToggleButtonPtr = std::make_unique<juce::ToggleButton>();
		mToggleButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
			processorRef.getAudioProcessorValueTreeState(), toggleOnParameterId, *mToggleButtonPtr);
		addAndMakeVisible(*mToggleButtonPtr);
	}

	~TunerPedalComponent()
	{
		mToggleButtonPtr.reset();
		mToggleButtonAttachmentPtr.reset();
		mGroupComponentPtr.reset();
	}

	void resized() override
	{
		auto bounds = getLocalBounds().reduced(10);
		mGroupComponentPtr->setBounds(bounds);

		juce::FlexBox mainFlexBox;
		mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
		mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
		mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
		mainFlexBox.alignContent = juce::FlexBox::AlignContent::stretch;

		mainFlexBox.items.add(juce::FlexItem().withFlex(0.3));

		juce::FlexBox row1FlexBox;
		row1FlexBox.flexDirection = juce::FlexBox::Direction::row;
		row1FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
		row1FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
		row1FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
		row1FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

		mainFlexBox.items.add(juce::FlexItem(row1FlexBox).withFlex(1));
		mainFlexBox.items.add(juce::FlexItem().withFlex(0.1));

		juce::FlexBox row2FlexBox;
		row2FlexBox.flexDirection = juce::FlexBox::Direction::row;
		row2FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
		row2FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
		row2FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
		row2FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

		row2FlexBox.items.add(juce::FlexItem(*mPitchLabel).withFlex(1).withMaxHeight(128));
		row2FlexBox.items.add(juce::FlexItem(*mRemainderLabel).withFlex(1).withMaxHeight(128));

		mainFlexBox.items.add(juce::FlexItem(row2FlexBox).withFlex(1)); // 2/3 of space

		// FlexBox for the bottom 1/3 where toggle button will be
		juce::FlexBox row4FlexBox;
		row4FlexBox.flexDirection = juce::FlexBox::Direction::row;
		row4FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
		row4FlexBox.alignItems = juce::FlexBox::AlignItems::center;
		row4FlexBox.alignContent = juce::FlexBox::AlignContent::center;

		auto toggleButtonSize = juce::jmin(bounds.getWidth(), bounds.getHeight() / 3) / 4;
		juce::FlexItem toggleFlexItem(toggleButtonSize, toggleButtonSize, *mToggleButtonPtr);
		row4FlexBox.items.add(toggleFlexItem);

		mainFlexBox.items.add(juce::FlexItem(row4FlexBox).withFlex(1));

		// Perform layout for the main FlexBox
		mainFlexBox.performLayout(bounds);
	}

private:
	std::unique_ptr<juce::GroupComponent> mGroupComponentPtr;
	std::unique_ptr<juce::Label> mPitchLabel;
	std::unique_ptr<juce::Label> mRemainderLabel;
	PluginAudioProcessor& mAudioProcessorRef;

	float mLastPitch;

	void timerCallback()
	{
		const auto newPitch = mAudioProcessorRef.getPitch();
		if (newPitch != mLastPitch)
		{
			mLastPitch = newPitch;

			const auto result = PluginUtils::getNoteNameAndCentsFromFrequency(mLastPitch);
			
			mPitchLabel->setText(std::get<0>(result), juce::dontSendNotification);
			mRemainderLabel->setText(juce::String::formatted("%.1f Hz", std::get<1>(result)), juce::dontSendNotification);
		}
	}

	// Toggle Button
	std::unique_ptr<juce::ToggleButton> mToggleButtonPtr;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mToggleButtonAttachmentPtr;
};