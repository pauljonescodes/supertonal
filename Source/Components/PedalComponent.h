/*
  ==============================================================================

    PedalComponent.h
    Created: 28 Jan 2024 4:31:28pm
    Author:  paulm

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PedalComponent : public juce::Component
{
public:
    struct ParameterSetting {
        std::string parameterId;
        std::string title;
        std::string suffix;
    };

    explicit PedalComponent(
        juce::AudioProcessorValueTreeState& apvts,
        const std::string& title,
        const std::vector<ParameterSetting>& parameterSettings,
        const std::string& toggleParameterId) noexcept
    {
        mGroupComponentPtr.reset(new juce::GroupComponent(title, title));
        addAndMakeVisible(mGroupComponentPtr.get());

        for (const auto& setting : parameterSettings) {
            auto sliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
            sliderPtr->setTitle(setting.title);
            sliderPtr->setTextValueSuffix(setting.suffix);
            sliderPtr->setScrollWheelEnabled(false);

            auto attachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, setting.parameterId, *sliderPtr);

            auto labelPtr = std::make_unique<juce::Label>();
            labelPtr->setText(setting.title, juce::dontSendNotification);
            labelPtr->attachToComponent(sliderPtr.get(), false);

            addAndMakeVisible(sliderPtr.get());
            addAndMakeVisible(labelPtr.get());

            mSliderPtrs.push_back(std::move(sliderPtr));
            mLabelPtrs.push_back(std::move(labelPtr));
            mAttachmentPtrs.push_back(std::move(attachmentPtr));
        }

        // Toggle Button
        mToggleButtonPtr = std::make_unique<juce::ToggleButton>();
        mToggleButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, toggleParameterId, *mToggleButtonPtr);
        addAndMakeVisible(mToggleButtonPtr.get());
    }

    ~PedalComponent()
    {
        mSliderPtrs.clear();
        mLabelPtrs.clear();
        mAttachmentPtrs.clear();
        mToggleButtonPtr.reset();
        mToggleButtonAttachmentPtr.reset();
        mGroupComponentPtr.reset();
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(10);
        mGroupComponentPtr->setBounds(bounds);

        // FlexBox for the whole component
        juce::FlexBox mainFlexBox;
        mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        mainFlexBox.alignContent = juce::FlexBox::AlignContent::stretch;

        // FlexBox for the top 2/3 where sliders will be in a grid
        juce::FlexBox slidersFlexBox;
        slidersFlexBox.flexDirection = juce::FlexBox::Direction::row;
        slidersFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        slidersFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        slidersFlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
        slidersFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

        for (auto& sliderPtr : mSliderPtrs) {           
            slidersFlexBox.items.add(juce::FlexItem(*sliderPtr).withFlex(1).withMaxHeight(128));
        }

        mainFlexBox.items.add(juce::FlexItem().withFlex(0.33));
        mainFlexBox.items.add(juce::FlexItem(slidersFlexBox).withFlex(2)); // 2/3 of space

        // FlexBox for the bottom 1/3 where toggle button will be
        juce::FlexBox toggleFlexBox;
        toggleFlexBox.flexDirection = juce::FlexBox::Direction::row;
        toggleFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        toggleFlexBox.alignItems = juce::FlexBox::AlignItems::center;
        toggleFlexBox.alignContent = juce::FlexBox::AlignContent::center;

        auto toggleButtonSize = juce::jmin(bounds.getWidth(), bounds.getHeight() / 3) / 4;
        juce::FlexItem toggleFlexItem(toggleButtonSize, toggleButtonSize, *mToggleButtonPtr);
        toggleFlexBox.items.add(toggleFlexItem);

        mainFlexBox.items.add(juce::FlexItem(toggleFlexBox).withFlex(1)); // 1/3 of space

        // Perform layout for the main FlexBox
        mainFlexBox.performLayout(bounds);
    }

private:
    std::unique_ptr<juce::GroupComponent> mGroupComponentPtr;
    std::vector<std::unique_ptr<juce::Slider>> mSliderPtrs;
    std::vector<std::unique_ptr<juce::Label>> mLabelPtrs;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> mAttachmentPtrs;

    // Toggle Button
    std::unique_ptr<juce::ToggleButton> mToggleButtonPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mToggleButtonAttachmentPtr;
};