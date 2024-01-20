/*
  ==============================================================================

	PluginLookAndFeel.h
	Created: 9 Jan 2024 10:30:59pm
	Author:  paulm

  ==============================================================================
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

	int getTabButtonBestWidth(juce::TabBarButton& /*tabIndex*/, int /*tabDepth*/) override
	{
		return 150; // This controls the width of the tab
	}

	void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override;


};
