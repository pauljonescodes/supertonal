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

class ApvtsIdComponent : public juce::Component
{
public:
	ApvtsIdComponent(
		juce::AudioProcessorValueTreeState& audioProcessorValueTreeState,
		const std::vector<std::vector<std::string>> apvtsIdRows) :
		mAudioProcessorValueTreeState(audioProcessorValueTreeState)
	{
		mViewportPtr = std::make_unique<juce::Viewport>();
		mContainerPtr = std::make_unique<juce::Component>();

		addAndMakeVisible(mViewportPtr.get());
		mViewportPtr->setViewedComponent(mContainerPtr.get(), false);

		for (int row = 0; row < apvtsIdRows.size(); ++row)
		{
			const auto& colIds = apvtsIdRows[row];

			for (const auto& parameterId : colIds)
			{

				if (mComponentRows.size() < row + 1)
				{
					mComponentRows.add(new juce::OwnedArray<juce::Component>());
				}

				if (PluginUtils::isToggleId(parameterId))
				{
					auto* button = new juce::ToggleButton(PluginUtils::toTitleCase(parameterId));
					mComponentRows[row]->add(button);
					mButtonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(
						mAudioProcessorValueTreeState,
						parameterId,
						*button
					));
					mContainerPtr->addAndMakeVisible(button);
				}
				else if (PluginUtils::isWaveshaperId(parameterId))
				{
					auto* comboBox = new juce::ComboBox(PluginUtils::toTitleCase(parameterId));
					for (int waveshaperIndex = 0; waveshaperIndex < apvts::waveShaperIds.size(); waveshaperIndex++) {
							comboBox->addItem(apvts::waveShaperIds.at(waveshaperIndex), waveshaperIndex + 1);
						}
					mComponentRows[row]->add(comboBox);
					mComboBoxAttachments.add(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
						mAudioProcessorValueTreeState,
						parameterId,
						*comboBox
					));
					mContainerPtr->addAndMakeVisible(comboBox);
				}
				else
				{
					auto* slider = new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
					slider->setTitle(PluginUtils::toTitleCase(parameterId));
					slider->setScrollWheelEnabled(false);

					auto* label = new juce::Label(parameterId, PluginUtils::toTitleCase(parameterId));
					label->attachToComponent(slider, false);

					mComponentRows[row]->add(slider);
					mSliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(
						mAudioProcessorValueTreeState,
						parameterId,
						*slider
					));
					mContainerPtr->addAndMakeVisible(slider);
				}
			}

		}
	};

	~ApvtsIdComponent()
	{

		mSliderAttachments.clear();
		mButtonAttachments.clear();
		mComboBoxAttachments.clear();

		mComponentRows.clear();
		mViewportPtr.reset();
		mContainerPtr.reset();
	};

	void paint(juce::Graphics& g) override
	{
		g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	};

	void resized() override
	{
		const auto localBounds = getLocalBounds();
		mViewportPtr->setBounds(localBounds);

		int numRows = mComponentRows.size();
		int numCols = 0;

		for (int i = 0; i < mComponentRows.size(); ++i) {
			int currentSize = mComponentRows[i]->size();

			if (currentSize > numCols) {
				numCols = currentSize;
			}
		}

		int buttonWidth = localBounds.getWidth() / numCols;
		int buttonHeight = buttonWidth;

		int totalHeight = ((buttonHeight + 12.5) * numRows);
		mContainerPtr->setBounds(0, 0, mViewportPtr->getMaximumVisibleWidth() - 8, totalHeight);

		for (int row = 0; row < mComponentRows.size(); ++row)
		{
			for (int col = 0; col < mComponentRows[row]->size(); ++col)
			{
				(*mComponentRows[row])[col]->setBounds(col * buttonWidth, row * buttonHeight + 50, buttonWidth, buttonHeight - 50);
			}
		}
	};

private:
	juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

	std::unique_ptr <juce::Viewport> mViewportPtr;
	std::unique_ptr <juce::Component> mContainerPtr;

	juce::OwnedArray<juce::OwnedArray<juce::Component>> mComponentRows;

	juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> mButtonAttachments;
	juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> mSliderAttachments;
	juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> mComboBoxAttachments;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApvtsIdComponent)
};