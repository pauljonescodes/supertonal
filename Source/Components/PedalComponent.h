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

class PedalComponent : public juce::Component
{
public:
	struct ParameterSetting {
		std::string parameterId;
		std::string title;
		std::string suffix;
	};

	explicit PedalComponent(
		juce::AudioProcessorValueTreeState& apvts,
		const std::string& title,
		const std::vector<ParameterSetting>& parameterSettings,
		const std::string& toggleOnParameterId,
		const std::string& additionalToggleParameterId = "") noexcept
	{
		mGroupComponentPtr.reset(new juce::GroupComponent(title, title));
		addAndMakeVisible(mGroupComponentPtr.get());

		for (const auto& setting : parameterSettings) {
			auto sliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
			sliderPtr->setTitle(setting.title);
			sliderPtr->setTextValueSuffix(setting.suffix);
			sliderPtr->setScrollWheelEnabled(false);

			auto attachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
				apvts, setting.parameterId, *sliderPtr);

			auto labelPtr = std::make_unique<juce::Label>();
			labelPtr->setText(setting.title, juce::dontSendNotification);
			labelPtr->attachToComponent(sliderPtr.get(), false);

			addAndMakeVisible(sliderPtr.get());
			addAndMakeVisible(labelPtr.get());

			mSliderPtrs.push_back(std::move(sliderPtr));
			mLabelPtrs.push_back(std::move(labelPtr));
			mAttachmentPtrs.push_back(std::move(attachmentPtr));
		}

		// Toggle Button
		mToggleButtonPtr = std::make_unique<juce::ToggleButton>();
		mToggleButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
			apvts, toggleOnParameterId, *mToggleButtonPtr);
		addAndMakeVisible(mToggleButtonPtr.get());

		if (additionalToggleParameterId != "")
		{
		    mAdditionalToggleButtonPtr = std::make_unique<juce::ToggleButton>();
		    mAdditionalToggleButtonPtr->setButtonText(PluginUtils::toTitleCase(additionalToggleParameterId));
		    mAdditionalToggleButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
		        apvts, additionalToggleParameterId, *mAdditionalToggleButtonPtr);
		    addAndMakeVisible(mAdditionalToggleButtonPtr.get());
		}
	}

	~PedalComponent()
	{
		mSliderPtrs.clear();
		mLabelPtrs.clear();
		mAttachmentPtrs.clear();
		mToggleButtonPtr.reset();
		mToggleButtonAttachmentPtr.reset();
		mGroupComponentPtr.reset();
		mAdditionalToggleButtonPtr.reset();
		mAdditionalToggleButtonAttachmentPtr.reset();
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

		int row1Count = 0;
		for (auto row1Index = 0; row1Index < mSliderPtrs.size(); ++row1Index)
		{
			auto& sliderPtr = mSliderPtrs[row1Index];

			row1FlexBox.items.add(juce::FlexItem(*sliderPtr).withFlex(1).withMaxHeight(128));

			row1Count++;

			if ((mSliderPtrs.size() == 4 && row1Count == 2) || (mSliderPtrs.size() > 4 && row1Count >= 3))
			{
				break;
			}
		}

		mainFlexBox.items.add(juce::FlexItem(row1FlexBox).withFlex(1));
		mainFlexBox.items.add(juce::FlexItem().withFlex(0.1));

		juce::FlexBox row2FlexBox;
		row2FlexBox.flexDirection = juce::FlexBox::Direction::row;
		row2FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
		row2FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
		row2FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
		row2FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

		for (auto row2Index = row1Count; row2Index < mSliderPtrs.size(); ++row2Index)
		{
			row2FlexBox.items.add(juce::FlexItem(*mSliderPtrs[row2Index]).withFlex(1).withMaxHeight(128));
		}

		mainFlexBox.items.add(juce::FlexItem(row2FlexBox).withFlex(1)); // 2/3 of space

		if (mAdditionalToggleButtonPtr != nullptr)
		{
			mainFlexBox.items.add(juce::FlexItem(*mAdditionalToggleButtonPtr).withFlex(0.5));
		}

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
	std::vector<std::unique_ptr<juce::Slider>> mSliderPtrs;
	std::vector<std::unique_ptr<juce::Label>> mLabelPtrs;
	std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> mAttachmentPtrs;

	// Toggle Button
	std::unique_ptr<juce::ToggleButton> mToggleButtonPtr;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mToggleButtonAttachmentPtr;

	// Additional toggle Button
	std::unique_ptr<juce::ToggleButton> mAdditionalToggleButtonPtr;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mAdditionalToggleButtonAttachmentPtr;
};
