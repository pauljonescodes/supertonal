#pragma once

#include <JuceHeader.h>
#include "../PluginPresetManager.h"
#include "../PluginAudioParameters.h"
#include "../PluginUtils.h"

class CabinetComponent : public juce::Component, private juce::Button::Listener, private juce::ValueTree::Listener, public juce::AudioProcessorValueTreeState::Listener
{
public:
    CabinetComponent(
                     juce::AudioProcessorValueTreeState& audioProcessorValueTreeState,
                     std::function<void()> onClickedSelectFile
                     ) :
    mAudioProcessorValueTreeState(audioProcessorValueTreeState),
    mImpulseResponseFileLabelPtr(std::make_unique<juce::Label>()),
    mViewportPtr(std::make_unique<juce::Viewport>()),
    mContainerPtr(std::make_unique<juce::Component>()),
    mOnClickedSelectFile(onClickedSelectFile)
    {
		 
		addAndMakeVisible(mViewportPtr.get());
		mViewportPtr->setViewedComponent(mContainerPtr.get(), false);

		mAudioProcessorValueTreeState.state.addListener(this);
		mAudioProcessorValueTreeState.addParameterListener(apvts::cabinetImpulseResponseConvolutionIndexId, this);

		const auto impulseResponseFullPathName = mAudioProcessorValueTreeState.state.getProperty(
			juce::String(apvts::impulseResponseFileFullPathNameId),
			juce::String()).toString();

		if (impulseResponseFullPathName.length() > 0)
		{
			mImpulseResponseFileLabelPtr->setText(impulseResponseFullPathName, juce::dontSendNotification);
		}
		else
		{
			const int index = *mAudioProcessorValueTreeState.getRawParameterValue(apvts::cabinetImpulseResponseConvolutionIndexId);
			mImpulseResponseFileLabelPtr->setText(apvts::cabIds.at(index), juce::dontSendNotification);
		}
		
		addAndMakeVisible(mImpulseResponseFileLabelPtr.get());

		static const std::vector<std::vector<std::string>> apvtsIdRows = {
{
	apvts::cabinetImpulseResponseConvolutionOnId,
	apvts::cabinetImpulseResponseConvolutionIndexId,
	apvts::cabinetImpulseResponseConvolutionFileId,
	apvts::cabinetGainId
}
		};

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
				else if (parameterId == apvts::cabinetImpulseResponseConvolutionFileId)
				{
					auto* selectFileButton = new juce::TextButton();
					selectFileButton->setButtonText("Select IR File");
					addAndMakeVisible(selectFileButton);
					selectFileButton->addListener(this);
					mComponentRows[row]->add(selectFileButton);
				}
				else if (parameterId == apvts::cabinetImpulseResponseConvolutionIndexId)
				{
					auto* comboBox = new juce::ComboBox(PluginUtils::toTitleCase(parameterId));
					for (int cabIndex = 0; cabIndex < apvts::cabIds.size(); cabIndex++) {
						comboBox->addItem(apvts::cabIds.at(cabIndex), cabIndex + 1);
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

	~CabinetComponent()
	{
		mSliderAttachments.clear();
		mButtonAttachments.clear();
		mComboBoxAttachments.clear();
		mComponentRows.clear();
		mViewportPtr.reset();
		mContainerPtr.reset();
		mAudioProcessorValueTreeState.state.removeListener(this);
		mAudioProcessorValueTreeState.removeParameterListener(apvts::cabinetImpulseResponseConvolutionIndexId, this);
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

		int contentHeight = ((buttonHeight + 12.5) * numRows) + 50;
		const int maximumVisibleWidth = mViewportPtr->getMaximumVisibleWidth() - 8;
		mContainerPtr->setBounds(0, 0, maximumVisibleWidth, contentHeight);

		mImpulseResponseFileLabelPtr->setBounds(0, 0, maximumVisibleWidth, 50);

		for (int row = 0; row < mComponentRows.size(); ++row)
		{
			for (int col = 0; col < mComponentRows[row]->size(); ++col)
			{
				(*mComponentRows[row])[col]->setBounds(col * buttonWidth, row * buttonHeight + 100, buttonWidth, buttonHeight - 50);
			}
		}
	};

	void buttonClicked(juce::Button* button) override
	{
		// Call the provided file selection function
		if (mOnClickedSelectFile)
		{
			mOnClickedSelectFile();
		}
	}

	void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
	{
		if (property == juce::Identifier(apvts::impulseResponseFileFullPathNameId))
		{
			juce::String newFilePath = treeWhosePropertyHasChanged.getProperty(property).toString();
			mImpulseResponseFileLabelPtr->setText(newFilePath.isNotEmpty() ? newFilePath : "Default cab loaded", juce::dontSendNotification);


			if (newFilePath.length() > 0)
			{
				mImpulseResponseFileLabelPtr->setText(newFilePath, juce::dontSendNotification);
			}
			else
			{
				const int index = *mAudioProcessorValueTreeState.getRawParameterValue(apvts::cabinetImpulseResponseConvolutionIndexId);
				mImpulseResponseFileLabelPtr->setText(apvts::cabIds.at(index), juce::dontSendNotification);
			}
		}
	}

	void parameterChanged(const juce::String& parameterID, float newValue) override
	{
		if (parameterID.toStdString() == apvts::cabinetImpulseResponseConvolutionIndexId)
		{
			const int index = *mAudioProcessorValueTreeState.getRawParameterValue(apvts::cabinetImpulseResponseConvolutionIndexId);
			mImpulseResponseFileLabelPtr->setText(apvts::cabIds.at(index), juce::dontSendNotification);
		}

		resized();
	}

private:
	juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

	std::unique_ptr<juce::Label> mImpulseResponseFileLabelPtr;

	std::unique_ptr <juce::Viewport> mViewportPtr;
	std::unique_ptr <juce::Component> mContainerPtr;

	juce::OwnedArray<juce::OwnedArray<juce::Component>> mComponentRows;

	std::function<void()> mOnClickedSelectFile;

	juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> mButtonAttachments;
	juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> mSliderAttachments;
	juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> mComboBoxAttachments;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabinetComponent)
};
