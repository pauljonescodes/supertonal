#include "PluginAudioProcessorEditor.h"
#include "PluginAudioParameters.h"
#include "PluginUtils.h"

PluginAudioProcessorEditor::PluginAudioProcessorEditor(
	PluginAudioProcessor& processorRef,
	juce::AudioProcessorValueTreeState& apvts,
	PluginPresetManager& presetManager)
	:
	AudioProcessorEditor(&processorRef),
	mProcessorRef(processorRef),
	mAudioProcessorValueTreeState(apvts),
	mTabbedComponentPtr(std::make_unique<juce::TabbedComponent>(juce::TabbedButtonBar::Orientation::TabsAtTop)),
	mPresetComponentPtr(std::make_unique<PresetComponent>(presetManager)),
	mPedalsComponentPtr(std::make_unique<PedalsComponent>(mAudioProcessorValueTreeState)),
	mAmpComponentPtr(std::make_unique<AmpComponent>(mAudioProcessorValueTreeState)),
	mFileChooser(std::make_unique<juce::FileChooser>("Select an Impulse Response File", juce::File{}, "*.wav;*.aiff;*.flac")),
	mCabinetComponentPtr(std::make_unique<CabinetComponent>(mAudioProcessorValueTreeState, [this]() {
	this->launchAsyncFileChooserForImpulseResponse();
		})),
		mMixerComponentPtr(std::make_unique<MixerComponent>(mAudioProcessorValueTreeState))
{
	setLookAndFeel(&mLookAndFeel);

	addAndMakeVisible(mPresetComponentPtr.get());

	addAndMakeVisible(mTabbedComponentPtr.get());

	mTabbedComponentPtr->addTab("Pedals", juce::Colours::transparentBlack, mPedalsComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Amplifier", juce::Colours::transparentBlack, mAmpComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Cabinet", juce::Colours::transparentBlack, mCabinetComponentPtr.get(), true);
	mTabbedComponentPtr->addTab("Mixer", juce::Colours::transparentBlack, mMixerComponentPtr.get(), true);

	setSize(750, 750);
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
	localBounds.removeFromTop(5);
	mTabbedComponentPtr->setBounds(localBounds);
}
