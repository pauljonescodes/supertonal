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
},
{
	apvts::chorusMixId,
	apvts::phaserMixId,
}
};

PluginAudioProcessorEditor::PluginAudioProcessorEditor(
	PluginAudioProcessor& processorRef,
	juce::AudioProcessorValueTreeState& apvts,
	PluginPresetManager& presetManager)
	:
	AudioProcessorEditor(&processorRef),
	mProcessorRef(processorRef),
	mAudioProcessorValueTreeState(apvts),
	mTopComponent(std::make_unique<TopComponent>(mAudioProcessorValueTreeState)),
	mTabbedComponentPtr(std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtTop)),
	mPresetComponentPtr(std::make_unique<PresetComponent>(presetManager)),
	mPedalsComponentPtr(std::make_unique<PreAmpComponent>(mAudioProcessorValueTreeState)),
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
