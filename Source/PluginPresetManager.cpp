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

#include "PluginPresetManager.h"

const juce::File PluginPresetManager::defaultDirectory{ juce::File::getSpecialLocation(
	juce::File::SpecialLocationType::commonDocumentsDirectory)
		.getChildFile(ProjectInfo::companyName)
		.getChildFile(ProjectInfo::projectName)
};
const juce::String PluginPresetManager::extension{ "preset" };
const juce::String PluginPresetManager::presetNameProperty{ "presetName" };

PluginPresetManager::PluginPresetManager(juce::AudioProcessorValueTreeState& apvts) :
	valueTreeState(apvts)
{
	// Create a default Preset Directory, if it doesn't exist
	if (!defaultDirectory.exists())
	{
		const auto result = defaultDirectory.createDirectory();
		if (result.failed())
		{
			DBG("Could not create preset directory: " + result.getErrorMessage());
			jassertfalse;
		}
	}

	valueTreeState.state.addListener(this);
	currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
}

void PluginPresetManager::savePreset(const juce::String& presetName)
{
	if (presetName.isEmpty())
		return;

	currentPreset.setValue(presetName);
	const auto xml = valueTreeState.copyState().createXml();
	const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
	if (!xml->writeTo(presetFile))
	{
		DBG("Could not create preset file: " + presetFile.getFullPathName());
		jassertfalse;
	}
}

void PluginPresetManager::deletePreset(const juce::String& presetName)
{
	if (presetName.isEmpty())
		return;

	const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
	if (!presetFile.existsAsFile())
	{
		DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
		jassertfalse;
		return;
	}
	if (!presetFile.deleteFile())
	{
		DBG("Preset file " + presetFile.getFullPathName() + " could not be deleted");
		jassertfalse;
		return;
	}
	currentPreset.setValue("");
}

void PluginPresetManager::loadPreset(const juce::String& presetName)
{
	if (presetName.isEmpty())
		return;

	const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
	if (!presetFile.existsAsFile())
	{
		DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
		jassertfalse;
		return;
	}
	// presetFile (XML) -> (ValueTree)
	juce::XmlDocument xmlDocument{ presetFile };
	const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

	valueTreeState.replaceState(valueTreeToLoad);
	currentPreset.setValue(presetName);

}

int PluginPresetManager::loadNextPreset()
{
	const auto allPresets = getAllPresets();
	if (allPresets.isEmpty())
		return -1;
	const auto currentIndex = allPresets.indexOf(currentPreset.toString());
	const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
	loadPreset(allPresets.getReference(nextIndex));
	return nextIndex;
}

int PluginPresetManager::loadPreviousPreset()
{
	const auto allPresets = getAllPresets();
	if (allPresets.isEmpty())
		return -1;
	const auto currentIndex = allPresets.indexOf(currentPreset.toString());
	const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
	loadPreset(allPresets.getReference(previousIndex));
	return previousIndex;
}

juce::StringArray PluginPresetManager::getAllPresets() const
{
	juce::StringArray presets;
	const auto fileArray = defaultDirectory.findChildFiles(
		juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
	for (const auto& file : fileArray)
	{
		presets.add(file.getFileNameWithoutExtension());
	}
	return presets;
}

juce::String PluginPresetManager::getCurrentPreset() const
{
	return currentPreset.toString();
}

void PluginPresetManager::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
{
	currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
}
