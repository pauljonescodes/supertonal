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
	apvts::stage1OnId,
	apvts::stage1InputGainId,
	apvts::stage1WaveShaperId,
	apvts::stage1OutputGainId,
	apvts::stage1DryWetId,
},
{
	apvts::stage2OnId,
	apvts::stage2InputGainId,
	apvts::stage2WaveShaperId,
	apvts::stage2OutputGainId,
	apvts::stage2DryWetId,
},
{
	apvts::stage3OnId,
	apvts::stage3InputGainId,
	apvts::stage3WaveShaperId,
	apvts::stage3OutputGainId,
	apvts::stage3DryWetId,
},
{
	apvts::stage4OnId,
	apvts::stage4InputGainId,
	apvts::stage4WaveShaperId,
	apvts::stage4OutputGainId,
	apvts::stage4DryWetId,
},
{
	apvts::modeId,
	apvts::biasComponentId,
},
{
	apvts::highPassOnId,
	apvts::highPassFrequencyId,
	apvts::highPassQId,
},
{
	apvts::midPeakOnId,
	apvts::midPeakFrequencyId,
	apvts::midPeakQId,
	apvts::midPeakGainId,
},
{
	apvts::highShelfOnId,
	apvts::highShelfFrequencyId,
	apvts::highShelfQId,
	apvts::highShelfGainId,
},
{
	apvts::compressorThresholdId,
	apvts::compressorAttackId,
	apvts::compressorRatioId,
	apvts::compressorReleaseId,
	apvts::compressorGainId,
},
{
	apvts::chorusRateId,
	apvts::chorusDepthId,
	apvts::chorusCenterDelayId,
	apvts::chorusFeedbackId,
	apvts::chorusMixId,
},
{
	apvts::phaserRateId,
	apvts::phaserDepthId,
	apvts::phaserCenterFrequencyId,
	apvts::phaserFeedbackId,
	apvts::phaserMixId,
},
{
	apvts::reverbRoomSize,
	apvts::reverbDamping,
	apvts::reverbWetLevel,
	apvts::reverbDryLevel,
	apvts::reverbWidth,
},
{
	apvts::cabinetImpulseResponseConvolutionOnId,
	apvts::outputGainId,
	apvts::limiterThresholdId,
	apvts::limiterReleaseId,
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
			else if (PluginUtils::isWaveshaperId(parameterId) || PluginUtils::isModeId(parameterId))
			{
				auto* comboBox = new juce::ComboBox(PluginUtils::toTitleCase(parameterId));
				if (PluginUtils::isWaveshaperId(parameterId))
				{
					for (int waveshaperIndex = 0; waveshaperIndex < apvts::waveShaperIds.size(); waveshaperIndex++) {
						comboBox->addItem(apvts::waveShaperIds.at(waveshaperIndex), waveshaperIndex + 1);
					}
				}
				else if (PluginUtils::isModeId(parameterId))
				{
					for (int modeIndex = 0; modeIndex < apvts::modeIds.size(); modeIndex++) {
						comboBox->addItem(apvts::modeIds.at(modeIndex), modeIndex + 1);
					}
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
	mComboBoxAttachments.clear();
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
