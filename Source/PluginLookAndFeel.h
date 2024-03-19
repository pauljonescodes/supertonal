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

#define _USE_MATH_DEFINES
#include <math.h>

static juce::Colour g_primaryColour = juce::Colours::indianred;
static juce::Colour g_secondaryColour = juce::Colour(34, 34, 34);
static int g_fontSize = 22;

class PluginLookAndFeel : public juce::LookAndFeel_V4
{
public:
	PluginLookAndFeel()
	{
		setColourScheme({
			juce::Colour::fromRGB(12, 12, 12), // windowBackground - Black
			g_secondaryColour, // widgetBackground - Dark grey
			juce::Colours::grey, // menuBackground - Grey
			juce::Colours::white, // outline - White
			juce::Colours::white, // defaultText - White
			g_primaryColour, // defaultFill - Pink (Hot Pink)
			juce::Colours::white, // highlightedText - White
			g_primaryColour, // highlightedFill - Deep Pink
			juce::Colours::white // menuText - White
		});

		setColour(juce::TabbedComponent::outlineColourId, juce::Colours::grey);
		setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
		//setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);

		static juce::Typeface::Ptr customTypeface = juce::Typeface::createSystemTypefaceFor(
			BinaryData::WorkSansRegular_ttf,
			BinaryData::WorkSansRegular_ttfSize);
		juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(customTypeface);
	}

	void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
		float sliderPosProportional, float rotaryStartAngle,
		float rotaryEndAngle, juce::Slider&) override;

	void drawButtonBackground(juce::Graphics& g, juce::Button& button,
		const juce::Colour& backgroundColour,
		bool isMouseOverButton, bool isButtonDown) override;

	int getDefaultScrollbarWidth() override;
	bool areScrollbarButtonsVisible() override { return false; }
	void drawScrollbarButton(juce::Graphics& g, juce::ScrollBar& bar,
		int width, int height, int buttonDirection,
		bool isScrollbarVertical, bool isMouseOverButton,
		bool isButtonDown) override {}

	void drawCornerResizer(juce::Graphics& g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/) override;
	void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& cb) override;
	void positionComboBoxText(juce::ComboBox& cb, juce::Label& labelToPosition) override;
	void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;
	void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
	void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override;
	void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
	juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
	juce::Font getLabelFont(juce::Label&) override;
	
	void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{
		using namespace juce; 
		auto tickWidth = g_fontSize * 1.1f;

		drawTickBox(g, button, 4.0f, ((float)button.getHeight() - tickWidth) * 0.5f,
			tickWidth, tickWidth,
			button.getToggleState(),
			button.isEnabled(),
			shouldDrawButtonAsHighlighted,
			shouldDrawButtonAsDown);

		g.setColour(button.findColour(ToggleButton::textColourId));
		g.setFont(g_fontSize);

		if (!button.isEnabled())
			g.setOpacity(0.5f);

		g.drawFittedText(button.getButtonText(),
			button.getLocalBounds().withTrimmedLeft(roundToInt(tickWidth) + 10)
			.withTrimmedRight(2),
			Justification::centredLeft, 10);
	}

	int getTabButtonBestWidth(juce::TabBarButton& button, int tabDepth) override
	{
		return button.getTabbedButtonBar().getWidth() / button.getTabbedButtonBar().getNumTabs();
	}

	void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override;


};
