#pragma once

#include <JuceHeader.h>

class DelayComponent : public juce::Component, public juce::AudioProcessorValueTreeState::Listener
{
public:

    explicit DelayComponent(
        juce::AudioProcessorValueTreeState& apvts,
        const std::string& title,
        const std::string& delayLeftPerBeatParameterId,
        const std::string& delayRightPerBeatParameterId,
        const std::string& delayLeftMillisecondParameterId,
        const std::string& delayRightMillisecondParameterId,
        const std::string& delayHighPassFrequencyParameterId,
        const std::string& delayLowPassFrequencyParameterId,
        const std::string& delayFeedbackParameterId,
        const std::string& delayDryWetParameterId,
        const std::string& delayIsSyncedParameterId,
        const std::string& toggleParameterId) noexcept :
        mApvts(apvts)
    {

        // Start delayLeftPerBeatParameterId
        mLeftPerBeatSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mLeftPerBeatSliderPtr->setTextValueSuffix(" division");
        mLeftPerBeatSliderPtr->setScrollWheelEnabled(false);

        mLeftPerBeatAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayLeftPerBeatParameterId, *mLeftPerBeatSliderPtr);

        mLeftPerBeatLabelPtr = std::make_unique<juce::Label>();
        mLeftPerBeatLabelPtr->setText("Left", juce::dontSendNotification);
        mLeftPerBeatLabelPtr->attachToComponent(mLeftPerBeatSliderPtr.get(), false);

        addAndMakeVisible(mLeftPerBeatSliderPtr.get());
        addAndMakeVisible(mLeftPerBeatLabelPtr.get());
        // End delayLeftPerBeatParameterId

        // Start delayRightPerBeatParameterId
        mRightPerBeatSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mRightPerBeatSliderPtr->setTextValueSuffix(" division");
        mRightPerBeatSliderPtr->setScrollWheelEnabled(false);

        mRightPerBeatAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayRightPerBeatParameterId, *mRightPerBeatSliderPtr);

        mRightPerBeatLabelPtr = std::make_unique<juce::Label>();
        mRightPerBeatLabelPtr->setText("Right", juce::dontSendNotification);
        mRightPerBeatLabelPtr->attachToComponent(mRightPerBeatSliderPtr.get(), false);

        addAndMakeVisible(mRightPerBeatSliderPtr.get());
        addAndMakeVisible(mRightPerBeatLabelPtr.get());
        // End delayRightPerBeatParameterId

        // Start delayLeftMillisecondParameterId
        mLeftMillisecondSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mLeftMillisecondSliderPtr->setTextValueSuffix(" ms");
        mLeftMillisecondSliderPtr->setScrollWheelEnabled(false);

        mLeftMillisecondAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayLeftMillisecondParameterId, *mLeftMillisecondSliderPtr);

        mLeftMillisecondLabelPtr = std::make_unique<juce::Label>();
        mLeftMillisecondLabelPtr->setText("Left", juce::dontSendNotification);
        mLeftMillisecondLabelPtr->attachToComponent(mLeftMillisecondSliderPtr.get(), false);

        addAndMakeVisible(mLeftMillisecondSliderPtr.get());
        addAndMakeVisible(mLeftMillisecondLabelPtr.get());
        // End delayLeftMillisecondParameterId

        // Start delayRightMillisecondParameterId
        mRightMillisecondSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mRightMillisecondSliderPtr->setTextValueSuffix(" ms");
        mRightMillisecondSliderPtr->setScrollWheelEnabled(false);

        mRightMillisecondAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayRightMillisecondParameterId, *mRightMillisecondSliderPtr);

        mRightMillisecondLabelPtr = std::make_unique<juce::Label>();
        mRightMillisecondLabelPtr->setText("Right", juce::dontSendNotification);
        mRightMillisecondLabelPtr->attachToComponent(mRightMillisecondSliderPtr.get(), false);

        addAndMakeVisible(mRightMillisecondSliderPtr.get());
        addAndMakeVisible(mRightMillisecondLabelPtr.get());
        // End delayRightMillisecondParameterId

        // Start delayHighPassFrequencyParameterId
        mHighPassFrequencySliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mHighPassFrequencySliderPtr->setTextValueSuffix(" Hz");
        mHighPassFrequencySliderPtr->setScrollWheelEnabled(false);

        mHighPassFrequencyAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayHighPassFrequencyParameterId, *mHighPassFrequencySliderPtr);

        mHighPassFrequencyLabelPtr = std::make_unique<juce::Label>();
        mHighPassFrequencyLabelPtr->setText("HPF", juce::dontSendNotification);
        mHighPassFrequencyLabelPtr->attachToComponent(mHighPassFrequencySliderPtr.get(), false);

        addAndMakeVisible(mHighPassFrequencySliderPtr.get());
        addAndMakeVisible(mHighPassFrequencyLabelPtr.get());
        // End delayHighPassFrequencyParameterId

        // Start delayLowPassFrequencyParameterId
        mLowPassFrequencySliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        mLowPassFrequencySliderPtr->setTextValueSuffix(" Hz");
        mLowPassFrequencySliderPtr->setScrollWheelEnabled(false);

        mLowPassFrequencyAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayLowPassFrequencyParameterId, *mLowPassFrequencySliderPtr);

        mLowPassFrequencyLabelPtr = std::make_unique<juce::Label>();
        mLowPassFrequencyLabelPtr->setText("LPF", juce::dontSendNotification);
        mLowPassFrequencyLabelPtr->attachToComponent(mLowPassFrequencySliderPtr.get(), false);

        addAndMakeVisible(mLowPassFrequencySliderPtr.get());
        addAndMakeVisible(mLowPassFrequencyLabelPtr.get());
        // End delayLowPassFrequencyParameterId

        // Start delayFeedbackParameterId
        mFeedbackSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        //mFeedbackSliderPtr->setTextValueSuffix(" %");
        mFeedbackSliderPtr->setScrollWheelEnabled(false);

        mFeedbackAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayFeedbackParameterId, *mFeedbackSliderPtr);

        mFeedbackLabelPtr = std::make_unique<juce::Label>();
        mFeedbackLabelPtr->setText("Feedback", juce::dontSendNotification);
        mFeedbackLabelPtr->attachToComponent(mFeedbackSliderPtr.get(), false);

        addAndMakeVisible(mFeedbackSliderPtr.get());
        addAndMakeVisible(mFeedbackLabelPtr.get());
        // End delayFeedbackParameterId

        // Start delayDryWetParameterId
        mDryWetSliderPtr = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
        //mDryWetSliderPtr->setTextValueSuffix(" %");
        mDryWetSliderPtr->setScrollWheelEnabled(false);

        mDryWetAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, delayDryWetParameterId, *mDryWetSliderPtr);

        mDryWetLabelPtr = std::make_unique<juce::Label>();
        mDryWetLabelPtr->setText("Mix", juce::dontSendNotification);
        mDryWetLabelPtr->attachToComponent(mDryWetSliderPtr.get(), false);

        addAndMakeVisible(mDryWetSliderPtr.get());
        addAndMakeVisible(mDryWetLabelPtr.get());
        // End delayDryWetParameterId

        mGroupComponentPtr.reset(new juce::GroupComponent(title, title));
        addAndMakeVisible(mGroupComponentPtr.get());

        // Sync Button
        mDelayIsSyncedParameterId = delayIsSyncedParameterId;
        mSyncButtonPtr = std::make_unique<juce::ToggleButton>("BPM Sync");
        mSyncButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, delayIsSyncedParameterId, *mSyncButtonPtr);
        addAndMakeVisible(mSyncButtonPtr.get());

        apvts.addParameterListener(mDelayIsSyncedParameterId, this);

        // Toggle Button
        mToggleButtonPtr = std::make_unique<juce::ToggleButton>();
        mToggleButtonAttachmentPtr = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, toggleParameterId, *mToggleButtonPtr);
        addAndMakeVisible(mToggleButtonPtr.get());
    }

    void parameterChanged(const juce::String& parameterID, float newValue) override
    {
        mDelayIsSynced = newValue;
        resized();
    }

    ~DelayComponent()
    {
        // Reset sliders
        mLeftPerBeatSliderPtr.reset();
        mRightPerBeatSliderPtr.reset();
        mLeftMillisecondSliderPtr.reset();
        mRightMillisecondSliderPtr.reset();
        mHighPassFrequencySliderPtr.reset();
        mLowPassFrequencySliderPtr.reset();
        mFeedbackSliderPtr.reset();
        mDryWetSliderPtr.reset();

        // Reset labels
        mLeftPerBeatLabelPtr.reset();
        mRightPerBeatLabelPtr.reset();
        mLeftMillisecondLabelPtr.reset();
        mRightMillisecondLabelPtr.reset();
        mHighPassFrequencyLabelPtr.reset();
        mLowPassFrequencyLabelPtr.reset();
        mFeedbackLabelPtr.reset();
        mDryWetLabelPtr.reset();

        // Reset attachments
        mLeftPerBeatAttachmentPtr.reset();
        mRightPerBeatAttachmentPtr.reset();
        mLeftMillisecondAttachmentPtr.reset();
        mRightMillisecondAttachmentPtr.reset();
        mHighPassFrequencyAttachmentPtr.reset();
        mLowPassFrequencyAttachmentPtr.reset();
        mFeedbackAttachmentPtr.reset();
        mDryWetAttachmentPtr.reset();

        // Reset any additional controls not previously mentioned
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

        mainFlexBox.items.add(juce::FlexItem().withFlex(0.1));

        juce::FlexBox row1FlexBox;
        row1FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row1FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row1FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        row1FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
        row1FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

        row1FlexBox.items.add(juce::FlexItem(*mSyncButtonPtr).withFlex(1).withMaxHeight(44));
        mainFlexBox.items.add(juce::FlexItem(row1FlexBox).withFlex(0.5));

        juce::FlexBox row2FlexBox;
        row2FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row2FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row2FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        row2FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
        row2FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

        row2FlexBox.items.add(juce::FlexItem(*mFeedbackSliderPtr).withFlex(1).withMaxHeight(128));

        mLeftPerBeatSliderPtr->setVisible(mDelayIsSynced);
        mRightPerBeatSliderPtr->setVisible(mDelayIsSynced);

        mLeftMillisecondSliderPtr->setVisible(!mDelayIsSynced);
        mRightMillisecondSliderPtr->setVisible(!mDelayIsSynced);

        if (mDelayIsSynced)
        {
            row2FlexBox.items.add(juce::FlexItem(*mLeftPerBeatSliderPtr).withFlex(1).withMaxHeight(128));
            row2FlexBox.items.add(juce::FlexItem(*mRightPerBeatSliderPtr).withFlex(1).withMaxHeight(128));
        }
        else
        {
            row2FlexBox.items.add(juce::FlexItem(*mLeftMillisecondSliderPtr).withFlex(1).withMaxHeight(128));
            row2FlexBox.items.add(juce::FlexItem(*mRightMillisecondSliderPtr).withFlex(1).withMaxHeight(128));
        }

        mainFlexBox.items.add(juce::FlexItem(row2FlexBox).withFlex(1));
        mainFlexBox.items.add(juce::FlexItem().withFlex(0.1));

        juce::FlexBox row3FlexBox;
        row3FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row3FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row3FlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        row3FlexBox.alignContent = juce::FlexBox::AlignContent::stretch;
        row3FlexBox.flexWrap = juce::FlexBox::Wrap::wrap;

        row3FlexBox.items.add(juce::FlexItem(*mDryWetSliderPtr).withFlex(1).withMaxHeight(128));
        row3FlexBox.items.add(juce::FlexItem(*mHighPassFrequencySliderPtr).withFlex(1).withMaxHeight(128));
        row3FlexBox.items.add(juce::FlexItem(*mLowPassFrequencySliderPtr).withFlex(1).withMaxHeight(128));

        mainFlexBox.items.add(juce::FlexItem(row3FlexBox).withFlex(1)); // 2/3 of space

        // FlexBox for the bottom 1/3 where toggle button will be
        juce::FlexBox row4FlexBox;
        row4FlexBox.flexDirection = juce::FlexBox::Direction::row;
        row4FlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        row4FlexBox.alignItems = juce::FlexBox::AlignItems::center;
        row4FlexBox.alignContent = juce::FlexBox::AlignContent::center;

        auto toggleButtonSize = juce::jmin(bounds.getWidth(), bounds.getHeight() / 3) / 4;
        juce::FlexItem toggleFlexItem(toggleButtonSize, toggleButtonSize, *mToggleButtonPtr);
        row4FlexBox.items.add(toggleFlexItem);

        mainFlexBox.items.add(juce::FlexItem(row4FlexBox).withFlex(1)); // 1/3 of space

        // Perform layout for the main FlexBox
        mainFlexBox.performLayout(bounds);
    }

private:
    juce::AudioProcessorValueTreeState& mApvts;

    std::string mDelayIsSyncedParameterId;

    bool mDelayIsSynced = false;

    std::unique_ptr<juce::GroupComponent> mGroupComponentPtr;
    
    std::unique_ptr<juce::Slider> mLeftPerBeatSliderPtr;
    std::unique_ptr<juce::Label> mLeftPerBeatLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mLeftPerBeatAttachmentPtr;

    std::unique_ptr<juce::Slider> mRightPerBeatSliderPtr;
    std::unique_ptr<juce::Label> mRightPerBeatLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mRightPerBeatAttachmentPtr;

    std::unique_ptr<juce::Slider> mLeftMillisecondSliderPtr;
    std::unique_ptr<juce::Label> mLeftMillisecondLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mLeftMillisecondAttachmentPtr;

    std::unique_ptr<juce::Slider> mRightMillisecondSliderPtr;
    std::unique_ptr<juce::Label> mRightMillisecondLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mRightMillisecondAttachmentPtr;

    std::unique_ptr<juce::Slider> mHighPassFrequencySliderPtr;
    std::unique_ptr<juce::Label> mHighPassFrequencyLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHighPassFrequencyAttachmentPtr;

    std::unique_ptr<juce::Slider> mLowPassFrequencySliderPtr;
    std::unique_ptr<juce::Label> mLowPassFrequencyLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mLowPassFrequencyAttachmentPtr;

    std::unique_ptr<juce::Slider> mFeedbackSliderPtr;
    std::unique_ptr<juce::Label> mFeedbackLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mFeedbackAttachmentPtr;

    std::unique_ptr<juce::Slider> mDryWetSliderPtr;
    std::unique_ptr<juce::Label> mDryWetLabelPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDryWetAttachmentPtr;

    std::unique_ptr<juce::ToggleButton> mSyncButtonPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mSyncButtonAttachmentPtr;
    
    std::unique_ptr<juce::ToggleButton> mToggleButtonPtr;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mToggleButtonAttachmentPtr;
};