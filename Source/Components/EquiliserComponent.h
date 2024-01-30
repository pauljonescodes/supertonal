#pragma once

#include <JuceHeader.h>

class EqualiserComponent : public juce::Component
{
public:
    struct ParameterSetting {
        std::string parameterId;
        std::string title;
        std::string suffix;
    };

    explicit EqualiserComponent(
        juce::AudioProcessorValueTreeState& apvts,
        const std::string& title,
        const std::vector<ParameterSetting>& parameterSettings,
        const std::string& toggleParameterId) noexcept
    {
        mGroupComponentPtr.reset(new juce::GroupComponent(title, title));
        addAndMakeVisible(mGroupComponentPtr.get());

        for (const auto& setting : parameterSettings) {
            auto sliderPtr = std::make_unique<juce::Slider>(juce::Slider::LinearVertical, juce::Slider::TextBoxBelow);
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

    ~EqualiserComponent()
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

        juce::FlexBox mainFlexBox;
        mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        mainFlexBox.alignContent = juce::FlexBox::AlignContent::stretch;

        mainFlexBox.items.add(juce::FlexItem().withFlex(0.3));

        juce::FlexBox row1FlexBox;
        row1FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row1FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row1FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        row1FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
        row1FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

        int row1Count = 0;
        for (auto row1Index = 0; row1Index < mSliderPtrs.size(); ++row1Index)
        {
            auto& sliderPtr = mSliderPtrs[row1Index];

            row1FlexBox.items.add(juce::FlexItem(*sliderPtr).withFlex(1));

            row1Count++;
        }

        mainFlexBox.items.add(juce::FlexItem(row1FlexBox).withFlex(2));

        // FlexBox for the bottom 1/3 where toggle button will be
        juce::FlexBox row3FlexBox;
        row3FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row3FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row3FlexBox.alignItems = juce::FlexBox::AlignItems::center;
        row3FlexBox.alignContent = juce::FlexBox::AlignContent::center;

        auto toggleButtonSize = juce::jmin(bounds.getWidth(), bounds.getHeight() / 3) / 4;
        juce::FlexItem toggleFlexItem(toggleButtonSize, toggleButtonSize, *mToggleButtonPtr);
        row3FlexBox.items.add(toggleFlexItem);

        mainFlexBox.items.add(juce::FlexItem(row3FlexBox).withFlex(1)); // 1/3 of space

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