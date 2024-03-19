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

#include "PluginAudioProcessor.h"
#include "Components/PresetComponent.h"
#include "PluginLookAndFeel.h"
#include "Components/PreAmpComponent.h"
#include "Components/AmpComponent.h"
#include "Components/CabinetComponent.h"
#include "Components/MixerComponent.h"
#include "Components/TopComponent.h"

class PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginAudioProcessorEditor (PluginAudioProcessor&);
    ~PluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void launchAsyncFileChooserForImpulseResponse();

private:
    PluginAudioProcessor& mProcessorRef;

    PluginLookAndFeel mLookAndFeel;

    std::unique_ptr<juce::FileChooser> mFileChooser;

    juce::AudioProcessorValueTreeState& mAudioProcessorValueTreeState;

    std::unique_ptr<PresetComponent> mPresetComponentPtr;
    std::unique_ptr<TopComponent> mTopComponent;

    std::unique_ptr<juce::TabbedComponent> mTabbedComponentPtr;
    std::unique_ptr<PreAmpComponent> mPedalsComponentPtr;
    std::unique_ptr<AmpComponent> mAmpComponentPtr;
    std::unique_ptr<CabinetComponent> mCabinetComponentPtr;
    std::unique_ptr<ApvtsIdComponent> mMixerApvtsIdComponentPtr;
    std::unique_ptr<ApvtsIdComponent> mHiddenApvtsIdComponentPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
