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

	static const std::vector<std::vector<std::string>> apvtsIdRows = {
{
	apvts::inputGainId,
	apvts::overdriveTanhShaperOnId,
	apvts::overdriveClipShaperOnId,
	apvts::overdriveBiasId,
	apvts::overdriveGainId,
	},
{
	apvts::ampCompThresholdId,
	apvts::ampCompAttackId,
	apvts::ampCompRatioId,
	apvts::ampCompReleaseId,
	apvts::ampGainId,
},
{
	apvts::ampHighPassOnId,
	apvts::ampHighPassFrequencyId,
	apvts::ampHighPassQId,
},
{
	apvts::ampMidPeakOnId,
	apvts::ampMidPeakFrequencyId,
	apvts::ampMidPeakQId,
	apvts::ampMidPeakGainId,
},
{
	apvts::ampHighShelfOnId,
	apvts::ampHighShelfFrequencyId,
	apvts::ampHighShelfQId,
	apvts::ampHighShelfGainId,
},
{
	apvts::ampImpulseResponseConvolutionOnId,
	apvts::cabImpulseResponseConvolutionOnId,
	apvts::outputGainId
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
			else
			{
				auto* slider = new juce::Slider(juce::Slider::Rotary, juce::Slider::TextBoxBelow);
				slider->setTitle(PluginUtils::toTitleCase(parameterId));

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

	setSize(750, 750);
	setResizable(true, true);
}

PluginAudioProcessorEditor::~PluginAudioProcessorEditor()
{
	mSliderAttachments.clear();
	mButtonAttachments.clear();
	mComponentRows.clear();
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
	mContainerPtr->setBounds(0, 0, mViewportPtr->getWidth(), totalHeight);

	for (int row = 0; row < mComponentRows.size(); ++row)
	{
		for (int col = 0; col < mComponentRows[row]->size(); ++col)
		{
			(*mComponentRows[row])[col]->setBounds(col * buttonWidth, row * buttonHeight + 100, buttonWidth, buttonHeight - 50);
		}
	}
}
