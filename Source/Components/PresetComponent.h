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

class PresetComponent : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
{
public:
	PresetComponent(PluginPresetManager& pm, juce::UndoManager& um) : presetManager(pm), undoManager(um)
	{
		configureButton(undoButton, "Undo");
		configureButton(redoButton, "Redo");
		configureButton(saveButton, "Save");
		configureButton(deleteButton, "Delete");
		configureButton(previousPresetButton, "<");
		configureButton(nextPresetButton, ">");

		presetList.setTextWhenNothingSelected("No Preset Selected");
		presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		addAndMakeVisible(presetList);
		presetList.addListener(this);

		loadPresetList();
	}

	~PresetComponent()
	{
		undoButton.removeListener(this);
		redoButton.removeListener(this);
		saveButton.removeListener(this);
		deleteButton.removeListener(this);
		previousPresetButton.removeListener(this);
		nextPresetButton.removeListener(this);
		presetList.removeListener(this);
	}

	void resized() override
	{
		const auto localBounds = getLocalBounds();
		auto bounds = localBounds;
		//2

		undoButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.1f)));
		redoButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.1f)));
		saveButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.15f)));
		previousPresetButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.1f)));
		presetList.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.3f)));
		nextPresetButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.1f)));
		deleteButton.setBounds(bounds.removeFromLeft(localBounds.proportionOfWidth(0.15f)));
	}
private:
	void buttonClicked(juce::Button* button) override
	{
		if (button == &saveButton)
		{
			fileChooser = std::make_unique<juce::FileChooser>(
				"Please enter the name of the preset to save",
				PluginPresetManager::defaultDirectory,
				"*." + PluginPresetManager::extension
				);
			fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
				{
					const auto resultFile = chooser.getResult();
			presetManager.savePreset(resultFile.getFileNameWithoutExtension());
			loadPresetList();
				});
		}
		if (button == &previousPresetButton)
		{
			const auto index = presetManager.loadPreviousPreset();
			presetList.setSelectedItemIndex(index, juce::dontSendNotification);
		}
		if (button == &nextPresetButton)
		{
			const auto index = presetManager.loadNextPreset();
			presetList.setSelectedItemIndex(index, juce::dontSendNotification);
		}
		if (button == &deleteButton)
		{
			presetManager.deletePreset(presetManager.getCurrentPreset());
			loadPresetList();
		}
		if (button == &undoButton)
		{
			undoManager.undo();
		}
		if (button == &redoButton)
		{
			undoManager.redo();
		}
	}
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
	{
		if (comboBoxThatHasChanged == &presetList)
		{
			presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
		}
	}

	void configureButton(juce::Button& button, const juce::String& buttonText)
	{
		button.setButtonText(buttonText);
		button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
		addAndMakeVisible(button);
		button.addListener(this);
	}

	void loadPresetList()
	{
		presetList.clear(juce::dontSendNotification);
		const auto allPresets = presetManager.getAllPresets();
		const auto currentPreset = presetManager.getCurrentPreset();
		presetList.addItemList(allPresets, 1);
		presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
	}

	PluginPresetManager& presetManager;
	juce::UndoManager& undoManager;
	juce::TextButton undoButton, redoButton, saveButton, deleteButton, previousPresetButton, nextPresetButton;
	juce::ComboBox presetList;
	std::unique_ptr<juce::FileChooser> fileChooser;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetComponent)
};