#include "PluginAudioProcessorEditor.h"
#include "PluginAudioParameters.h"
#include "PluginUtils.h"

PluginAudioProcessorEditor::PluginAudioProcessorEditor(
	PluginAudioProcessor& p, 
	juce::AudioProcessorValueTreeState& apvts,
	PluginPresetManager& presetManager)
	: 
	AudioProcessorEditor(&p), 
	mProcessorRef(p), 
	mAudioProcessorValueTreeState(apvts)
{
	mViewportPtr = std::make_unique<juce::Viewport>();
	mContainerPtr = std::make_unique<juce::Component>();

	addAndMakeVisible(mViewportPtr.get());
	mViewportPtr->setViewedComponent(mContainerPtr.get(), false);

	mPresetComponentPtr = std::make_unique<PluginPresetComponent>(presetManager);
	mContainerPtr->addAndMakeVisible(mPresetComponentPtr.get());

	for (const auto& parameterId : apvts::ids)
	{
		if (PluginUtils::isToggleId(parameterId))
		{
			const auto& button = new juce::ToggleButton(PluginUtils::toTitleCase(parameterId));
			mComponents.add(button);
			mButtonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(
				mAudioProcessorValueTreeState,
				parameterId,
				*button
			));
			mContainerPtr->addAndMakeVisible(button);
		}
		else
		{
			const auto& slider = new juce::Slider(juce::Slider::Rotary, juce::Slider::TextBoxBelow);
			slider->setTitle(PluginUtils::toTitleCase(parameterId));

			const auto& label = new juce::Label(parameterId, PluginUtils::toTitleCase(parameterId));
			label->attachToComponent(slider, false);

			mComponents.add(slider);
			mSliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(
				mAudioProcessorValueTreeState,
				parameterId,
				*slider
			));
			mContainerPtr->addAndMakeVisible(slider);
		}
	}

	setSize(600, 600);
	setResizable(true, true);
}

PluginAudioProcessorEditor::~PluginAudioProcessorEditor()
{
	mSliderAttachments.clear();
	mButtonAttachments.clear();
	mComponents.clear();
	mViewportPtr.reset();
	mContainerPtr.reset();
	mPresetComponentPtr.reset();
}

void PluginAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginAudioProcessorEditor::resized()
{
	const auto localBounds = getLocalBounds();
	mViewportPtr->setBounds(localBounds);

	auto presetControls = localBounds;

	presetControls.setHeight(50);

	mPresetComponentPtr->setBounds(presetControls);

	int numRows = std::max(1, localBounds.getHeight() / 200);
	int numCols = std::max(1, localBounds.getWidth() / 200);
	
	int buttonWidth = localBounds.getWidth() / numCols;
	int buttonHeight = localBounds.getHeight() / numRows;

	int totalRowsNeeded = (mComponents.size() + numCols - 1) / numCols;
	int totalHeight = ((buttonHeight + 50) * totalRowsNeeded);
	mContainerPtr->setBounds(0, 0, mViewportPtr->getWidth(), totalHeight);

	for (int i = 0; i < mComponents.size(); ++i)
	{
		int row = i / numCols;
		int col = i % numCols;
		mComponents[i]->setBounds(col * buttonWidth, row * buttonHeight + 100, buttonWidth, buttonHeight - 50);
	}
}
