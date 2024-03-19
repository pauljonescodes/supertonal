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

#include "PluginLookAndFeel.h"

void PluginLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider)
{
    const auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    const auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
    const auto text = slider.findColour(juce::Slider::textBoxTextColourId);

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto centre = bounds.getCentre();

    const auto tickHeight = 6;
    const auto tickWidth = 40;

    auto radius = juce::jmin(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);

    g.setColour(text);
    if (radius > tickWidth * 2.0f + 10.0f)
    {
        const auto xLeft = int(bounds.getX());
        const auto xRight = int(bounds.getRight() - tickWidth);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.5f)), int(centre.getX() - tickWidth / 2), int(bounds.getY()), tickWidth, tickHeight, juce::Justification::centred, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.375f)), xLeft, int(bounds.getY()), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.25f)), xLeft, int(juce::jmap(0.33f, float(bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.125f)), xLeft, int(juce::jmap(0.66f, float(bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.0f)), xLeft, int(bounds.getBottom() - tickHeight), tickWidth, tickHeight, juce::Justification::left, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.625f)), xRight, int(bounds.getY()), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.75f)), xRight, int(juce::jmap(0.33f, float(bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(0.875f)), xRight, int(juce::jmap(0.66f, float(bounds.getY()), bounds.getBottom() - tickHeight)), tickWidth, tickHeight, juce::Justification::right, 1);
        g.drawFittedText(slider.getTextFromValue(slider.proportionOfLengthToValue(1.0f)), xRight, int(bounds.getBottom() - tickHeight), tickWidth, tickHeight, juce::Justification::right, 1);

        bounds.removeFromTop(tickHeight + 4.0f);
        bounds.reduce(tickWidth, 0.0f);
    }

    radius = juce::jmin(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f);
    centre = bounds.getCentre();

    if (radius > 50.0f)
    {
        for (int i = 0; i < 9; ++i)
        {
            const auto angle = juce::jmap(i / 8.0f, rotaryStartAngle, rotaryEndAngle);
            const auto point = centre.getPointOnCircumference(radius - 2.0f, angle);
            g.fillEllipse(point.getX() - 3, point.getY() - 3, 6, 6);
        }
        radius -= 10.0f;
    }

    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(4.0f, radius * 0.5f);
    auto arcRadius = radius - lineW;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);

    g.setColour(outline);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));

    auto knobRadius = std::max(radius - 3.0f * lineW, 10.0f);
    {
        juce::Graphics::ScopedSaveState saved(g);
        if (slider.isEnabled())
        {
            juce::ColourGradient fillGradient(outline.brighter(), centre.getX() + lineW * 2.0f, centre.getY() - lineW * 4.0f, outline, centre.getX() + knobRadius, centre.getY() + knobRadius, true);
            g.setGradientFill(fillGradient);
        }
        g.fillEllipse(centre.getX() - knobRadius, centre.getY() - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    }

    knobRadius = std::max(knobRadius - 4.0f, 10.0f);
    g.setColour(outline.brighter());
    g.drawEllipse(centre.getX() - knobRadius, centre.getY() - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 2.0f);

    if (slider.isEnabled() && arcRadius > 10.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
                               bounds.getCentreY(),
                               arcRadius,
                               arcRadius,
                               0.0f,
                               rotaryStartAngle,
                               toAngle,
                               true);

        g.setColour(fill);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
    }

    juce::Path p;
    p.startNewSubPath(centre.getPointOnCircumference(knobRadius - lineW, toAngle));
    p.lineTo(centre.getPointOnCircumference((knobRadius - lineW) * 0.6f, toAngle));
    g.strokePath(p, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void PluginLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button,
                                             const juce::Colour &backgroundColour,
                                             bool isMouseOverButton, bool isButtonDown)
{
    const auto baseColour = backgroundColour
                                .withMultipliedAlpha(button.isEnabled() ? 0.75f : 0.3f);

    const auto width = float(button.getWidth());
    const auto height = float(button.getHeight());

    if (width > 0 && height > 0)
    {
        const float cornerSize = 1.f;

        juce::Path outline;
        outline.addRoundedRectangle(0.f, 0.f,
                                    width, height, cornerSize, cornerSize,
                                    true, true, true, true);

        g.setGradientFill(juce::ColourGradient(
            baseColour.darker(0.1f), 0.f, height / 2 - 2,
            baseColour.darker(0.2f), 0.f, height / 2 + 2, false));

        g.fillPath(outline);

        if (isButtonDown || isMouseOverButton)
        {
            g.setColour(baseColour.brighter(isButtonDown ? 0.1f : 0.01f));
            g.fillPath(outline);
        }

        constexpr auto outlineAlpha = 0.05f;

        g.setColour(juce::Colours::white.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f));

        g.setColour(juce::Colours::black.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f),
                     juce::AffineTransform::translation(0.f, 1.f).scaled(1.f, (height + 2.f) / height));
    }
}

int PluginLookAndFeel::getDefaultScrollbarWidth()
{
    return 8;
}

void PluginLookAndFeel::drawCornerResizer(juce::Graphics &g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/)
{
    using namespace juce;

    const float lineThickness = jmin(w, h) * 0.05f;
    const Colour col1(this->findColour(ResizableWindow::backgroundColourId).darker(0.2f));
    const Colour col2(this->findColour(ResizableWindow::backgroundColourId).brighter(0.04f));

    for (float i = 0.8f; i > 0.2f; i -= 0.25f)
    {
        g.setColour(col1);

        g.drawLine(w * i,
                   h + 1.0f,
                   w + 1.0f,
                   h * i,
                   lineThickness);

        g.setColour(col2);

        g.drawLine(w * i + lineThickness,
                   h + 1.0f,
                   w + 1.0f,
                   h * i + lineThickness,
                   lineThickness);
    }
}

void PluginLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &comboBox)
{
    const auto baseColour = g_secondaryColour.withMultipliedAlpha(comboBox.isEnabled() ? 0.75f : 0.3f);

    if (width > 0 && height > 0)
    {
        const float cornerSize = 1.f;

        juce::Path outline;
        outline.addRoundedRectangle(0.f, 0.f,
                                    width, height, cornerSize, cornerSize,
                                    true, true, true, true);

        g.setGradientFill(juce::ColourGradient(
            baseColour.darker(0.1f), 0.f, height / 2 - 2,
            baseColour.darker(0.2f), 0.f, height / 2 + 2, false));

        g.fillPath(outline);

        if (isButtonDown)
        {
            g.setColour(baseColour.brighter(isButtonDown ? 0.1f : 0.01f));
            g.fillPath(outline);
        }

        constexpr auto outlineAlpha = 0.05f;

        g.setColour(juce::Colours::white.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f));

        g.setColour(juce::Colours::black.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f),
                     juce::AffineTransform::translation(0.f, 1.f).scaled(1.f, (height + 2.f) / height));
    }
}

void PluginLookAndFeel::positionComboBoxText(juce::ComboBox &cb, juce::Label &labelToPosition)
{
    labelToPosition.setBounds(cb.getLocalBounds().reduced(cb.getHeight() / 2, 0));
}

void PluginLookAndFeel::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    g.fillAll(g_secondaryColour);
}

void PluginLookAndFeel::drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String &text, const juce::String &shortcutKeyText, const juce::Drawable *icon, const juce::Colour *textColour)
{
    using namespace juce;

    g.setColour(g_secondaryColour);
    g.fillRect(area);
    g.setFont(juce::Font(g_fontSize));

    if (isSeparator)
    {
        g.setColour(juce::Colour(0xFFFFFF).withAlpha(0.2f));
        const auto separatorRect = Rectangle<int>(10, area.getHeight() / 2 - 0.5f, area.getWidth() - 20, 1);
        g.fillRect(separatorRect);
    }
    else
    {
        if (isHighlighted)
        {
            g.setColour(g_secondaryColour);
            g.fillRect(area.reduced(2, 2));
        }

        g.setColour(juce::Colour(0xFFFFFF).withAlpha(isActive ? 0.8f : 0.2f));
        g.drawText(text, area.reduced(25, 0), Justification::left);

        if (hasSubMenu)
        {
            constexpr int triWidth = 6;
            constexpr int triHeight = 8;

            const int x0 = area.getRight() - triWidth - 10;
            const int x1 = x0 + triWidth;
            const int y0 = area.getY() + area.getHeight() / 2 - triHeight / 2;
            const int y1 = y0 + triHeight / 2;
            const int y2 = y1 + triHeight / 2;

            Path triangle;
            triangle.addTriangle(x0, y0, x0, y2, x1, y1);
            g.fillPath(triangle);
        }

        if (isTicked)
        {
            g.setColour(g_primaryColour);
            g.fillEllipse(10, (area.getHeight() - 5) / 2, 8, 8);
        }
    }
}

void PluginLookAndFeel::drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar, int x, int y, int width, int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
{
    using namespace juce;

    const bool highlight = isMouseOver || isMouseDown;

    g.setColour(Colour::fromFloatRGBA(1.f, 1.f, 1.f, highlight ? 0.6f : 0.4f));

    if (isScrollbarVertical)
    {
        g.fillRoundedRectangle(x, y + thumbStartPosition, width, thumbSize, width / 2);
    }
    else
    {
        g.fillRoundedRectangle(x + thumbStartPosition, y, thumbSize, height, height / 2);
    }
}

void PluginLookAndFeel::drawTabButton(juce::TabBarButton &button, juce::Graphics &g, bool isMouseOver, bool isMouseDown)
{
    using namespace juce;

    const auto currentIndex = button.getTabbedButtonBar().getCurrentTabIndex();
    const auto buttonIndex = button.getIndex();

    const auto selected = currentIndex == buttonIndex;

    const auto baseColour = g_secondaryColour.darker(selected ? 0.25 : 0.0);

    const auto width = float(button.getWidth());
    const auto height = float(button.getHeight());

    if (width > 0 && height > 0)
    {
        const float cornerSize = 1.f;

        juce::Path outline;
        outline.addRoundedRectangle(0.f, 0.f,
                                    width, height, cornerSize, cornerSize,
                                    true, true, true, true);

        g.setGradientFill(juce::ColourGradient(
            baseColour.darker(0.1f), 0.f, height / 2 - 2,
            baseColour.darker(0.2f), 0.f, height / 2 + 2, false));

        g.fillPath(outline);

        if (isMouseDown || isMouseOver)
        {
            g.setColour(baseColour.brighter(isMouseDown ? 0.1f : 0.01f));
            g.fillPath(outline);
        }

        constexpr auto outlineAlpha = 0.05f;

        g.setColour(juce::Colours::white.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f));

        g.setColour(juce::Colours::black.withAlpha(outlineAlpha));
        g.strokePath(outline, juce::PathStrokeType(1.f),
                     juce::AffineTransform::translation(0.f, 1.f).scaled(1.f, (height + 2.f) / height));

        Font font(g_fontSize);
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
                                                              : TextButton::textColourOffId)
                        .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = button.proportionOfHeight(0.1f);

        const int leftIndent = cornerSize / (button.isConnectedOnLeft() ? yIndent * 2 : yIndent);
        const int rightIndent = cornerSize / (button.isConnectedOnRight() ? yIndent * 2 : yIndent);
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                             leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                             Justification::centred, 2);
    }
}

juce::Font PluginLookAndFeel::getTextButtonFont(juce::TextButton &, int buttonHeight)
{
    return juce::Font(g_fontSize);
}

juce::Font PluginLookAndFeel::getLabelFont(juce::Label &)
{
    return juce::Font(g_fontSize);
}

void PluginLookAndFeel::drawButtonText(juce::Graphics &g, juce::TextButton &button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
    using namespace juce;
    Font font(getTextButtonFont(button, button.getHeight()));
    g.setFont(font);
    g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
                                                          : TextButton::textColourOffId)
                    .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    const int yIndent = button.proportionOfHeight(0.1f);
    const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

    const int leftIndent = cornerSize / (button.isConnectedOnLeft() ? yIndent * 2 : yIndent);
    const int rightIndent = cornerSize / (button.isConnectedOnRight() ? yIndent * 2 : yIndent);
    const int textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText(button.getButtonText(),
                         leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                         Justification::centred, 2);
}